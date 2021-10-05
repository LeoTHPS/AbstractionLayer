#pragma once
#include "AL/Common.hpp"

#include <math.h>

namespace AL
{
	class BitConverter
	{
		template<typename T, size_t BIT_COUNT = sizeof(T) * 8>
		struct Flip_Decimal;
		template<typename T>
		struct Flip_Decimal<T, 32>
		{
			static T DoTheThing(T value)
			{
				struct Bytes
				{
					uint8 B1, B2, B3, B4;
				} bytes;

				bytes.B1 = reinterpret_cast<const Bytes*>(&value)->B4;
				bytes.B2 = reinterpret_cast<const Bytes*>(&value)->B3;
				bytes.B3 = reinterpret_cast<const Bytes*>(&value)->B2;
				bytes.B4 = reinterpret_cast<const Bytes*>(&value)->B1;

				return *reinterpret_cast<const T*>(
					&bytes
				);
			}
		};
		template<typename T>
		struct Flip_Decimal<T, 64>
		{
			static T DoTheThing(T value)
			{
				struct Bytes
				{
					uint8 B1, B2, B3, B4, B5, B6, B7, B8;
				} bytes;

				bytes.B1 = reinterpret_cast<const Bytes*>(&value)->B8;
				bytes.B2 = reinterpret_cast<const Bytes*>(&value)->B7;
				bytes.B3 = reinterpret_cast<const Bytes*>(&value)->B6;
				bytes.B4 = reinterpret_cast<const Bytes*>(&value)->B5;
				bytes.B5 = reinterpret_cast<const Bytes*>(&value)->B4;
				bytes.B6 = reinterpret_cast<const Bytes*>(&value)->B3;
				bytes.B7 = reinterpret_cast<const Bytes*>(&value)->B2;
				bytes.B8 = reinterpret_cast<const Bytes*>(&value)->B1;

				return *reinterpret_cast<const T*>(
					&bytes
				);
			}
		};

		template<typename T, size_t BIT_COUNT = sizeof(T) * 8>
		struct Flip_Enum_Or_Integer;
		template<typename T>
		struct Flip_Enum_Or_Integer<T, 8>
		{
			static constexpr T DoTheThing(T value)
			{
				return value;
			}
		};
		template<typename T>
		struct Flip_Enum_Or_Integer<T, 16>
		{
			static constexpr T DoTheThing(T value)
			{
				typedef typename Get_Enum_Or_Integer_Base<T>::Type Base;

				return static_cast<T>(
					((static_cast<Base>(value) & 0x00FF) << 8) |
					((static_cast<Base>(value) & 0xFF00) >> 8)
				);
			}
		};
		template<typename T>
		struct Flip_Enum_Or_Integer<T, 32>
		{
			static constexpr T DoTheThing(T value)
			{
				typedef typename Get_Enum_Or_Integer_Base<T>::Type Base;

				return static_cast<T>(
					((static_cast<Base>(value) & 0xFF000000) >> 24) |
					((static_cast<Base>(value) & 0x00FF0000) >> 8) |
					((static_cast<Base>(value) & 0x0000FF00) << 8) |
					((static_cast<Base>(value) & 0x000000FF) << 24)
				);
			}
		};
		template<typename T>
		struct Flip_Enum_Or_Integer<T, 64>
		{
			static constexpr T DoTheThing(T value)
			{
				typedef typename Get_Enum_Or_Integer_Base<T>::Type Base;

				return static_cast<T>(
					((static_cast<Base>(value) & 0xFF00000000000000) >> 56) |
					((static_cast<Base>(value) & 0x00FF000000000000) >> 40) |
					((static_cast<Base>(value) & 0x0000FF0000000000) >> 24) |
					((static_cast<Base>(value) & 0x000000FF00000000) >> 8) |
					((static_cast<Base>(value) & 0x00000000FF000000) << 8) |
					((static_cast<Base>(value) & 0x0000000000FF0000) << 24) |
					((static_cast<Base>(value) & 0x000000000000FF00) << 40) |
					((static_cast<Base>(value) & 0x00000000000000FF) << 56)
				);
			}
		};

		BitConverter() = delete;

	public:
		static constexpr Bool IsBigEndian()
		{
			return Endians::Machine == Endians::Big;
		}

		static constexpr Bool IsLittleEndian()
		{
			return Endians::Machine == Endians::Little;
		}

		template<typename T>
		static constexpr T Flip(T value)
		{
			if constexpr (Is_Decimal<T>::Value)
			{

				return Flip_Decimal<T>::DoTheThing(
					value
				);
			}
			else if constexpr (Is_Enum_Or_Integer<T>::Value)
			{

				return Flip_Enum_Or_Integer<T>::DoTheThing(
					value
				);
			}
		}

		template<typename T>
		static constexpr T ToBigEndian(T value)
		{
			return !IsBigEndian() ? Flip(value) : value;
		}

		template<typename T>
		static constexpr T ToLittleEndian(T value)
		{
			return !IsLittleEndian() ? Flip(value) : value;
		}

		template<typename T>
		static constexpr T FromBigEndian(T value)
		{
			return IsLittleEndian() ? Flip(value) : value;
		}
		
		template<typename T>
		static constexpr T FromLittleEndian(T value)
		{
			return IsBigEndian() ? Flip(value) : value;
		}

		template<typename T>
		static constexpr T NetworkToHost(T value)
		{
			return IsLittleEndian() ? Flip(value) : value;
		}

		template<typename T>
		static constexpr T HostToNetwork(T value)
		{
			return IsLittleEndian() ? Flip(value) : value;
		}

		template<typename T_NEW_TYPE, typename T_OLD_TYPE>
		static constexpr T_NEW_TYPE Cast(T_OLD_TYPE value)
		{
			if constexpr (Is_Float<T_OLD_TYPE>::Value)
			{
				return FromFloat<T_NEW_TYPE>(
					value
				);
			}
			else if constexpr (Is_Double<T_OLD_TYPE>::Value)
			{
				return FromDouble<T_NEW_TYPE>(
					value
				);
			}
			else
			{
				return static_cast<T_NEW_TYPE>(
					value
				);
			}
		}

		template<typename T>
		static T FromFloat(Float value, Bool roundUp = true)
		{
			Float fraction;

			return FromFloat<T>(
				value,
				fraction,
				roundUp
			);
		}
		template<typename T>
		static T FromFloat(Float value, Float& fraction, Bool roundUp = true)
		{
			Float baseValue;

			fraction = ::modff(
				value,
				&baseValue
			);

			if (roundUp && Math::Round(fraction, 0.0f, 1.0f))
			{

				return static_cast<T>(
					baseValue + 1
				);
			}

			return static_cast<T>(
				baseValue
			);
		}

		template<typename T>
		static T FromDouble(Double value, Bool roundUp = true)
		{
			Double fraction;

			return FromDouble<T>(
				value,
				fraction,
				roundUp
			);
		}
		template<typename T>
		static T FromDouble(Double value, Double& fraction, Bool roundUp = true)
		{
			Double baseValue;

			fraction = ::modf(
				value,
				&baseValue
			);

			if (roundUp && Math::Round(fraction, 0.0, 1.0))
			{

				return static_cast<T>(
					baseValue + 1
				);
			}

			return static_cast<T>(
				baseValue
			);
		}
	};
}
