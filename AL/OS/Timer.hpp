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
		uint64 start;
#elif defined(AL_PLATFORM_LINUX)
		uint64 start;
#elif defined(AL_PLATFORM_WINDOWS)
		Double start;
		Double frequency;
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
				timer.start
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

			return TimeSpan::FromNanoseconds(
				((time.tv_sec * 1000000000) + time.tv_nsec) - start
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
			::timespec time;

			::timespec_get(
				&time,
				TIME_UTC
			);

			start = (time.tv_sec * 1000000000) + time.tv_nsec;
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
			start     = timer.start;
#elif defined(AL_PLATFORM_LINUX)
			start     = timer.start;
#elif defined(AL_PLATFORM_WINDOWS)
			start     = timer.start;
			frequency = timer.frequency;
#endif

			return *this;
		}
	};
}
