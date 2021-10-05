#pragma once
#include "AL/Common.hpp"

#include <cfloat>
#include <typeinfo>
#include <type_traits>

#define AL_DEFINE_ENUM_FLAG_OPERATORS_OR(e) \
	inline e operator | (e a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return static_cast<e>(static_cast<BaseType>(a) | static_cast<BaseType>(b)); \
	} \
	inline e& operator |= (e& a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return reinterpret_cast<e&>(reinterpret_cast<BaseType&>(a) |= static_cast<BaseType>(b)); \
	}
	
#define AL_DEFINE_ENUM_FLAG_OPERATORS_AND(e) \
	inline e operator & (e a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return static_cast<e>(static_cast<BaseType>(a) & static_cast<BaseType>(b)); \
	} \
	inline e& operator &= (e& a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return reinterpret_cast<e&>(reinterpret_cast<BaseType&>(a) &= static_cast<BaseType>(b)); \
	}

#define AL_DEFINE_ENUM_FLAG_OPERATORS_NOT(e) \
	inline e operator ~ (e a) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return static_cast<e>(~static_cast<BaseType>(a)); \
	}

#define AL_DEFINE_ENUM_FLAG_OPERATORS_XOR(e) \
	inline e operator ^ (e a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return static_cast<e>(static_cast<BaseType>(a) ^ static_cast<BaseType>(b)); \
	} \
	inline e& operator ^= (e& a, e b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<e>::Type BaseType; \
		return reinterpret_cast<e&>(reinterpret_cast<BaseType&>(a) ^= static_cast<BaseType>(b)); \
	}

#define AL_DEFINE_ENUM_FLAG_OPERATORS(e) \
	AL_DEFINE_ENUM_FLAG_OPERATORS_OR(e); \
	AL_DEFINE_ENUM_FLAG_OPERATORS_AND(e); \
	AL_DEFINE_ENUM_FLAG_OPERATORS_NOT(e); \
	AL_DEFINE_ENUM_FLAG_OPERATORS_XOR(e)

namespace AL
{
	template<typename T>
	struct Is_POD
	{
		static constexpr Bool Value = std::is_pod<T>::value;
	};

	template<typename T_1, typename T_2>
	struct Is_Type
	{
		static constexpr Bool Value = False;
	};
	template<typename T>
	struct Is_Type<T, T>
	{
		static constexpr Bool Value = True;
	};

	template<typename T>
	struct Is_Enum
	{
		static constexpr Bool Value = std::is_enum<T>::value;
	};

	template<typename T>
	struct Is_Integer
	{
		static constexpr Bool Value = std::is_integral<T>::value;
	};

	template<typename T>
	struct Is_Enum_Or_Integer
	{
		static constexpr Bool Value = Is_Enum<T>::Value || Is_Integer<T>::Value;
	};

	template<typename T>
	struct Is_Float
	{
		static constexpr Bool Value = Is_Type<T, Float>::Value;
	};

	template<typename T>
	struct Is_Double
	{
		static constexpr Bool Value = Is_Type<T, Double>::Value;
	};

	template<typename T>
	struct Is_Decimal
	{
		static constexpr Bool Value = Is_Float<T>::Value || Is_Double<T>::Value;
	};

	template<typename T>
	struct Is_Signed
	{
		static constexpr Bool Value = std::is_signed<T>::value;
	};

	template<typename T>
	struct Is_Unsigned
	{
		static constexpr Bool Value = std::is_unsigned<T>::value;
	};

	template<typename T>
	struct Is_Const
	{
		static constexpr Bool Value = std::is_const<T>::value;
	};

	template<typename T>
	struct Is_Pointer
	{
		static constexpr Bool Value = std::is_pointer<T>::value;
	};

	template<typename T>
	struct Is_Reference
	{
		static constexpr Bool Value = std::is_reference<T>::value;
	};

	template<typename BASE, typename T>
	struct Is_Base_Of
	{
		static constexpr Bool Value = std::is_base_of<BASE, T>::value;
	};

	template<typename TYPE, typename TYPE2>
	struct Is_Convertable
	{
		static constexpr Bool Value = std::is_convertible<TYPE, TYPE2>::value;
	};

