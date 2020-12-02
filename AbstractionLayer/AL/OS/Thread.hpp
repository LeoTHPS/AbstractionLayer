#pragma once
#include "AL/Common.hpp"

#include "Timer.hpp"

#include <thread>

#if defined(AL_PLATFORM_LINUX)
	#include <unistd.h>

	#include <sys/syscall.h>
#endif

namespace AL::OS
{
	class Thread
	{
		bool isRunning = false;

		std::thread thread;

		Thread(Thread&&) = delete;
		Thread(const Thread&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void StartAndDetach(F&& function)
		{
			try
			{
				std::thread thread(
					Move(function)
				);

				thread.detach();
			}
			catch (const std::exception& exception)
			{

				throw Exceptions::Exception(
					"Error starting std::thread: %s",
					exception.what()
				);
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
					(lpFunction->*lpInstance)();
				}
			);
		}

		Thread()
		{
		}

		virtual ~Thread()
		{
		}

		bool IsRunning() const
		{
			return isRunning;
		}

		// @throw AL::Exceptions::Exception
		template<typename F>
		void Start(F&& function)
		{
			AL_ASSERT(!IsRunning(), "Thread already running");

			auto detour = [this, function = Move(function)]()
			{
				function();

				isRunning = false;
			};

			try
			{
				thread = std::thread(
					Move(detour)
				);
			}
			catch (const std::exception& exception)
			{

				throw Exceptions::Exception(
					"Error starting std::thread: %s",
					exception.what()
				);
			}

			isRunning = true;
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
					(lpFunction->*lpInstance)();
				}
			);
		}
		
		// @throw AL::Exceptions::Exception
		// @return false if time elapsed and thread is still running
		bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			Timer timer;

			while (timer.GetElapsed() < maxWaitTime)
			{
				if (!thread.joinable())
				{
					
					break;
				}

				try
				{
					thread.join();
				}
				catch (const std::exception& exception)
				{

					throw Exceptions::Exception(
						"%s",
						exception.what()
					);
				}
			}

			return !IsRunning();
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
