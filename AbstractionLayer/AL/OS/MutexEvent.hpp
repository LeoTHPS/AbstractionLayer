#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"
#include "MutexCondition.hpp"

namespace AL::OS
{
	class MutexEvent
	{
		bool isSet = false;

		mutable Mutex mutex;

		Mutex mutex2;
		MutexCondition condition;

	public:
		MutexEvent()
			: condition(
				mutex2
			)
		{
		}

		virtual ~MutexEvent()
		{
		}

		bool IsSet() const
		{
			MutexGuard lock(
				mutex
			);

			return isSet;
		}

		// @throw AL::Exceptions::Exception
		void Set(bool set = true)
		{
			MutexGuard lock(
				mutex
			);

			if (!IsSet() && set)
			{
				isSet = true;
			}
			else if (IsSet() && !set)
			{
				isSet = false;

				condition.WakeAll();
			}
		}

		// @throw AL::Exceptions::Exception
		// @return false if duration elapsed
		bool Sleep(TimeSpan duration = TimeSpan::Infinite)
		{
			MutexGuard lock(
				mutex
			);

			if (IsSet() && !condition.Sleep(duration))
			{

				return false;
			}

			return true;
		}
	};
}
