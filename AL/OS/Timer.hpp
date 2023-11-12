#pragma once
#include "AL/Common.hpp"

#include "SystemException.hpp"

#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
	#include <chrono>
#elif defined(AL_PLATFORM_WINDOWS)

#else
	#error Platform not supported
#endif

namespace AL::OS
{
	class Timer
	{
#if defined(AL_PLATFORM_PICO)
		uint64                                start;
#elif defined(AL_PLATFORM_LINUX)
		::std::chrono::steady_clock::duration start;
#elif defined(AL_PLATFORM_WINDOWS)
		Double                                start;
		Double                                frequency;
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

		// @throw AL::Exception
		TimeSpan GetElapsed() const
		{
#if defined(AL_PLATFORM_PICO)
			return TimeSpan::FromMicroseconds(
				time_us_64() - start
			);
#elif defined(AL_PLATFORM_LINUX)
			return TimeSpan::FromNanoseconds(
				::std::chrono::duration_cast<::std::chrono::nanoseconds>(
					::std::chrono::steady_clock::now().time_since_epoch() - start
				).count()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER integer;

			if (!::QueryPerformanceCounter(&integer))
			{

				throw SystemException(
					"QueryPerformanceCounter"
				);
			}

			return TimeSpan::FromNanoseconds(
				static_cast<uint64>(
					(integer.QuadPart - start) * frequency
				)
			);
#endif
		}

		// @throw AL::Exception
		Void Reset()
		{
#if defined(AL_PLATFORM_PICO)
			start = ::time_us_64();
#elif defined(AL_PLATFORM_LINUX)
			start = ::std::chrono::steady_clock::now().time_since_epoch();
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER integer;

			if (!::QueryPerformanceCounter(&integer))
			{

				throw SystemException(
					"QueryPerformanceCounter"
				);
			}

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
