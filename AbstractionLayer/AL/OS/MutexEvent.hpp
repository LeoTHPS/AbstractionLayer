#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"
#include "MutexCondition.hpp"

namespace AL::OS
{
	class MutexEvent
	{
		bool isSet = false;

		mutable Mutex isSetMutex;

		Mutex mutex;
		MutexCondition condition;

	public:
		MutexEvent()
		{
		}

		virtual ~MutexEvent()
		{
		}

		bool IsSet() const
		{
			MutexGuard lock(
				isSetMutex
			);

			return isSet;
		}

		// @throw AL::Exceptions::Exception
		void Set(bool set = true)
		{
			auto isSet = IsSet();

			if (!isSet && set)
			{
				MutexGuard lock(
					isSetMutex
				);

				this->isSet = true;
			}
			else if (isSet && !set)
			{
				{
					MutexGuard lock(
						isSetMutex
					);

					this->isSet = false;
				}

				condition.WakeAll();
			}
		}

		// @throw AL::Exceptions::Exception
		// @return false if duration elapsed
		bool Sleep(TimeSpan duration = TimeSpan::Infinite)
		{
			if (IsSet() && !condition.Sleep(mutex, duration))
			{

				return false;
			}

			return true;
		}
	};
}
