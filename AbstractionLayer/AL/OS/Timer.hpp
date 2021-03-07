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

		Timer(const Timer&) = delete;

	public:
		Timer()
#if defined(AL_PLATFORM_WINDOWS)
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
#endif
		{
			Reset();
		}

		Timer(Timer&& timer)
#if defined(AL_PLATFORM_LINUX)
			: start(
				Move(timer.start)
			)
#elif defined(AL_PLATFORM_WINDOWS)
			: start(
				timer.start
			),
			frequency(
				timer.frequency
			),
			integer(
				Move(timer.integer)
			)
#endif
		{
			timer.Reset();
		}

		virtual ~Timer()
		{
		}

		auto GetElapsed() const
		{
#if defined(AL_PLATFORM_LINUX)
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
				Clock::now() - start
			).count();

			return TimeSpan::FromMicroseconds(
				static_cast<uint64>(
					elapsed
				)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			QueryPerformanceCounter(
				&integer
			);

			return TimeSpan::FromMicroseconds(
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

		auto& operator = (Timer&& timer)
		{
#if defined(AL_PLATFORM_LINUX)
			start = Move(
				timer.start
			);
#elif defined(AL_PLATFORM_WINDOWS)
			start = timer.start;
			
			integer = Move(
				timer.integer
			);

			frequency = timer.frequency;
#endif

			timer.Reset();

			return *this;
		}
	};
}
