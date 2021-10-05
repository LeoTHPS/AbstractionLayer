#pragma once
#include "AL/Common.hpp"

namespace AL
{
	struct Time
	{
		uint32 Hours;
		uint32 Minutes;
		uint32 Seconds;
		uint32 Milliseconds;

		Time()
			: Hours(
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

		Time(const DateTime& dateTime)
			: Hours(
				dateTime.Hour
			),
			Minutes(
				dateTime.Minutes
			),
			Seconds(
				dateTime.Seconds
			),
			Milliseconds(
				dateTime.Milliseconds
			)
		{
		}
	};
}
