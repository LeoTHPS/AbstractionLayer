#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <sysinfoapi.h>
#endif

namespace AL::OS
{
	class System
	{
		System() = delete;

	public:
		static int8 GetTimezone()
		{
#if defined(AL_PLATFORM_LINUX)
			throw AL::Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			TIME_ZONE_INFORMATION zoneInfo;

			switch (GetTimeZoneInformation(&zoneInfo))
			{
				case TIME_ZONE_ID_STANDARD:
					return -static_cast<int8>(zoneInfo.Bias / 60);

				case TIME_ZONE_ID_DAYLIGHT:
					return -static_cast<int8>(zoneInfo.Bias / 60) + 1;
			}
#endif

			return 0;
		}

		static DateTime GetDateTime()
		{
			DateTime time;

#if defined(AL_PLATFORM_LINUX)
			throw AL::Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			time.Year = static_cast<uint32>(
				systemTime.wYear
			);
			time.Month = static_cast<DateTime::Months>(
				systemTime.wMonth
			);
			time.Day = static_cast<uint32>(
				systemTime.wDay
			);
			time.DayOfWeek = static_cast<DateTime::DaysOfWeek>(
				systemTime.wDayOfWeek
			);
			time.Hour = static_cast<uint32>(
				systemTime.wHour
			);
			time.Minutes = static_cast<uint32>(
				systemTime.wMinute
			);
			time.Seconds = static_cast<uint32>(
				systemTime.wSecond
			);
			time.Milliseconds = static_cast<uint32>(
				systemTime.wMilliseconds
			);
#endif

			return time;
		}

		static Timestamp GetTimestamp()
		{
			Timestamp timestamp;
			Integer<uint64> integer;

#if defined(AL_PLATFORM_LINUX)
			throw AL::Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			FILETIME time;
			GetSystemTimeAsFileTime(&time);

			integer.Low.Value = time.dwLowDateTime;
			integer.High.Value = time.dwHighDateTime;

			timestamp = Timestamp::FromSeconds(
				(integer.Value - 116444736000000000) / 10000000
			);
#endif

			return timestamp;
		}

		static auto GetProcessorCount()
		{
#if defined(AL_PLATFORM_LINUX)
			throw AL::Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);

			return static_cast<size_t>(
				systemInfo.dwNumberOfProcessors
			);
#endif
		}
	};
}
