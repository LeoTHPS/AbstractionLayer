#pragma once
#include "AL/Common.hpp"

#include "SystemException.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/UnorderedSet.hpp"

#if defined(AL_PLATFORM_PICO)
	#include <time.h>
#elif defined(AL_PLATFORM_LINUX)
	#include <pwd.h>
	#include <time.h>
	#include <stdlib.h>
	#include <unistd.h>

	#include <sys/types.h>
	#include <sys/sysinfo.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <lmcons.h>
	#include <sysinfoapi.h>

	#undef GetEnvironmentVariable
#else
	#error Platform not supported
#endif

namespace AL::OS
{
	class System
	{
		System() = delete;

	public:
		static int8 GetTimezone()
		{
#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			auto time = ::time(
				NULL
			);

			::tm localTime = { 0 };

			::localtime_r(
				&time,
				&localTime
			);

			return static_cast<int8>(
				localTime.tm_gmtoff / 3600
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::TIME_ZONE_INFORMATION zoneInfo;

			switch (::GetTimeZoneInformation(&zoneInfo))
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

#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			auto time = ::time(
				NULL
			);

			::tm localTime = { 0 };

			::localtime_r(
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
			::SYSTEMTIME systemTime;
			::GetSystemTime(&systemTime);

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
#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			return Timestamp::FromSeconds(
				static_cast<uint64>(::time(NULL))
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::FILETIME time;
			::GetSystemTimeAsFileTime(&time);

			Integer<uint64> integer;
			integer.Low.Value  = time.dwLowDateTime;
			integer.High.Value = time.dwHighDateTime;

			return Timestamp::FromNanoseconds(
				(integer.Value - 116444736000000000) * 100
			);
#endif
		}

		static auto GetPageSize()
		{
#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			return static_cast<size_t>(
				::sysconf(_SC_PAGESIZE)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::SYSTEM_INFO systemInfo;
			::GetSystemInfo(&systemInfo);

			return static_cast<size_t>(
				systemInfo.dwPageSize
			);
#endif
		}

