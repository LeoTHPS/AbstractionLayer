#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/Timer.hpp"
#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
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

			virtual ::HANDLE GetHandle() const = 0;

			// @throw AL::Exception
			virtual Void Start() = 0;

			// @throw AL::Exception
			virtual Void Detatch() = 0;

			// @throw AL::Exception
			virtual Void Terminate(uint32 exitCode) = 0;

			// @throw AL::Exception
			// @return False if maxWaitTime elapsed
			virtual Bool Join(TimeSpan maxWaitTime) = 0;
		};

		template<typename F>
		class NativeThread
			: public INativeThread
		{
			Bool isRunning = False;
			Bool isDetatched = False;

			::HANDLE    hThread = NULL;
			F           function;

		public:
			explicit NativeThread(F&& function)
				: function(
					Move(function)
				)
			{
			}

			virtual ~NativeThread()
			{
				if (hThread != NULL)
				{

					::CloseHandle(
						hThread
					);
				}
			}

			virtual Bool IsRunning() const override
			{
				return isRunning;
			}

			virtual Bool IsDetatched() const override
			{
				return isDetatched;
			}

			virtual ::HANDLE GetHandle() const override
			{
				return hThread;
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
						::CloseHandle(
							lpNativeThread->hThread
						);

						delete lpNativeThread;
					}

					return (DWORD)(0);
				};

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

				isRunning = True;
			}

			// @throw AL::Exception
			virtual Void Detatch() override
			{
				AL_ASSERT(
					IsRunning(),
					"NativeThread not running"
				);

				isDetatched = True;
			}

			// @throw AL::Exception
			virtual Void Terminate(uint32 exitCode) override
			{
				AL_ASSERT(
					IsRunning(),
					"NativeThread not running"
				);

				if (::TerminateThread(hThread, static_cast<::DWORD>(exitCode)) == 0)
				{

					throw SystemException(
						"TerminateThread"
					);
				}

				::CloseHandle(
					hThread
				);

				hThread = NULL;

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

				switch (::WaitForSingleObject(hThread, static_cast<::DWORD>(maxWaitTime.ToMilliseconds())))
				{
					case WAIT_FAILED:    throw SystemException("WaitForSingleObject");
					case WAIT_TIMEOUT:   return False;
					case WAIT_ABANDONED: throw Exception("Error calling 'WaitForSingleObject': WAIT_ABANDONED");
				}

				return !IsRunning();
			}
		};

		INativeThread* lpNativeThread;

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
					thread.Terminate(
						0
					);
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
		Void Terminate(uint32 exitCode)
		{
			if (IsRunning())
			{
				lpNativeThread->Terminate(
					exitCode
				);

				delete lpNativeThread;
				lpNativeThread = nullptr;
			}
		}

		// @throw AL::Exception
		// @return False if time elapsed and Thread is still running
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
		return static_cast<uint32>(
			::GetCurrentThreadId()
		);
	}
}