	template<typename T>
	struct Is_Copy_Constructible
	{
		static constexpr Bool Value = std::is_copy_constructible<T>::value;
	};

	template<typename T>
	struct Is_Copy_Assignable
	{
		static constexpr Bool Value = std::is_copy_assignable<T>::value;
	};
	
	template<typename T>
	struct Is_Move_Constructible
	{
		static constexpr Bool Value = std::is_move_constructible<T>::value;
	};

	template<typename T>
	struct Is_Move_Assignable
	{
		static constexpr Bool Value = std::is_move_assignable<T>::value;
	};

	template<size_t SIZE, Bool IS_SIGNED>
	struct _Get_Enum_Or_Integer_Base;

	template<typename T>
	struct Get_Enum_Or_Integer_Base
		: public _Get_Enum_Or_Integer_Base<sizeof(T), Is_Signed<T>::Value>
	{
		static_assert(
			Is_Enum_Or_Integer<T>::Value,
			"T must be enum or integer"
		);
	};

	template<size_t SIZE, Bool IS_SIGNED>
	struct _Get_Decimal_Base;

	template<typename T>
	struct Get_Decimal_Base
		: public _Get_Decimal_Base<sizeof(T), Is_Signed<T>::Value>
	{
		static_assert(
			Is_Decimal<T>::Value,
			"T must be decimal"
		);
	};

	template<Bool CONDITION, typename T_1, typename T_2>
	struct Conditional
	{
		typedef T_1 Type;
	};
	template<typename T_1, typename T_2>
	struct Conditional<False, T_1, T_2>
	{
		typedef T_2 Type;
	};

	template<Bool CONDITION, typename T, T V_1, T V_2>
	struct Conditional_Value
	{
		static constexpr T Value = V_1;
	};
	template<typename T, T V_1, T V_2>
	struct Conditional_Value<False, T, V_1, V_2>
	{
		static constexpr T Value = V_2;
	};

	template<Bool CONDITION, typename T>
	struct Inherit_If
	{
		typedef T Type;
	};
	template<typename T>
	struct Inherit_If<False, T>
	{
		struct Empty
		{
		};

		typedef Empty Type;
	};

	template<Bool CONDITION, typename T_1, typename T_2>
	using Inherit_If_Else = Conditional<CONDITION, T_1, T_2>;

	template<Bool CONDITION, typename T>
	struct Enable_If
	{
		typedef T Type;
	};
	template<typename T>
	struct Enable_If<False, T>
	{
	};

	template<typename T>
	struct Remove_Const
	{
		typedef T Type;
	};
	template<typename T>
	struct Remove_Const<const T>
	{
		typedef T Type;
	};

	template<typename T>
	struct Remove_Pointer
	{
		typedef T Type;
	};
	template<typename T>
	struct Remove_Pointer<T*>
	{
		typedef T Type;
	};

	template<typename T>
	struct Remove_Reference
	{
		typedef T Type;
	};
	template<typename T>
	struct Remove_Reference<T&>
	{
		typedef T Type;
	};

	template<typename T>
	struct Remove_Modifiers
	{
		typedef T Type;
	};
	template<typename T>
	struct Remove_Modifiers<T&>
		: public Remove_Modifiers<T>
	{
	};
	template<typename T>
	struct Remove_Modifiers<T*>
		: public Remove_Modifiers<T>
	{
	};
	template<typename T>
	struct Remove_Modifiers<const T>
		: public Remove_Modifiers<T>
	{
	};

	template<typename T>
	struct Type
	{
		inline static const char*  Name = typeid(T).name();
		inline static const size_t Hash = typeid(T).hash_code();
	};

	template<typename T>
	struct Integer;

	template<typename T>
	struct Enum
		: public Integer<typename Get_Enum_Or_Integer_Base<T>::Type>
	{
		static_assert(
			Is_Enum<T>::Value,
			"T must be enum"
		);

		typedef typename Get_Enum_Or_Integer_Base<T>::Type BaseType;
	};

	template<typename T>
	struct _Decimal;
	template<>
	struct _Decimal<Float>
	{
		static constexpr Bool IsSigned   = Is_Signed<Float>::Value;
		static constexpr Bool IsUnsigned = Is_Unsigned<Float>::Value;

