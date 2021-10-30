#pragma once
#include "AL/Common.hpp"

#include "Timer.hpp"

#include "SystemException.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <unistd.h>

	#if __has_include(<pthread.h>)
		#define AL_DEPENDENCY_PTHREAD

		extern "C"
		{
			#include <pthread.h>
		}
	#endif

	#include <sys/syscall.h>
#endif

namespace AL::OS
{
	class Thread
	{
		class INativeThread
		{
			INativeThread(INativeThread&&) = delete;
			INativeThread(const INativeThread&) = delete;

		public:
			INativeThread()
			{
			}

			virtual ~INativeThread()
			{
			}

			virtual Bool IsRunning() const = 0;

			virtual Bool IsDetatched() const = 0;

			virtual Void* GetHandle() const = 0;

			virtual Void Start() = 0;

			virtual Void Detatch() = 0;

			virtual Void Terminate() = 0;

			// @return False if maxWaitTime elapsed
			virtual Bool Join(TimeSpan maxWaitTime) = 0;
		};

		template<typename F>
		class NativeThread
			: public INativeThread
		{
			Bool isRunning = False;
			Bool isDetatched = False;

			F function;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
			::pthread_t pthread;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			::HANDLE    hThread = NULL;
#endif

		public:
			explicit NativeThread(F&& function)
				: function(
					Move(function)
				)
			{
			}

			virtual ~NativeThread()
			{
#if defined(AL_PLATFORM_WINDOWS)
				if (hThread != NULL)
				{

					::CloseHandle(
						hThread
					);
				}
#endif
			}

			virtual Bool IsRunning() const override
			{
				return isRunning;
			}

			virtual Bool IsDetatched() const override
			{
				return isDetatched;
			}

			virtual Void* GetHandle() const override
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
				return const_cast<::pthread_t*>(
					&pthread
				);
	#else
				return nullptr;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
				return hThread;
#else
				return nullptr;
#endif
			}

			// @throw AL::Exception
			virtual Void Start() override
			{
				AL_ASSERT(
					!IsRunning(),
					"NativeThread already running"
				);

				auto function_detour = [](Void* _lpParam)
				{
					auto lpNativeThread = reinterpret_cast<NativeThread*>(
						_lpParam
					);

					lpNativeThread->function();

					lpNativeThread->isRunning = False;

					if (lpNativeThread->IsDetatched())
					{
#if defined(AL_PLATFORM_WINDOWS)
						::CloseHandle(
							lpNativeThread->hThread
						);
#endif

						delete lpNativeThread;
					}

#if defined(AL_PLATFORM_LINUX)
					return (Void*)(nullptr);
#elif defined(AL_PLATFORM_WINDOWS)
					return (DWORD)(0);
#endif
				};

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
				ErrorCode errorCode;

				if ((errorCode = ::pthread_create(&pthread, nullptr, function_detour, this)) != 0)
				{

					throw SystemException(
						"pthread_create",
						errorCode
					);
				}
	#else
				throw DependencyMissingException(
					"pthread"
				);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
				if (hThread != NULL)
				{

					::CloseHandle(
						hThread
					);
				}

				if ((hThread = ::CreateThread(nullptr, 0, function_detour, this, 0, nullptr)) == NULL)
				{

					throw SystemException(
						"CreateThread"
					);
				}
#else
				throw PlatformNotSupportedException();
#endif

				isRunning = True;
			}

			// @throw AL::Exception
			virtual Void Detatch() override
			{
				AL_ASSERT(
					IsRunning(),
					"NativeThread not running"
				);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
				ErrorCode errorCode;

				if ((errorCode = ::pthread_detach(pthread)) != 0)
				{

					throw SystemException(
						"pthread_detach",
						errorCode
					);
				}
	#else
				throw NotImplementedException();
	#endif
#elif defined(AL_PLATFORM_WINDOWS)

#else
				throw PlatformNotSupportedException();
#endif

				isDetatched = True;
			}

			// @throw AL::Exception
			virtual Void Terminate() override
			{
				AL_ASSERT(
					IsRunning(),
					"NativeThread not running"
				);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
				ErrorCode errorCode;

				if ((errorCode = ::pthread_cancel(pthread)) != 0)
				{

					throw SystemException(
						"pthread_cancel",
						errorCode
					);
				}
	#else
				throw NotImplementedException();
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
				if (::TerminateThread(hThread, 0) == 0)
				{

					throw SystemException(
						"TerminateThread"
					);
				}

				::CloseHandle(
					hThread
				);

				hThread = NULL;
#else
				throw PlatformNotSupportedException();
#endif

				isRunning = False;
			}

