#pragma once
#include "AL/Common.hpp"

namespace AL
{
	struct DateTime
	{
		enum class Months : uint8
		{
			January = 1,
			February,
			March,
			April,
			May,
			June,
			July,
			August,
			September,
			October,
			November,
			December
		};

		enum class DaysOfWeek : uint8
		{
			Sunday,
			Monday,
			Tuesday,
			Wednesday,
			Thursday,
			Friday,
			Saturday
		};

		uint32     Year;
		Months     Month;
		uint32     Day;
		DaysOfWeek DayOfWeek;
		uint32     Hour;
		uint32     Minutes;
		uint32     Seconds;
		uint32     Milliseconds;

		DateTime()
			: Year(
				0
			),
			Month(
				Months::January
			),
			Day(
				0
			),
			DayOfWeek(
				DaysOfWeek::Sunday
			),
			Hour(
				0
			),
			Minutes(
				0
			),
			Seconds(
				0
			),
			Milliseconds(
				0
			)
		{
		}
	};
}
