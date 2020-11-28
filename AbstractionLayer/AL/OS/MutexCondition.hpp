#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <condition_variable>
#endif

namespace AL::OS
{
	class MutexCondition
	{
		Mutex* const lpMutex;

#if defined(AL_PLATFORM_LINUX)
		std::condition_variable condition;
#elif defined(AL_PLATFORM_WINDOWS)
		CONDITION_VARIABLE condition;
#endif

	public:
		explicit MutexCondition(Mutex& mutex)
			: lpMutex(
				&mutex
			)
		{
#if defined(AL_PLATFORM_WINDOWS)
			InitializeConditionVariable(
				&condition
			);
#endif
		}

		virtual ~MutexCondition()
		{
		}

		auto& GetMutex()
		{
			return *lpMutex;
		}
		auto& GetMutex() const
		{
			return *lpMutex;
		}

		void WakeAll()
		{
#if defined(AL_PLATFORM_LINUX)
			condition.notify_all();
#elif defined(AL_PLATFORM_WINDOWS)
			WakeAllConditionVariable(
				&condition
			);
#endif
		}

		void WakeOne()
		{
#if defined(AL_PLATFORM_LINUX)
			condition.notify_one();
#elif defined(AL_PLATFORM_WINDOWS)
			WakeConditionVariable(
				&condition
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		// @return false if duration elapsed
		bool Sleep(TimeSpan duration = TimeSpan::Infinite)
		{
			Timer timer;

#if defined(AL_PLATFORM_LINUX)
			std::unique_lock<std::mutex> lock(
				GetMutex()
			);

			try
			{
				condition.wait_for(
					lock,
					std::chrono::milliseconds(
						duration.ToMilliseconds()
					)
				);
			}
			catch (const std::exception& exception)
			{

				throw Exceptions::Exception(
					exception.what()
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			CRITICAL_SECTION& criticalSection = GetMutex();

			if (SleepConditionVariableCS(&condition, &criticalSection, static_cast<DWORD>(duration.ToMilliseconds())) != 0)
			{

				throw Exceptions::SystemException(
					"SleepConditionVariableCS"
				);
			}
#endif

			return timer.GetElapsed() < duration;
		}

#if defined(AL_PLATFORM_LINUX)
		operator std::condition_variable& ()
		{
			return condition;
		}
		operator const std::condition_variable& () const
		{
			return condition;
		}
#elif defined(AL_PLATFORM_WINDOWS)
		operator CONDITION_VARIABLE& ()
		{
			return condition;
		}
		operator const CONDITION_VARIABLE& () const
		{
			return condition;
		}
#endif
	};
}