			// @throw AL::Exception
			// @return False if maxWaitTime elapsed
			virtual Bool Join(TimeSpan maxWaitTime) override
			{
				AL_ASSERT(
					IsRunning(),
					"NativeThread not running"
				);

				Timer timer;

				while (timer.GetElapsed() < maxWaitTime)
				{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
					ErrorCode errorCode;
					Void*     threadRetValue;

					if ((errorCode = ::pthread_join(pthread, &threadRetValue)) != 0)
					{
						if (errorCode == ESRCH)
						{

							return True;
						}

						throw SystemException(
							"pthread_join",
							errorCode
						);
					}
	#else
					throw NotImplementedException();
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
					if (::WaitForSingleObject(hThread, static_cast<::DWORD>((maxWaitTime - timer.GetElapsed()).ToMilliseconds())) == 0)
					{

						throw SystemException(
							"WaitForSingleObject"
						);
					}
#else
					throw PlatformNotSupportedException();
#endif
				}

				return !IsRunning();
			}
		};

		INativeThread*     lpNativeThread;

		Thread(Thread&&) = delete;
		Thread(const Thread&) = delete;

	public:
		// @throw AL::Exception
		template<typename F>
		static Void StartAndDetach(F&& function)
		{
			Thread thread;

			thread.Start(
				Move(function)
			);

			try
			{
				thread.Detatch();
			}
			catch (Exception& exception)
			{
				try
				{
					thread.Terminate();
				}
				catch (Exception& innerException)
				{

					throw Exception(
						Move(exception),
						Move(innerException)
					);
				}

				throw;
			}
		}
		// @throw AL::Exception
		static Void StartAndDetach(Void(*lpFunction)())
		{
			StartAndDetach(
				[lpFunction]()
				{
					(*lpFunction)();
				}
			);
		}
		// @throw AL::Exception
		template<typename C>
		static Void StartAndDetach(Void(C::*lpFunction)(), C* lpInstance)
		{
			StartAndDetach(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}
		// @throw AL::Exception
		template<typename C>
		static Void StartAndDetach(Void(C::*lpFunction)() const, const C* lpInstance)
		{
			StartAndDetach(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}

		Thread()
			: lpNativeThread(
				nullptr
			)
		{
		}

		virtual ~Thread()
		{
			if (lpNativeThread != nullptr)
			{
				delete lpNativeThread;
				lpNativeThread = nullptr;
			}
		}

		Bool IsRunning() const
		{
			return (lpNativeThread != nullptr) ? lpNativeThread->IsRunning() : False;
		}

		auto GetHandle() const
		{
			return (lpNativeThread != nullptr) ? lpNativeThread->GetHandle() : nullptr;
		}

		// @throw AL::Exception
		template<typename F>
		Void Start(F&& function)
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			if (lpNativeThread != nullptr)
			{

				delete lpNativeThread;
			}

			lpNativeThread = new NativeThread<F>(
				Move(function)
			);

			try
			{
				lpNativeThread->Start();
			}
			catch (Exception&)
			{
				delete lpNativeThread;
				lpNativeThread = nullptr;

				throw;
			}
		}
		// @throw AL::Exception
		Void Start(Void(*lpFunction)())
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			Start(
				[lpFunction]()
				{
					(*lpFunction)();
				}
			);
		}
		// @throw AL::Exception
		template<typename C>
		Void Start(Void(C::*lpFunction)(), C* lpInstance)
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			Start(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}
		// @throw AL::Exception
		template<typename C>
		Void Start(Void(C::*lpFunction)() const, const C* lpInstance)
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			Start(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}

		// @throw AL::Exception
		Void Detatch()
		{
			AL_ASSERT(
				IsRunning(),
				"Thread not running"
			);

			lpNativeThread->Detatch();
			lpNativeThread = nullptr;
		}

		// @throw AL::Exception
		Void Terminate()
		{
			if (IsRunning())
			{
				lpNativeThread->Terminate();

				delete lpNativeThread;
				lpNativeThread = nullptr;
			}
		}

		// @throw AL::Exception
		// @return False if time elapsed and thread is still running
		Bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			if (IsRunning() && !lpNativeThread->Join(maxWaitTime))
			{

				return False;
			}

			return True;
		}
	};

	inline uint32 GetCurrentThreadId()
	{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_PTHREAD)
		return static_cast<uint32>(
			::syscall(SYS_gettid)
		);
	#else
		throw DependencyMissingException(
			"pthread"
		);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
		return static_cast<uint32>(
			::GetCurrentThreadId()
		);
#else
		throw PlatformNotSupportedException();
#endif
	}
}