		// @throw AL::Exception
		static auto GetCurrentUser()
		{
#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
			::passwd* lpPasswd;

			if ((lpPasswd = ::getpwuid(::geteuid())) == nullptr)
			{

				throw SystemException(
					"getpwuid"
				);
			}

			return String(
				lpPasswd->pw_name
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::DWORD bufferSize = 0;

			if (::GetUserNameA(nullptr, &bufferSize) == 0)
			{
				auto lastError = GetLastError();

				if (lastError != ERROR_INSUFFICIENT_BUFFER)
				{

					throw SystemException(
						"GetUserNameA",
						lastError
					);
				}
			}

			Collections::Array<typename String::Char> buffer(
				bufferSize
			);

			if (::GetUserNameA(&buffer[0], &bufferSize) == 0)
			{

				throw SystemException(
					"GetUserNameA"
				);
			}

			return String(
				&buffer[0],
				static_cast<size_t>(bufferSize - 1)
			);
#endif
		}

		// @throw AL::Exception
		static auto GetCurrentUserID()
		{
#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
			return static_cast<uint32>(
				::geteuid()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif
		}

		static auto GetMinimumAddress()
		{
#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			::SYSTEM_INFO systemInfo;
			::GetSystemInfo(&systemInfo);

			return reinterpret_cast<size_t>(
				systemInfo.lpMinimumApplicationAddress
			);
#endif
		}

		static auto GetMaximumAddress()
		{
#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			::SYSTEM_INFO systemInfo;
			::GetSystemInfo(&systemInfo);

			return reinterpret_cast<size_t>(
				systemInfo.lpMaximumApplicationAddress
			);
#endif
		}

		// @throw AL::Exception
		static Bool GetEnvironmentVariable(String& value, const String& name)
		{
#if defined(AL_PLATFORM_LINUX)
			if (auto buffer = ::getenv(name.GetCString()))
			{
				value.Assign(
					buffer
				);

				return True;
			}

			return False;
#elif defined(AL_PLATFORM_WINDOWS)
			if (auto bufferLength = ::GetEnvironmentVariableA(name.GetCString(), nullptr, 0))
			{
				value.SetCapacity(
					bufferLength
				);

				if (::GetEnvironmentVariableA(name.GetCString(), &value[0], bufferLength) == 0)
				{

					throw SystemException(
						"GetEnvironmentVariableA"
					);
				}

				return True;
			}
#endif

			return False;
		}

		static size_t GetProcessorCount()
		{
#if defined(AL_PLATFORM_PICO)
			return 2;
#elif defined(AL_PLATFORM_LINUX)
			return static_cast<size_t>(
				::get_nprocs()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::SYSTEM_INFO systemInfo;
			::GetSystemInfo(&systemInfo);

			return static_cast<size_t>(
				systemInfo.dwNumberOfProcessors
			);
#endif
		}

		// @throw AL::Exception
		static size_t GetProcessorCacheCount()
		{
			size_t count = 0;

#if defined(AL_PLATFORM_PICO)
			count = 1;
#elif defined(AL_PLATFORM_LINUX)
			if (::sysconf(_SC_LEVEL1_DCACHE_SIZE) != -1)
			{

				++count;
			}

			if (::sysconf(_SC_LEVEL2_CACHE_SIZE) != -1)
			{

				++count;
			}

			if (::sysconf(_SC_LEVEL3_CACHE_SIZE) != -1)
			{

				++count;
			}

			if (::sysconf(_SC_LEVEL4_CACHE_SIZE) != -1)
			{

				++count;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			Collections::UnorderedSet<size_t> levels;

			EnumerateLogicalProcessorInformation(
				[&count, &levels](const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == ::LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (!levels.Contains(_info.Cache.Level))
						{
							++count;

							levels.Add(
								_info.Cache.Level
							);
						}
					}

					return True;
				}
			);
#endif

			return count;
		}

		// @throw AL::Exception
		static size_t GetProcessorCacheSize(size_t cache)
		{
			size_t size = 0;

#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
			switch (cache)
			{
				case 0:
					size = static_cast<size_t>(::sysconf(_SC_LEVEL1_DCACHE_SIZE));
					break;

				case 1:
					size = static_cast<size_t>(::sysconf(_SC_LEVEL2_CACHE_SIZE));
					break;

				case 2:
					size = static_cast<size_t>(::sysconf(_SC_LEVEL3_CACHE_SIZE));
					break;

				case 3:
					size = static_cast<size_t>(::sysconf(_SC_LEVEL4_CACHE_SIZE));
					break;
			}

			if (size == static_cast<size_t>(-1))
			{

				throw SystemException(
					"sysconf"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			EnumerateLogicalProcessorInformation(
				[&size, cache = cache + 1](const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == ::LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (_info.Cache.Level == static_cast<::BYTE>(cache))
						{
							size = _info.Cache.Size;

							return False;
						}
					}

					return True;
				}
			);
#endif

			return size;
		}

		// @throw AL::Exception
		static size_t GetProcessorCacheLineSize(size_t cache)
		{
			size_t lineSize = 0;

#if defined(AL_PLATFORM_PICO)
			switch (cache)
			{
				case 0:
					// the RP2040 line size is 64 bits
					// but only the first 32 are guaranteed
					lineSize = 4;
					break;

				default:
					throw OperationNotSupportedException();
			}
#elif defined(AL_PLATFORM_LINUX)
			switch (cache)
			{
				case 0:
					lineSize = static_cast<size_t>(::sysconf(_SC_LEVEL1_DCACHE_LINESIZE));
					break;

				case 1:
					lineSize = static_cast<size_t>(::sysconf(_SC_LEVEL2_CACHE_LINESIZE));
					break;

				case 2:
					lineSize = static_cast<size_t>(::sysconf(_SC_LEVEL3_CACHE_LINESIZE));
					break;

				case 3:
					lineSize = static_cast<size_t>(::sysconf(_SC_LEVEL4_CACHE_LINESIZE));
					break;

				default:
					throw OperationNotSupportedException();
			}

			if (lineSize == static_cast<size_t>(-1))
			{

				throw SystemException(
					"sysconf"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			Bool isFound = False;

			EnumerateLogicalProcessorInformation(
				[&isFound, &lineSize, cache = cache + 1](const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION& _info)
				{
					if (_info.Relationship == ::LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache)
					{
						if (_info.Cache.Level == static_cast<::BYTE>(cache))
						{
							isFound  = True;
							lineSize = _info.Cache.LineSize;

							return False;
						}
					}

					return True;
				}
			);

			if (!isFound)
			{

				throw OperationNotSupportedException();
			}
#endif

			return lineSize;
		}

	private:
#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exception
		template<typename F = Bool(*)(const ::SYSTEM_LOGICAL_PROCESSOR_INFORMATION&)>
		static Void EnumerateLogicalProcessorInformation(F&& callback)
		{
			::DWORD bufferSize = 0;

			if (!::GetLogicalProcessorInformation(nullptr, &bufferSize))
			{
				switch (auto lastErrorCode = GetLastError())
				{
					case ERROR_INSUFFICIENT_BUFFER:
						break;

					default:
						throw SystemException(
							"GetLogicalProcessorInformation",
							lastErrorCode
						);
				}
			}

			Collections::Array<::SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
				bufferSize / sizeof(::SYSTEM_LOGICAL_PROCESSOR_INFORMATION)
			);

			if (!::GetLogicalProcessorInformation(&buffer[0], &bufferSize))
			{

				throw SystemException(
					"GetLogicalProcessorInformation"
				);
			}

			for (auto& logicalProcessorInfo : buffer)
			{
				if (!callback(logicalProcessorInfo))
				{

					break;
				}
			}
		}
#endif
	};
}
