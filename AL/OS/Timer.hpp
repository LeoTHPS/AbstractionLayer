#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <time.h>
#endif

namespace AL::OS
{
	class Timer
	{
#if defined(AL_PLATFORM_PICO)
		uint64     start;
#elif defined(AL_PLATFORM_LINUX)
		::timespec start;
#elif defined(AL_PLATFORM_WINDOWS)
		Double     start;
		Double     frequency;
#endif

		Timer(const Timer&) = delete;

	public:
		Timer()
#if defined(AL_PLATFORM_WINDOWS)
			: frequency(
				[]()
				{
					::LARGE_INTEGER integer;

					::QueryPerformanceFrequency(
						&integer
					);

					return 1000000000.0 / integer.QuadPart;
				}()
			)
#endif
		{
			Reset();
		}

		Timer(Timer&& timer)
#if defined(AL_PLATFORM_PICO)
			: start(
				timer.start
			)
#elif defined(AL_PLATFORM_LINUX)
			: start(
				Move(timer.start)
			)
#elif defined(AL_PLATFORM_WINDOWS)
			: start(
				timer.start
			),
			frequency(
				timer.frequency
			)
#endif
		{
		}

		virtual ~Timer()
		{
		}

		TimeSpan GetElapsed() const
		{
#if defined(AL_PLATFORM_PICO)
			return TimeSpan::FromMicroseconds(
				time_us_64() - start
			);
#elif defined(AL_PLATFORM_LINUX)
			::timespec time;

			::timespec_get(
				&time,
				TIME_UTC
			);

			::timespec elapsed =
			{
				.tv_sec  = time.tv_sec - start.tv_sec,
				.tv_nsec = (time.tv_nsec >= start.tv_nsec) ? (time.tv_nsec - start.tv_nsec) : 0
			};

			if (time.tv_nsec >= start.tv_nsec)
			{

				elapsed.tv_nsec = time.tv_nsec - start.tv_nsec;
			}
			else
			{
				--elapsed.tv_sec;

				elapsed.tv_nsec = (1000000000 - start.tv_nsec) + time.tv_nsec;
			}

			return TimeSpan::FromNanoseconds(
				static_cast<uint64>((elapsed.tv_sec * 1000000000) + elapsed.tv_nsec)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER integer;

			::QueryPerformanceCounter(
				&integer
			);

			return TimeSpan::FromNanoseconds(
				static_cast<uint64>(
					(integer.QuadPart - start) * frequency
				)
			);
#endif
		}

		Void Reset()
		{
#if defined(AL_PLATFORM_PICO)
			start = ::time_us_64();
#elif defined(AL_PLATFORM_LINUX)
			::timespec_get(
				&start,
				TIME_UTC
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER integer;

			::QueryPerformanceCounter(
				&integer
			);

			start = static_cast<Double>(
				integer.QuadPart
			);
#endif
		}

		Timer& operator = (Timer&& timer)
		{
#if defined(AL_PLATFORM_PICO)
			start = timer.start;
#elif defined(AL_PLATFORM_LINUX)
			start = Move(
				timer.start
			);
#elif defined(AL_PLATFORM_WINDOWS)
			start     = timer.start;
			frequency = timer.frequency;
#endif

			return *this;
		}
	};
}