		static constexpr Float Minimum = FLT_MIN;
		static constexpr Float Maximum = FLT_MAX;
	};
	template<>
	struct _Decimal<Double>
	{
		static constexpr Bool IsSigned   = Is_Signed<Double>::Value;
		static constexpr Bool IsUnsigned = Is_Unsigned<Double>::Value;

		static constexpr Double Minimum = DBL_MIN;
		static constexpr Double Maximum = DBL_MAX;
	};

	template<typename T>
	using Decimal = _Decimal<typename Get_Decimal_Base<T>::Type>;

	template<>
	struct Integer<int8>
	{
		static constexpr Bool IsSigned   = True;
		static constexpr Bool IsUnsigned = False;

		static constexpr int8 Minimum = int8(-0x7F);
		static constexpr int8 Maximum = int8(0x7F);

		static constexpr int8 SignedCastMask   = int8(0x7F);
		static constexpr int8 UnsignedCastMask = int8(0xFF);

		int8 Value;
	};
	template<>
	struct Integer<uint8>
	{
		static constexpr Bool IsSigned   = False;
		static constexpr Bool IsUnsigned = True;

		static constexpr uint8 Minimum = uint8(0x0);
		static constexpr uint8 Maximum = uint8(0xFF);

		static constexpr uint8 SignedCastMask   = uint8(0x7F);
		static constexpr uint8 UnsignedCastMask = uint8(0xFF);

		uint8 Value;
	};
	template<>
	struct Integer<int16>
	{
		static constexpr Bool IsSigned   = True;
		static constexpr Bool IsUnsigned = False;

		static constexpr int16 Minimum = int16(-0x7FFF);
		static constexpr int16 Maximum = int16(0x7FFF);

		static constexpr int16 SignedCastMask   = int16(0x7FFF);
		static constexpr int16 UnsignedCastMask = int16(0xFFFF);

		union
		{
			struct
			{
				Integer<int8> Low;
				Integer<int8> High;
			};

			int16 Value;
		};
	};
	template<>
	struct Integer<uint16>
	{
		static constexpr Bool IsSigned   = False;
		static constexpr Bool IsUnsigned = True;

		static constexpr uint16 Minimum = uint16(0x0);
		static constexpr uint16 Maximum = uint16(0xFFFF);

		static constexpr uint16 SignedCastMask   = uint16(0x7FFF);
		static constexpr uint16 UnsignedCastMask = uint16(0xFFFF);

		union
		{
			struct
			{
				Integer<uint8> Low;
				Integer<uint8> High;
			};

			uint16 Value;
		};
	};
	template<>
	struct Integer<int32>
	{
		static constexpr Bool IsSigned   = True;
		static constexpr Bool IsUnsigned = False;

		static constexpr int32 Minimum = int32(-0x7FFFFFFF);
		static constexpr int32 Maximum = int32(0x7FFFFFFF);

		static constexpr int32 SignedCastMask   = int32(0x7FFFFFFF);
		static constexpr int32 UnsignedCastMask = int32(0xFFFFFFFF);

		union
		{
			struct
			{
				Integer<int16> Low;
				Integer<int16> High;
			};

			int32 Value;
		};
	};
	template<>
	struct Integer<uint32>
	{
		static constexpr Bool IsSigned   = False;
		static constexpr Bool IsUnsigned = True;

		static constexpr uint32 Minimum = uint32(0x0);
		static constexpr uint32 Maximum = uint32(0xFFFFFFFF);

		static constexpr uint32 SignedCastMask   = uint32(0x7FFFFFFF);
		static constexpr uint32 UnsignedCastMask = uint32(0xFFFFFFFF);

		union
		{
			struct
			{
				Integer<uint16> Low;
				Integer<uint16> High;
			};

			uint32 Value;
		};
	};
	template<>
	struct Integer<int64>
	{
		static constexpr Bool IsSigned   = True;
		static constexpr Bool IsUnsigned = False;

		static constexpr int64 Minimum = int64(-0x7FFFFFFFFFFFFFFF);
		static constexpr int64 Maximum = int64(0x7FFFFFFFFFFFFFFF);

