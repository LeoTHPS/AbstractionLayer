#pragma once
#include "AL/Common.hpp"

namespace AL
{
	template<typename T>
	struct BitMask
	{
		typedef T Type;

		static constexpr Bool IsSet(Type mask, Type flag)
		{
			typedef typename Get_Enum_Or_Integer_Base<Type>::Type TypeBase;

			return static_cast<TypeBase>(flag) ? ((static_cast<TypeBase>(mask) & static_cast<TypeBase>(flag)) == static_cast<TypeBase>(flag)) : False;
		}

		static constexpr Type Add(Type mask, Type flag)
		{
			typedef typename Get_Enum_Or_Integer_Base<Type>::Type TypeBase;

			return static_cast<Type>(static_cast<TypeBase>(mask) | static_cast<TypeBase>(flag));
		}

		static constexpr Type Remove(Type mask, Type flag)
		{
			typedef typename Get_Enum_Or_Integer_Base<Type>::Type TypeBase;

			return static_cast<Type>(static_cast<TypeBase>(mask) & ~static_cast<TypeBase>(flag));
		}

		static constexpr Type Set(Type mask, Type flag, Bool set = True)
		{
			return set ? Add(mask, flag) : Remove(mask, flag);;
		}

		static constexpr Type Clear(Type mask)
		{
			return Integer<Type>::Zero;
		}

		Type Value;

		BitMask()
			: BitMask(
				Type()
			)
		{
		}

		BitMask(Type value)
			: Value(
				value
			)
		{
		}

		BitMask(BitMask&& mask)
			: Value(
				Move(mask.Value)
			)
		{
		}

		BitMask(const BitMask& mask)
			: Value(
				mask.Value
			)
		{
		}

		Bool IsSet(Type flag) const
		{
			if (!BitMask<Type>::IsSet(Value, flag))
			{

				return False;
			}

			return True;
		}

		Void Add(Type flag)
		{
			Value = BitMask<Type>::Add(
				Value,
				flag
			);
		}

		Void Remove(Type flag)
		{
			Value = BitMask<Type>::Remove(
				Value,
				flag
			);
		}

		Void Set(Type flag, Bool set = True)
		{
			Value = BitMask<Type>::Set(
				Value,
				flag,
				set
			);
		}

		Void Clear()
		{
			Value = BitMask<Type>::Clear(
				Value
			);
		}

		BitMask& operator = (Type value)
		{
			Value = value;

			return *this;
		}

		BitMask& operator = (BitMask&& mask)
		{
			Value = Move(
				mask.Value
			);

			return *this;
		}
		BitMask& operator = (const BitMask& mask)
		{
			Value = mask.Value;

			return *this;
		}

		Bool operator == (const BitMask& mask) const
		{
			if (Value != mask.Value)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const BitMask& mask) const
		{
			if (operator==(mask))
			{

				return False;
			}

			return True;
		}
	};
}
