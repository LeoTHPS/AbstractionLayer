#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class TimeSpan
	{
		uint64 value;

	public:
		static constexpr uint64 Zero     = 0;
		static constexpr uint64 Infinite = 0x000F423FFFF0BDC0;

		static TimeSpan FromNanoseconds(uint64 value)
		{
			return TimeSpan(
				value
			);
		}

		static TimeSpan FromMicroseconds(uint64 value)
		{
			return TimeSpan(
				value * 1000
			);
		}

		static TimeSpan FromMilliseconds(uint64 value)
		{
			return TimeSpan(
				value * 1000000
			);
		}

		static TimeSpan FromSeconds(uint64 value)
		{
			return TimeSpan(
				value * 1000000000
			);
		}

		static TimeSpan FromMinutes(uint64 value)
		{
			return TimeSpan(
				value * 60000000000
			);
		}

		static TimeSpan FromHours(uint64 value)
		{
			return TimeSpan(
				value * 3600000000000
			);
		}

		static TimeSpan FromDays(uint64 value)
		{
			return TimeSpan(
				value * 86400000000000
			);
		}

		TimeSpan()
			: TimeSpan(
				0
			)
		{
		}

		TimeSpan(uint64 nanoseconds)
			: value(
				nanoseconds
			)
		{
		}

		virtual ~TimeSpan()
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

		Bool operator > (TimeSpan time) const
		{
			if (value <= time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator < (TimeSpan time) const
		{
			if (value >= time.value)
			{

				return False;
			}

			return True;
		}

		Bool operator >= (TimeSpan time) const
		{
			if (value < time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator <= (TimeSpan time) const
		{
			if (value > time.value)
			{

				return False;
			}

			return True;
		}

		TimeSpan& operator += (TimeSpan time)
		{
			value += time.ToNanoseconds();

			return *this;
		}
		TimeSpan& operator -= (TimeSpan time)
		{
			auto ms = time.ToNanoseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}
		TimeSpan& operator /= (TimeSpan time)
		{
			value /= time.ToNanoseconds();

			return *this;
		}
		TimeSpan& operator *= (TimeSpan time)
		{
			value *= time.ToNanoseconds();

			return *this;
		}

		TimeSpan operator + (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time += time;

			return _time;
		}
		TimeSpan operator - (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time -= time;

			return _time;
		}
		TimeSpan operator / (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time /= time;

			return _time;
		}
		TimeSpan operator * (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time *= time;

			return _time;
		}

		Bool operator == (TimeSpan time) const
		{
			if (value != time.value)
			{

				return False;
			}

			return True;
		}
		Bool operator != (TimeSpan time) const
		{
			if (operator==(time))
			{

				return False;
			}

			return True;
		}
	};
}