		static constexpr int64 SignedCastMask   = int64(0x7FFFFFFFFFFFFFFF);
		static constexpr int64 UnsignedCastMask = int64(0xFFFFFFFFFFFFFFFF);

		union
		{
			struct
			{
				Integer<int32> Low;
				Integer<int32> High;
			};

			int64 Value;
		};
	};
	template<>
	struct Integer<uint64>
	{
		static constexpr Bool IsSigned   = False;
		static constexpr Bool IsUnsigned = True;

		static constexpr uint64 Minimum = uint64(0x0);
		static constexpr uint64 Maximum = uint64(0xFFFFFFFFFFFFFFFF);

		static constexpr uint64 SignedCastMask   = uint64(0x7FFFFFFFFFFFFFFF);
		static constexpr uint64 UnsignedCastMask = uint64(0xFFFFFFFFFFFFFFFF);

		union
		{
			struct
			{
				Integer<uint32> Low;
				Integer<uint32> High;
			};

			uint64 Value;
		};
	};

	template<typename T>
	struct Enum_Or_Integer
		: public Integer<typename Get_Enum_Or_Integer_Base<T>::Type>
	{
		static_assert(
			Is_Enum_Or_Integer<T>::Value,
			"T must be enum or integer"
		);

		typedef typename Get_Enum_Or_Integer_Base<T>::Type BaseType;
	};

	template<typename ... TYPES>
	struct Type_Sequence
	{
	};

	template<size_t I, typename ... TYPES>
	struct Get_Type_Sequence;
	template<size_t I, typename TYPE, typename ... TYPES>
	struct Get_Type_Sequence<I, TYPE, TYPES ...>
		: public Get_Type_Sequence<I - 1, TYPES ...>
	{
	};
	template<typename TYPE, typename ... TYPES>
	struct Get_Type_Sequence<0, TYPE, TYPES ...>
	{
		typedef TYPE Type;
	};
	template<size_t I, typename TYPE, typename ... TYPES>
	struct Get_Type_Sequence<I, Type_Sequence<TYPE, TYPES ...>>
		: public Get_Type_Sequence<I - 1, Type_Sequence<TYPES ...>>
	{
	};
	template<typename TYPE, typename ... TYPES>
	struct Get_Type_Sequence<0, Type_Sequence<TYPE, TYPES ...>>
	{
		typedef TYPE Type;
	};

	template<size_t ... INDEXES>
	struct Index_Sequence
	{
	};

