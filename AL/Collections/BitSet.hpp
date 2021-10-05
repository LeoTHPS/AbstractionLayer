#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename T>
	class BitSet
	{
		static_assert(
			Is_Enum_Or_Integer<T>::Value,
			"T must be enum or integer"
		);

		static constexpr size_t T_BIT_COUNT = sizeof(T) * 8;

		typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE_TYPE;

		Bool bits[T_BIT_COUNT];

	public:
		typedef T Type;

		BitSet()
			: BitSet(
				Type(0)
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

			for (size_t i = 0; i < GetSize(); ++i)
			{

				value |= static_cast<T_BASE_TYPE>(bits[i]) << i;
			}

			return static_cast<Type>(
				value
			);
		}

		Void SetValue(Type value)
		{
			for (size_t i = 0; i < GetSize(); ++i)
			{
				
				bits[i] = (static_cast<T_BASE_TYPE>(value) >> i) & 0x1;
			}
		}

		auto& operator [] (size_t index)
		{
			return bits[index];
		}
		auto  operator [] (size_t index) const
		{
			return bits[index];
		}

		Bool operator == (const BitSet& bitSet) const
		{
			if (!memcmp(bits, bitSet.bits))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const BitSet& bitSet) const
		{
			if (operator==(bitSet))
			{

				return False;
			}

			return True;
		}
	};
}
