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
				uint8 bytes[4];

				bytes[0] = reinterpret_cast<const uint8*>(&value)[3];
				bytes[1] = reinterpret_cast<const uint8*>(&value)[2];
				bytes[2] = reinterpret_cast<const uint8*>(&value)[1];
				bytes[3] = reinterpret_cast<const uint8*>(&value)[0];

				return *reinterpret_cast<const T*>(
					&bytes[0]
				);
			}
		};
		template<typename T>
		struct Flip_Decimal<T, 64>
		{
			static T DoTheThing(T value)
			{
				uint8 bytes[8];

				bytes[0] = reinterpret_cast<const uint8*>(&value)[7];
				bytes[1] = reinterpret_cast<const uint8*>(&value)[6];
				bytes[2] = reinterpret_cast<const uint8*>(&value)[5];
				bytes[3] = reinterpret_cast<const uint8*>(&value)[4];
				bytes[4] = reinterpret_cast<const uint8*>(&value)[3];
				bytes[5] = reinterpret_cast<const uint8*>(&value)[2];
				bytes[6] = reinterpret_cast<const uint8*>(&value)[1];
				bytes[7] = reinterpret_cast<const uint8*>(&value)[0];

				return *reinterpret_cast<const T*>(
					&bytes[0]
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
		static T FromFloat(Float value, Bool roundUp = True)
		{
			Float fraction;

			return FromFloat<T>(
				value,
				fraction,
				roundUp
			);
		}
		template<typename T>
		static T FromFloat(Float value, Float& fraction, Bool roundUp = True)
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
		static T FromDouble(Double value, Bool roundUp = True)
		{
			Double fraction;

			return FromDouble<T>(
				value,
				fraction,
				roundUp
			);
		}
		template<typename T>
		static T FromDouble(Double value, Double& fraction, Bool roundUp = True)
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
