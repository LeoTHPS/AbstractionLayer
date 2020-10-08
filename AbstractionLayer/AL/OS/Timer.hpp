#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <chrono>
#endif

namespace AL::OS
{
	class Timer
	{
#if defined(AL_PLATFORM_LINUX)
		typedef std::chrono::high_resolution_clock Clock;

		typename Clock::time_point start;
#elif defined(AL_PLATFORM_WINDOWS)
		double start;
		double frequency;
		mutable LARGE_INTEGER integer;
#endif

	public:
		Timer()
#if defined(AL_PLATFORM_LINUX)
			: start(
				Clock::now()
			)
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
			: frequency(
				[this]()
				{
					QueryPerformanceFrequency(
						&integer
					);

					return static_cast<double>(
						integer.QuadPart
					) / 1000000;
				}()
			)
		{
			Reset();
		}
#endif

		virtual ~Timer()
		{
		}

		auto GetElapsed() const
		{
#if defined(AL_PLATFORM_LINUX)
			auto now = Clock::now();
			
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
				now - start
			).count();

			return TimeSpan(
				static_cast<uint64>(
					elapsed
				)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			QueryPerformanceCounter(
				&integer
			);

			return TimeSpan(
				static_cast<uint64>(
					(integer.QuadPart - start) / frequency
				)
			);
#endif
		}

		void Reset()
		{
#if defined(AL_PLATFORM_LINUX)
			start = Clock::now();
#elif defined(AL_PLATFORM_WINDOWS)
			QueryPerformanceCounter(
				&integer
			);

			start = static_cast<double>(
				integer.QuadPart
			);
#endif
		}
	};
}
