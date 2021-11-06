#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class TimeSpan
	{
		uint64 value;

	public:
		static constexpr uint64 Zero     = 0;
		static constexpr uint64 Infinite = 0x3E7FFFFFC18;

		static TimeSpan FromMicroseconds(uint64 value)
		{
			return TimeSpan(
				value
			);
		}

		static TimeSpan FromMilliseconds(uint64 value)
		{
			return TimeSpan(
				value * 1000
			);
		}

		static TimeSpan FromSeconds(uint64 value)
		{
			return TimeSpan(
				value * 1000000
			);
		}

		static TimeSpan FromMinutes(uint64 value)
		{
			return TimeSpan(
				value * 60000000
			);
		}

		static TimeSpan FromHours(uint64 value)
		{
			return TimeSpan(
				value * 3600000000
			);
		}

		static TimeSpan FromDays(uint64 value)
		{
			return TimeSpan(
				value * 86400000000
			);
		}

		TimeSpan()
			: TimeSpan(
				0
			)
		{
		}

		TimeSpan(uint64 microseconds)
			: value(
				microseconds
			)
		{
		}

		virtual ~TimeSpan()
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
			value += time.ToMicroseconds();

			return *this;
		}
		TimeSpan& operator -= (TimeSpan time)
		{
			auto ms = time.ToMicroseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}
		TimeSpan& operator /= (TimeSpan time)
		{
			value /= time.ToMicroseconds();

			return *this;
		}
		TimeSpan& operator *= (TimeSpan time)
		{
			value *= time.ToMicroseconds();

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
