#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/UnorderedSet.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <time.h>

	#include <sys/sysinfo.h>
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
			auto time = ::time(
				NULL
			);

			tm localTime = { 0 };

			localtime_r(
				&time,
				&localTime
			);

			return static_cast<int8>(
				localTime.tm_gmtoff / 3600
			);
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
			DateTime dateTime;

#if defined(AL_PLATFORM_LINUX)
			auto time = ::time(
				NULL
			);

			tm localTime = { 0 };

			localtime_r(
				&time,
				&localTime
			);

			dateTime.Year = static_cast<uint32>(
				localTime.tm_year
			);
			dateTime.Month = static_cast<DateTime::Months>(
				localTime.tm_mon
			);
			dateTime.Day = static_cast<uint32>(
				localTime.tm_mday
			);
			dateTime.DayOfWeek = static_cast<DateTime::DaysOfWeek>(
				localTime.tm_wday
			);
			dateTime.Hour = static_cast<uint32>(
				localTime.tm_hour
			);
			dateTime.Minutes = static_cast<uint32>(
				localTime.tm_min
			);
			dateTime.Seconds = static_cast<uint32>(
				localTime.tm_sec
			);
			dateTime.Milliseconds = static_cast<uint32>(
				0
			);
#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);

			dateTime.Year = static_cast<uint32>(
				systemTime.wYear
			);
			dateTime.Month = static_cast<DateTime::Months>(
				systemTime.wMonth
			);
			dateTime.Day = static_cast<uint32>(
				systemTime.wDay
			);
			dateTime.DayOfWeek = static_cast<DateTime::DaysOfWeek>(
				systemTime.wDayOfWeek
			);
			dateTime.Hour = static_cast<uint32>(
				systemTime.wHour
			);
			dateTime.Minutes = static_cast<uint32>(
				systemTime.wMinute
			);
			dateTime.Seconds = static_cast<uint32>(
				systemTime.wSecond
			);
			dateTime.Milliseconds = static_cast<uint32>(
				systemTime.wMilliseconds
			);
#endif

			return dateTime;
		}

		static Timestamp GetTimestamp()
		{
#if defined(AL_PLATFORM_LINUX)
			return Timestamp::FromSeconds(
				static_cast<uint64>(::time(NULL))
			);
#elif defined(AL_PLATFORM_WINDOWS)
			FILETIME time;
			GetSystemTimeAsFileTime(&time);

			Integer<uint64> integer;
			integer.Low.Value = time.dwLowDateTime;
			integer.High.Value = time.dwHighDateTime;

			return Timestamp::FromSeconds(
				(integer.Value - 116444736000000000) / 10000000
			);
#endif
		}

		static auto GetPageSize()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);

			return static_cast<size_t>(
				systemInfo.dwPageSize
			);
#endif
		}

		static auto GetMinimumAddress()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);

			return reinterpret_cast<size_t>(
				systemInfo.lpMinimumApplicationAddress
			);
#endif
		}

		static auto GetMaximumAddress()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);

			return reinterpret_cast<size_t>(
				systemInfo.lpMaximumApplicationAddress
			);
#endif
		}

		static auto GetProcessorCount()
		{
#if defined(AL_PLATFORM_LINUX)
			return static_cast<size_t>(
				get_nprocs()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);

			return static_cast<size_t>(
				systemInfo.dwNumberOfProcessors
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		static auto GetProcessorCacheCount()
		{
			size_t count = 0;

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			Collections::UnorderedSet<size_t> levels;

			EnumerateLogicalProcessorInformation(
				[&count, &levels](const SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (!levels.Contains(_info.Cache.Level))
						{
							++count;

							levels.Add(
								_info.Cache.Level
							);
						}
					}

					return true;
				}
			);
#endif

			return count;
		}

		// @throw AL::Exceptions::Exception
		static auto GetProcessorCacheSize(size_t cacheLevel)
		{
			size_t size = 0;

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

			EnumerateLogicalProcessorInformation(
				[&size, cacheLevel](const SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (_info.Cache.Level == static_cast<BYTE>(cacheLevel))
						{
							size = _info.Cache.Size;

							return false;
						}
					}

					return true;
				}
			);
#endif

			return size;
		}

		// @throw AL::Exceptions::Exception
		static auto GetProcessorCacheLineSize(size_t cacheLevel)
		{
			size_t lineSize = 0;

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

			EnumerateLogicalProcessorInformation(
				[&lineSize, cacheLevel](const SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (_info.Cache.Level == static_cast<BYTE>(cacheLevel))
						{
							lineSize = _info.Cache.LineSize;
							
							return false;
						}
					}

					return true;
				}
			);
#endif

			return lineSize;
		}

	private:
#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		template<typename F = bool(*)(const SYSTEM_LOGICAL_PROCESSOR_INFORMATION&)>
		static void EnumerateLogicalProcessorInformation(F&& callback)
		{
			DWORD bufferSize = 0;

			if (!GetLogicalProcessorInformation(nullptr, &bufferSize))
			{
				switch (auto lastErrorCode = GetLastError())
				{
					case ERROR_INSUFFICIENT_BUFFER:
						break;

					default:
						throw Exceptions::SystemException(
							"GetLogicalProcessorInformation",
							lastErrorCode
						);
				}
			}

			Collections::Array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
				bufferSize
			);

			if (!GetLogicalProcessorInformation(&buffer[0], &bufferSize))
			{

				throw Exceptions::SystemException(
					"GetLogicalProcessorInformation"
				);
			}
			
			for (size_t i = 0; i < bufferSize; ++i)
			{
				if (!callback(buffer[i]))
				{

					break;
				}
			}
		}
#endif
	};
}
