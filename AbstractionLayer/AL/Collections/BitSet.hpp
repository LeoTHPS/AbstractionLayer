#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"

namespace AL::Collections
{
	template<typename T, Endians ENDIAN = Endians::Machine>
	class BitSet
	{
		static_assert(
			Is_Enum_Or_Integer<T>::Value,
			"T must be enum or integer"
		);

		static constexpr size_t T_BIT_COUNT = sizeof(T) * 8;

		typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE_TYPE;

		Array<bool[T_BIT_COUNT]> bits;

	public:
		typedef T Type;

		BitSet()
			: BitSet(
				static_cast<Type>(0)
			)
		{
		}

		BitSet(Type value)
		{
			SetValue(
				value
			);
		}

		static constexpr size_t GetSize()
		{
			return T_BIT_COUNT;
		}

		auto GetValue() const
		{
			T_BASE_TYPE value = 0;

			for (size_t i = 1; i <= GetSize(); ++i)
			{
				value |= static_cast<T_BASE_TYPE>(
					static_cast<T_BASE_TYPE>(1) << (T_BIT_COUNT - i)
				);
			}

			return static_cast<Type>(
				value
			);
		}

		void SetValue(Type value)
		{
			for (size_t i = 0, j = 1; i < GetSize(); ++i, ++j)
			{
				bits[i] = (static_cast<T_BASE_TYPE>(value) >> (T_BIT_COUNT - j)) & 0x1;
			}
		}

		auto& operator [] (size_t index)
		{
			return bits[index];
		}
		auto operator [] (size_t index) const
		{
			return bits[index];
		}

		bool operator == (const BitSet& bitSet) const
		{
			return bits == bitSet.bits;
		}
		bool operator != (const BitSet& bitSet) const
		{
			return !operator==(
				bitSet
			);
		}
	};
}