	template<size_t I, size_t COUNT, typename INDEXES>
	struct _Make_Index_Sequence;
	template<size_t I, size_t ... INDEXES>
	struct _Make_Index_Sequence<I, I, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};
	template<size_t I, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence<I, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence<I + 1, COUNT, Index_Sequence<INDEXES ..., I>>
	{
	};

	template<size_t COUNT>
	using Make_Index_Sequence = _Make_Index_Sequence<0, COUNT, Index_Sequence<>>;

	template<size_t I, size_t NOT, size_t COUNT, typename INDEXES>
	struct _Make_Index_Sequence_Not;
	template<size_t I, size_t NOT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Not<I, NOT, I, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};
	template<size_t I, size_t NOT, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Not<I, NOT, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Not<I + 1, NOT, COUNT, Index_Sequence<INDEXES ..., I>>
	{
	};
	template<size_t NOT, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Not<NOT, NOT, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Not<NOT + 1, NOT, COUNT, Index_Sequence<INDEXES ...>>
	{
	};

	template<size_t COUNT, size_t NOT>
	using Make_Index_Sequence_Not = _Make_Index_Sequence_Not<0, NOT, COUNT, Index_Sequence<>>;

	template<size_t I, size_t I_END, typename INDEXES>
	struct _Make_Index_Sequence_Range;
	template<size_t I, size_t I_END, size_t ... INDEXES>
	struct _Make_Index_Sequence_Range<I, I_END, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Range<I + 1, I_END, Index_Sequence<INDEXES ..., I>>
	{
	};
	template<size_t I_END, size_t ... INDEXES>
	struct _Make_Index_Sequence_Range<I_END, I_END, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};

	template<size_t I, size_t COUNT>
	using Make_Index_Sequence_Range = _Make_Index_Sequence_Range<I, I + COUNT, Index_Sequence<>>;
	
	template<size_t I, size_t COUNT, typename INDEXES>
	struct _Make_Index_Sequence_Reverse;
	template<size_t I, size_t ... INDEXES>
	struct _Make_Index_Sequence_Reverse<I, I, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};
	template<size_t I, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Reverse<I, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Reverse<I + 1, COUNT, Index_Sequence<I, INDEXES ...>>
	{
	};

	template<size_t COUNT>
	using Make_Index_Sequence_Reverse = _Make_Index_Sequence_Reverse<0, COUNT, Index_Sequence<>>;

	template<size_t I, size_t I_END, typename INDEXES>
	struct _Make_Index_Sequence_Range_Reverse;
	template<size_t I, size_t I_END, size_t ... INDEXES>
	struct _Make_Index_Sequence_Range_Reverse<I, I_END, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Range_Reverse<I + 1, I_END, Index_Sequence<I, INDEXES ...>>
	{
	};
	template<size_t I_END, size_t ... INDEXES>
	struct _Make_Index_Sequence_Range_Reverse<I_END, I_END, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};

	template<size_t I, size_t COUNT>
	using Make_Index_Sequence_Range_Reverse = _Make_Index_Sequence_Range_Reverse<I, I + COUNT, Index_Sequence<>>;

	template<size_t I, size_t NOT, size_t COUNT, typename INDEXES>
	struct _Make_Index_Sequence_Reverse_Not;
	template<size_t I, size_t NOT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Reverse_Not<I, NOT, I, Index_Sequence<INDEXES ...>>
	{
		typedef Index_Sequence<INDEXES ...> Type;
	};
	template<size_t I, size_t NOT, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Reverse_Not<I, NOT, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Reverse_Not<I + 1, NOT, COUNT, Index_Sequence<I, INDEXES ...>>
	{
	};
	template<size_t NOT, size_t COUNT, size_t ... INDEXES>
	struct _Make_Index_Sequence_Reverse_Not<NOT, NOT, COUNT, Index_Sequence<INDEXES ...>>
		: public _Make_Index_Sequence_Reverse_Not<NOT + 1, NOT, COUNT, Index_Sequence<INDEXES ...>>
	{
	};

	template<size_t COUNT, size_t NOT>
	using Make_Index_Sequence_Reverse_Not = _Make_Index_Sequence_Reverse_Not<0, NOT, COUNT, Index_Sequence<>>;

	template<typename F>
	struct Get_Function_Traits;
	template<typename T, typename ... TArgs>
	struct Get_Function_Traits<T(TArgs ...)>
	{
		typedef T                        Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr Bool IsMember = False;
	};
	template<typename T, typename ... TArgs>
	struct Get_Function_Traits<T(*)(TArgs ...)>
	{
		typedef T                        Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr Bool IsMember = False;
	};
	template<typename T, typename C, typename ... TArgs>
	struct Get_Function_Traits<T(C::*)(TArgs ...)>
	{
		typedef T                        Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr Bool IsMember = True;
	};

	template<typename F, size_t I>
	using Get_Function_Arg_Type = Get_Type_Sequence<I, typename Get_Function_Traits<F>::Arguments>;
}

template<AL::Bool IS_SIGNED>
struct AL::_Get_Enum_Or_Integer_Base<1, IS_SIGNED>
{
	typedef typename Conditional<IS_SIGNED, int8, uint8>::Type Type;
};
template<AL::Bool IS_SIGNED>
struct AL::_Get_Enum_Or_Integer_Base<2, IS_SIGNED>
{
	typedef typename Conditional<IS_SIGNED, int16, uint16>::Type Type;
};
template<AL::Bool IS_SIGNED>
struct AL::_Get_Enum_Or_Integer_Base<4, IS_SIGNED>
{
	typedef typename Conditional<IS_SIGNED, int32, uint32>::Type Type;
};
template<AL::Bool IS_SIGNED>
struct AL::_Get_Enum_Or_Integer_Base<8, IS_SIGNED>
{
	typedef typename Conditional<IS_SIGNED, int64, uint64>::Type Type;
};
