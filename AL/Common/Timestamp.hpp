#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class Timestamp
	{
		uint64 value;

	public:
		static constexpr uint64 Zero = 0;

		static Timestamp FromNanoseconds(uint64 value)
		{
			return Timestamp(
				value
			);
		}

		static Timestamp FromMicroseconds(uint64 value)
		{
			return Timestamp(
				value * 1000
			);
		}

		static Timestamp FromMilliseconds(uint64 value)
		{
			return Timestamp(
				value * 1000000
			);
		}

		static Timestamp FromSeconds(uint64 value)
		{
			return Timestamp(
				value * 1000000000
			);
		}

		static Timestamp FromMinutes(uint64 value)
		{
			return Timestamp(
				value * 60000000000
			);
		}

		static Timestamp FromHours(uint64 value)
		{
			return Timestamp(
				value * 3600000000000
			);
		}

		static Timestamp FromDays(uint64 value)
		{
			return Timestamp(
				value * 86400000000000
			);
		}

		Timestamp()
			: Timestamp(
				0
			)
		{
		}

		Timestamp(uint64 nanoseconds)
			: value(
				nanoseconds
			)
		{
		}

		virtual ~Timestamp()
		{
		}

		auto ToNanoseconds() const
		{
			return value;
		}

		auto ToMicroseconds() const
		{
			return value / 1000;
		}

		auto ToMilliseconds() const
		{
			return value / 1000000;
		}

		auto ToSeconds() const
		{
			return value / 1000000000;
		}

		auto ToMinutes() const
		{
			return value / 60000000000;
		}

		auto ToHours() const
		{
			return value / 3600000000000;
		}

		auto ToDays() const
		{
			return value / 86400000000000;
		}

		operator TimeSpan () const
		{
			return TimeSpan(
				value
			);
		}

		Timestamp& operator += (TimeSpan time)
		{
			value += time.ToNanoseconds();

			return *this;
		}
		Timestamp& operator -= (TimeSpan time)
		{
			auto ms = time.ToNanoseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}

		TimeSpan operator - (TimeSpan time) const
		{
			auto ms = time.ToNanoseconds();

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
