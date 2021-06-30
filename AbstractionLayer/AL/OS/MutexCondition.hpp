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
		MutexCondition(MutexCondition&&) = delete;
		MutexCondition(const MutexCondition&) = delete;

	public:
#if defined(AL_PLATFORM_LINUX)
		typedef std::condition_variable Type;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef CONDITION_VARIABLE Type;
#endif

		MutexCondition()
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
			Mutex mutex;

#if defined(AL_PLATFORM_LINUX)
			std::unique_lock<std::mutex> lock(
				mutex
			);

			Timer timer;

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

			return timer.GetElapsed() < duration;
#elif defined(AL_PLATFORM_WINDOWS)
			Timer timer;

			if (SleepConditionVariableCS(&condition, &(mutex.operator CRITICAL_SECTION &()), static_cast<DWORD>(duration.ToMilliseconds())) == 0)
			{
				auto errorCode = GetLastError();

				if (errorCode == ERROR_TIMEOUT)
				{

					return false;
				}

				throw Exceptions::SystemException(
					"SleepConditionVariableCS",
					errorCode
				);
			}

			return true;
#endif
		}

		operator Type& ()
		{
			return condition;
		}
		operator const Type& () const
		{
			return condition;
		}

	private:
		Type condition;
	};
}
