#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"
#include "Timer.hpp"
#include "SystemException.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#include <condition_variable>
#endif

namespace AL::OS
{
	class ConditionVariable
	{
#if defined(AL_PLATFORM_WINDOWS)
		::CONDITION_VARIABLE      condition;
#else
		::std::condition_variable condition;
#endif

		ConditionVariable(ConditionVariable&&) = delete;
		ConditionVariable(const ConditionVariable&) = delete;

	public:
#if defined(AL_PLATFORM_WINDOWS)
		typedef ::CONDITION_VARIABLE      Type;
#else
		typedef ::std::condition_variable Type;
#endif

		ConditionVariable()
		{
#if defined(AL_PLATFORM_WINDOWS)
			::InitializeConditionVariable(
				&condition
			);
#endif
		}

		virtual ~ConditionVariable()
		{
		}

		Void WakeAll()
		{
#if defined(AL_PLATFORM_WINDOWS)
			::WakeAllConditionVariable(
				&condition
			);
#else
			condition.notify_all();
#endif
		}

		Void WakeOne()
		{
#if defined(AL_PLATFORM_WINDOWS)
			::WakeConditionVariable(
				&condition
			);
#else
			condition.notify_one();
#endif
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Sleep(TimeSpan timeout = TimeSpan::Infinite)
		{
			Mutex mutex;

#if defined(AL_PLATFORM_WINDOWS)
			Timer timer;

			if (::SleepConditionVariableCS(&condition, &(mutex.operator AL::OS::Mutex::Type&()), static_cast<::DWORD>(timeout.ToMilliseconds())))
			{
				auto errorCode = GetLastError();

				if (errorCode == ERROR_TIMEOUT)
				{

					return False;
				}

				throw SystemException(
					"SleepConditionVariableCS",
					errorCode
				);
			}
#else
			std::unique_lock<std::mutex> lock(
				mutex
			);

			Timer timer;

			try
			{
				condition.wait_for(
					lock,
					::std::chrono::milliseconds(
						timeout.ToMilliseconds()
					)
				);
			}
			catch (const ::std::exception& exception)
			{

				throw Exception(
					exception.what()
				);
			}

			if (timer.GetElapsed() >= timeout)
			{

				return False;
			}
#endif

			return True;
		}

		operator Type& ()
		{
			return condition;
		}
		operator const Type& () const
		{
			return condition;
		}
	};
}
