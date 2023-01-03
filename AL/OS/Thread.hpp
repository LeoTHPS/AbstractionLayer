#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Thread.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Thread.hpp"
#else
	#error Platform not supported
#endif

namespace AL::OS
{
#if defined(AL_PLATFORM_LINUX)
	typedef Linux::ThreadId ThreadId;
#elif defined(AL_PLATFORM_WINDOWS)
	typedef Windows::ThreadId ThreadId;
#endif

	class Thread
	{
#if defined(AL_PLATFORM_LINUX)
		Linux::Thread   thread;
#elif defined(AL_PLATFORM_WINDOWS)
		Windows::Thread thread;
#endif

		Thread(Thread&&) = delete;
		Thread(const Thread&) = delete;

	public:
		// @throw AL::Exception
		template<typename F>
		static Void StartAndDetach(F&& function)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Thread::StartAndDetach(
				Move(function)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Thread::StartAndDetach(
				Move(function)
			);
#endif
		}
		// @throw AL::Exception
		static Void StartAndDetach(Void(*lpFunction)())
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Thread::StartAndDetach(
				lpFunction
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Thread::StartAndDetach(
				lpFunction
			);
#endif
		}
		// @throw AL::Exception
		template<typename C>
		static Void StartAndDetach(Void(C::*lpFunction)(), C* lpInstance)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Thread::StartAndDetach(
				lpFunction,
				lpInstance
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Thread::StartAndDetach(
				lpFunction,
				lpInstance
			);
#endif
		}
		// @throw AL::Exception
		template<typename C>
		static Void StartAndDetach(Void(C::*lpFunction)() const, const C* lpInstance)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Thread::StartAndDetach(
				lpFunction,
				lpInstance
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Thread::StartAndDetach(
				lpFunction,
				lpInstance
			);
#endif
		}

		Thread()
		{
		}

		virtual ~Thread()
		{
		}

		Bool IsRunning() const
		{
#if defined(AL_PLATFORM_LINUX)
			return thread.IsRunning();
#elif defined(AL_PLATFORM_WINDOWS)
			return thread.IsRunning();
#endif
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)
			return thread.GetHandle();
#elif defined(AL_PLATFORM_WINDOWS)
			return thread.GetHandle();
#endif
		}

		// @throw AL::Exception
		template<typename F>
		Void Start(F&& function)
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			thread.Start(
				Move(function)
			);
		}
		// @throw AL::Exception
		Void Start(Void(*lpFunction)())
		{
			AL_ASSERT(
				!IsRunning(),
				"Thread already running"
			);

			thread.Start(
				lpFunction
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

			thread.Start(
				lpFunction,
				lpInstance
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

			thread.Start(
				lpFunction,
				lpInstance
			);
		}

		// @throw AL::Exception
		Void Detatch()
		{
			AL_ASSERT(
				IsRunning(),
				"Thread not running"
			);

			thread.Detatch();
		}

		// @throw AL::Exception
		Void Terminate()
		{
#if defined(AL_PLATFORM_LINUX)
			thread.Terminate();
#elif defined(AL_PLATFORM_WINDOWS)
			thread.Terminate(
				0
			);
#endif
		}

		// @throw AL::Exception
		// @return AL::False if time elapsed and thread is still running
		Bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			if (!thread.Join(maxWaitTime))
			{

				return False;
			}

			return True;
		}
	};

	inline ThreadId GetCurrentThreadId()
	{
#if defined(AL_PLATFORM_LINUX)
		return Linux::GetCurrentThreadId();
#elif defined(AL_PLATFORM_WINDOWS)
		return Windows::GetCurrentThreadId();
#endif
	}
}
