#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class Timestamp
	{
		uint64 value;

	public:
		static Timestamp FromMicroseconds(uint64 value)
		{
			return Timestamp(
				value
			);
		}

		static Timestamp FromMilliseconds(uint64 value)
		{
			return Timestamp(
				value * 1000
			);
		}

		static Timestamp FromSeconds(uint64 value)
		{
			return Timestamp(
				value * 1000000
			);
		}

		static Timestamp FromMinutes(uint64 value)
		{
			return Timestamp(
				value * 60000000
			);
		}

		static Timestamp FromHours(uint64 value)
		{
			return Timestamp(
				value * 3600000000
			);
		}

		static Timestamp FromDays(uint64 value)
		{
			return Timestamp(
				value * 86400000000
			);
		}

		Timestamp()
			: Timestamp(
				0
			)
		{
		}

		Timestamp(uint64 microseconds)
			: value(
				microseconds
			)
		{
		}

		virtual ~Timestamp()
		{
		}

		auto ToMicroseconds() const
		{
			return value;
		}

		auto ToMilliseconds() const
		{
			return value / 1000;
		}

		auto ToSeconds() const
		{
			return value / 1000000;
		}

		auto ToMinutes() const
		{
			return value / 60000000;
		}

		auto ToHours() const
		{
			return value / 3600000000;
		}

		auto ToDays() const
		{
			return value / 86400000000;
		}

		operator TimeSpan () const
		{
			return TimeSpan(
				value
			);
		}

		Timestamp& operator += (TimeSpan time)
		{
			value += time.ToMicroseconds();

			return *this;
		}
		Timestamp& operator -= (TimeSpan time)
		{
			auto ms = time.ToMicroseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}

		TimeSpan operator - (TimeSpan time) const
		{
			auto ms = time.ToMicroseconds();

			return TimeSpan(
				(value >= ms) ? (value - ms) : ((~0 - ms) + value)
			);
		}
		TimeSpan operator - (Timestamp time) const
		{
			return TimeSpan(
				value - time.value
			);
		}

		Bool operator > (Timestamp time) const
		{
			if (value < time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator < (Timestamp time) const
		{
			if (value > time.value)
			{

				return False;
			}

			return True;
		}

		Bool operator >= (Timestamp time) const
		{
			if (value < time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator <= (Timestamp time) const
		{
			if (value > time.value)
			{

				return False;
			}

			return True;
		}

		Bool operator == (Timestamp time) const
		{
			if (value != time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator != (Timestamp time) const
		{
			if (operator==(time))
			{

				return False;
			}

			return True;
		}
	};
}
