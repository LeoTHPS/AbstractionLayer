#pragma once
#include "AL/Common.hpp"

#include <cmath>

namespace AL
{
	class Math
	{
		Math() = delete;

	public:
		static constexpr Double PI      = 3.14159265358979323846;
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
		static constexpr T Abs(T value)
		{
			return ::std::abs(
				value
			);
		}

		template<typename T>
		static constexpr T Exp(T value)
		{
			return ::std::exp(
				value
			);
		}

		template<typename T>
		static constexpr T Fabs(T value)
		{
			return ::std::fabs(
				value
			);
		}

		template<typename T>
		static constexpr T Log(T value)
		{
			return ::std::log(
				value
			);
		}

		template<typename T>
		static constexpr T Log2(T value)
		{
			return ::std::log2(
				value
			);
		}

		template<typename T>
		static constexpr T Log10(T value)
		{
			return ::std::log10(
				value
			);
		}

		template<typename T>
		static constexpr T Cos(T value)
		{
			return ::std::cos(
				value
			);
		}

		template<typename T>
		static constexpr T Pow(T value, T exponent)
		{
			return ::std::pow(
				value,
				exponent
			);
		}

		template<typename T>
		static constexpr T Rad(T value)
		{
			return static_cast<T>(value * Rad2Deg);
		}

		template<typename T>
		static constexpr T Sin(T value)
		{
			return ::std::sin(
				value
			);
		}

		template<typename T>
		static constexpr T Sqrt(T value)
		{
			return ::std::sqrt(
				value
			);
		}

		template<typename T>
		static constexpr T Atan2(T y, T x)
		{
			return ::std::atan2(
				y,
				x
			);
		}

		template<typename T, size_t S>
		static           auto Add(const T(&values)[S])
		{
			auto result  = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				result += *lpValue;
			}

			return result;
		}
		template<typename ... T_VALUES>
		static constexpr auto Add(T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{

				return (values + ...);
			}
		}

		template<typename T, size_t S>
		static           auto Subtract(const T(&values)[S])
		{
			auto result  = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				result -= *lpValue;
			}

			return result;
		}
		template<typename ... T_VALUES>
		static constexpr auto Subtract(T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return _Subtract<T_VALUES ...>(
					Forward<T_VALUES>(values) ...
				);
			}
		}

		template<typename T, size_t S>
		static constexpr auto Average(const T(&values)[S])
		{
			return Add<T, S>(values) / S;
		}
		template<typename ... T_VALUES>
		static constexpr auto Average(T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{

				return Add<T_VALUES ...>(Forward<T_VALUES>(values) ...) / (sizeof ...(T_VALUES));
			}
		}

		template<typename T, size_t S>
		static           auto Lowest(const T(&values)[S])
		{
			auto result  = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				if (*lpValue < result)
				{

					result = *lpValue;
				}
			}

			return result;
		}
		template<typename ... T_VALUES>
		static constexpr auto Lowest(T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return _Lowest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...
				);
			}
		}

		template<typename T, size_t S>
		static           auto Highest(const T(&values)[S])
		{
			auto result  = values[0];
			auto lpValue = &values[1];

			for (size_t i = 1; i < S; ++i, ++lpValue)
			{
				if (*lpValue > result)
				{

					result = *lpValue;
				}
			}

			return result;
		}
		template<typename ... T_VALUES>
		static constexpr auto Highest(T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return _Highest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...
				);
			}
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
		static constexpr T Round(T value, T min, T max)
		{
			return ((value - min) < (max - value)) ? min : max;
		}

	private:
		template<typename T_VALUE>
		static constexpr auto _Subtract(T_VALUE value)
		{
			return value;
		}
		template<typename T_VALUE_1, typename T_VALUE_2, typename ... T_VALUES>
		static constexpr auto _Subtract(T_VALUE_1 value1, T_VALUE_2 value2, T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return __Subtract<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					value1 - value2
				);
			}
			else
			{

				return value1 - value2;
			}
		}
		template<typename T_VALUE, typename ... T_VALUES, typename T_RESULT>
		static constexpr auto __Subtract(T_VALUE value, T_VALUES ... values, T_RESULT result)
		{
			if constexpr (sizeof ...(T_VALUES) == 0)
			{

				return result - value;
			}
			else
			{

				return __Subtract<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					result - value
				);
			}
		}

		template<typename T_VALUE>
		static constexpr auto _Lowest(T_VALUE value)
		{
			return value;
		}
		template<typename T_VALUE_1, typename T_VALUE_2, typename ... T_VALUES>
		static constexpr auto _Lowest(T_VALUE_1 value1, T_VALUE_2 value2, T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return __Lowest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					(value1 < value2) ? value1 : value2
				);
			}
			else
			{

				return (value1 < value2) ? value1 : value2;
			}
		}
		template<typename T_VALUE, typename ... T_VALUES, typename T_RESULT>
		static constexpr auto __Lowest(T_VALUE value, T_VALUES ... values, T_RESULT result)
		{
			if constexpr (sizeof ...(T_VALUES) == 0)
			{

				return (value < result) ? value : result;
			}
			else
			{

				return __Lowest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					(value < result) ? value : result
				);
			}
		}

		template<typename T_VALUE>
		static constexpr auto _Highest(T_VALUE value)
		{
			return value;
		}
		template<typename T_VALUE_1, typename T_VALUE_2, typename ... T_VALUES>
		static constexpr auto _Highest(T_VALUE_1 value1, T_VALUE_2 value2, T_VALUES ... values)
		{
			if constexpr (sizeof ...(T_VALUES) > 0)
			{
				return __Highest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					(value1 > value2) ? value1 : value2
				);
			}
			else
			{

				return (value1 > value2) ? value1 : value2;
			}
		}
		template<typename T_VALUE, typename ... T_VALUES, typename T_RESULT>
		static constexpr auto __Highest(T_VALUE value, T_VALUES ... values, T_RESULT result)
		{
			if constexpr (sizeof ...(T_VALUES) == 0)
			{

				return (value > result) ? value : result;
			}
			else
			{

				return __Highest<T_VALUES ...>(
					Forward<T_VALUES>(values) ...,
					(value > result) ? value : result
				);
			}
		}
	};
}
