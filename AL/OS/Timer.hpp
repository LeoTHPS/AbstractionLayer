#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <time.h>
#endif

namespace AL::OS
{
	class Timer
	{
#if defined(AL_PLATFORM_LINUX)
		::timespec              start;
		mutable ::timespec      time;
#elif defined(AL_PLATFORM_WINDOWS)
		Double                  start;
		mutable ::LARGE_INTEGER integer;
		Double                  frequency;
#endif

		Timer(const Timer&) = delete;

	public:
		Timer()
#if defined(AL_PLATFORM_WINDOWS)
			: frequency(
				[this]()
				{
					::QueryPerformanceFrequency(
						&integer
					);

					auto quad = static_cast<Double>(
						integer.QuadPart
					);

					return quad / 1000000;
				}()
			)
#endif
		{
			Reset();
		}

		Timer(Timer&& timer)
#if defined(AL_PLATFORM_LINUX)
			: start(
				Move(timer.start)
			),
			time(
				Move(timer.time)
			)
#elif defined(AL_PLATFORM_WINDOWS)
			: start(
				timer.start
			),
			integer(
				Move(timer.integer)
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
#if defined(AL_PLATFORM_LINUX)
			::timespec_get(
				&time,
				TIME_UTC
			);

			::timespec elapsed;
			elapsed.tv_sec = time.tv_sec - start.tv_sec;
			elapsed.tv_nsec = (time.tv_nsec >= start.tv_nsec) ? (time.tv_nsec - start.tv_nsec) : 0;

			if (time.tv_nsec >= start.tv_nsec)
			{

				elapsed.tv_nsec = time.tv_nsec - start.tv_nsec;
			}
			else
			{
				--elapsed.tv_sec;

				elapsed.tv_nsec = (1000000000 - start.tv_nsec) + time.tv_nsec;
			}

			return TimeSpan::FromMicroseconds(
				static_cast<uint64>((elapsed.tv_sec * 1000000) + (elapsed.tv_nsec / 1000))
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::QueryPerformanceCounter(
				&integer
			);

			return TimeSpan::FromMicroseconds(
				static_cast<uint64>(
					(integer.QuadPart - start) / frequency
				)
			);
#endif
		}

		Void Reset()
		{
#if defined(AL_PLATFORM_LINUX)
			::timespec_get(
				&start,
				TIME_UTC
			);
#elif defined(AL_PLATFORM_WINDOWS)
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
#if defined(AL_PLATFORM_LINUX)
			start = Move(
				timer.start
			);

			time = Move(
				timer.time
			);
#elif defined(AL_PLATFORM_WINDOWS)
			start = timer.start;

			integer = Move(
				timer.integer
			);

			frequency = timer.frequency;
#endif

			return *this;
		}
	};
}
