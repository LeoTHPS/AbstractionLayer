#pragma once
#include "AL/Common.hpp"

#include "Timer.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <unistd.h>
	#include <pthread.h>

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

			virtual bool IsRunning() const = 0;

			virtual bool IsDetatched() const = 0;

			// @throw AL::Exceptions::Exception
			virtual void Start() = 0;

			// @throw AL::Exceptions::Exception
			virtual void Detatch() = 0;

			// @throw AL::Exceptions::Exception
			virtual void Terminate() = 0;

			// @throw AL::Exceptions::Exception
			// @return false if maxWaitTime elapsed
			virtual bool Join(TimeSpan maxWaitTime) = 0;
		};

		template<typename F>
		class NativeThread
			: public INativeThread
		{
			bool isRunning = false;
			bool isDetatched = false;

			F function;

#if defined(AL_PLATFORM_LINUX)
			pthread_t pthread;
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hThread;
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
			}

			virtual bool IsRunning() const override
			{
				return isRunning;
			}

			virtual bool IsDetatched() const override
			{
				return isDetatched;
			}

			// @throw AL::Exceptions::Exception
			virtual void Start() override
			{
				AL_ASSERT(!IsRunning(), "NativeThread already running");

				auto function_detour = [](void* _lpParam)
				{
					auto lpNativeThread = reinterpret_cast<NativeThread*>(
						_lpParam
					);

					lpNativeThread->function();

					lpNativeThread->isRunning = false;

#if defined(AL_PLATFORM_LINUX)
					if (lpNativeThread->IsDetatched())
					{

						delete lpNativeThread;
					}

					pthread_exit(NULL);

					return (void*)(nullptr);
#elif defined(AL_PLATFORM_WINDOWS)
					CloseHandle(
						lpNativeThread->hThread
					);

					if (lpNativeThread->IsDetatched())
					{

						delete lpNativeThread;
					}

					return DWORD(0);
#endif
				};

#if defined(AL_PLATFORM_LINUX)
				OS::ErrorCode errorCode;

				if ((errorCode = pthread_create(&pthread, nullptr, function_detour, this)) != 0)
				{

					throw Exceptions::SystemException(
						"pthread_create",
						errorCode
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				if ((hThread = CreateThread(nullptr, 0, function_detour, this, 0, nullptr)) == NULL)
				{

					throw Exceptions::SystemException(
						"CreateThread"
					);
				}
#endif

				isRunning = true;
			}

			// @throw AL::Exceptions::Exception
			virtual void Detatch() override
			{
				AL_ASSERT(IsRunning(), "NativeThread not running");

#if defined(AL_PLATFORM_LINUX)
				OS::ErrorCode errorCode;

				if ((errorCode = pthread_detach(pthread)) != 0)
				{

					throw Exceptions::SystemException(
						"pthread_detach",
						errorCode
					);
				}
#endif

				isDetatched = true;
			}

			// @throw AL::Exceptions::Exception
			virtual void Terminate() override
			{
				AL_ASSERT(IsRunning(), "NativeThread not running");

#if defined(AL_PLATFORM_LINUX)
				OS::ErrorCode errorCode;

				if ((errorCode = pthread_cancel(pthread)) != 0)
				{

					throw Exceptions::SystemException(
						"pthread_cancel",
						errorCode
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				if (TerminateThread(hThread, 0) == 0)
				{

					throw Exceptions::SystemException(
						"TerminateThread"
					);
				}

				CloseHandle(
					hThread
				);
#endif

				isRunning = false;
			}

			// @throw AL::Exceptions::Exception
			// @return false if maxWaitTime elapsed
			virtual bool Join(TimeSpan maxWaitTime) override
			{
				AL_ASSERT(IsRunning(), "NativeThread not running");

				Timer timer;

				while (timer.GetElapsed() < maxWaitTime)
				{
#if defined(AL_PLATFORM_LINUX)
					void* threadRetValue;
					OS::ErrorCode errorCode;

					if ((errorCode = pthread_join(pthread, &threadRetValue)) != 0)
					{

						throw Exceptions::SystemException(
							"pthread_join",
							errorCode
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (WaitForSingleObject(hThread, static_cast<DWORD>((maxWaitTime - timer.GetElapsed()).ToMilliseconds())) == 0)
					{

						throw Exceptions::SystemException(
							"WaitForSingleObject"
						);
					}
#endif
				}

				return !IsRunning();
			}
		};

		INativeThread* lpNativeThread;

		Thread(const Thread&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void StartAndDetach(F&& function)
		{
			Thread thread;

			thread.Start(
				Move(function)
			);

			try
			{
				thread.Detatch();
			}
			catch (Exceptions::Exception& exception)
			{
				try
				{
					thread.Terminate();
				}
				catch (Exceptions::Exception& innerException)
				{

					throw Exceptions::Exception(
						Move(exception),
						Move(innerException)
					);
				}

				throw;
			}
		}
		// @throw AL::Exceptions::Exception
		static void StartAndDetach(void(*lpFunction)())
		{
			StartAndDetach(
				[lpFunction]()
				{
					(*lpFunction)();
				}
			);
		}
		// @throw AL::Exceptions::Exception
		template<typename C>
		static void StartAndDetach(void(C::*lpFunction)(), C* lpInstance)
		{
			StartAndDetach(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}
		// @throw AL::Exceptions::Exception
		template<typename C>
		static void StartAndDetach(void(C::*lpFunction)() const, const C* lpInstance)
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

		Thread(Thread&& thread)
			: lpNativeThread(
				Move(thread.lpNativeThread)
			)
		{
			thread.lpNativeThread = nullptr;
		}

		virtual ~Thread()
		{
			if (lpNativeThread != nullptr)
			{
				delete lpNativeThread;
			}
		}

		bool IsRunning() const
		{
			return (lpNativeThread != nullptr) ? lpNativeThread->IsRunning() : false;
		}

		// @throw AL::Exceptions::Exception
		template<typename F>
		void Start(F&& function)
		{
			AL_ASSERT(!IsRunning(), "Thread already running");

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
			catch (Exceptions::Exception&)
			{
				delete lpNativeThread;
				lpNativeThread = nullptr;

				throw;
			}
		}
		// @throw AL::Exceptions::Exception
		void Start(void(*lpFunction)())
		{
			Start(
				[lpFunction]()
				{
					(*lpFunction)();
				}
			);
		}
		// @throw AL::Exceptions::Exception
		template<typename C>
		void Start(void(C::*lpFunction)(), C* lpInstance)
		{
			Start(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}
		// @throw AL::Exceptions::Exception
		template<typename C>
		void Start(void(C::*lpFunction)() const, const C* lpInstance)
		{
			Start(
				[lpInstance, lpFunction]()
				{
					(lpInstance->*lpFunction)();
				}
			);
		}
		
		// @throw AL::Exceptions::Exception
		void Detatch()
		{
			AL_ASSERT(IsRunning(), "Thread not running");

			lpNativeThread->Detatch();
			lpNativeThread = nullptr;
		}

		// @throw AL::Exceptions::Exception
		void Terminate()
		{
			AL_ASSERT(IsRunning(), "Thread not running");

			lpNativeThread->Terminate();

			delete lpNativeThread;
			lpNativeThread = nullptr;
		}

		// @throw AL::Exceptions::Exception
		// @return false if time elapsed and thread is still running
		bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(IsRunning(), "Thread not running");

			return lpNativeThread->Join(
				maxWaitTime
			);
		}
	};

	inline uint32 GetCurrentThreadId()
	{
#if defined(AL_PLATFORM_LINUX)
		return static_cast<uint32>(
			::syscall(SYS_gettid)
		);
#elif defined(AL_PLATFORM_WINDOWS)
		return static_cast<uint32>(
			::GetCurrentThreadId()
		);
#else
		throw Exceptions::NotImplementedException();
#endif
	}
}
