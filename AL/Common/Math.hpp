#pragma once
#include "AL/Common.hpp"

#include <cmath>

namespace AL
{
	class Math
	{
		Math() = delete;

	public:
		static constexpr Double PI = 3.14159265358979323846;

		static constexpr Double Rad2Deg = 360 / (PI * 2);

		template<typename T>
		static constexpr Bool IsPowerOf2(T value)
		{
			return value && !(value & (value - static_cast<T>(1)));
		}

		template<typename T>
		static constexpr Bool IsInRange(T value, T min, T max)
		{
			return (value >= min) && (value <= max);
		}

		template<typename T>
		static constexpr T Exp(T value)
		{
			return std::exp(
				value
			);
		}

		template<typename T>
		static constexpr T Log(T value)
		{
			return std::log(
				value
			);
		}

		template<typename T>
		static constexpr T Pow(T value, T exponent)
		{
			return std::pow(
				value,
				exponent
			);
		}

		template<typename T, size_t S>
		static T Lowest(const T(&values)[S])
		{
			auto lowest = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				if (*lpValue < lowest)
				{

					lowest = *lpValue;
				}
			}

			return lowest;
		}

		template<typename T, size_t S>
		static T Highest(const T(&values)[S])
		{
			auto highest = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				if (*lpValue > highest)
				{

					highest = *lpValue;
				}
			}

			return highest;
		}

		template<typename T>
		static constexpr T Clamp(T value, T min, T max)
		{
			if (value < min)
			{

				value = min;
			}
			else if (value > max)
			{

				value = max;
			}

			return value;
		}

		template<typename T>
		static constexpr T Round(T value, T low, T high)
		{
			return ((value - low) < (high - value)) ? low : high;
		}
	};
}
