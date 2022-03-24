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
		static constexpr Bool Value =
			std::is_trivial<T>::value &&
			std::is_standard_layout<T>::value;
	};

	template<typename T>
	struct Is_Array
	{
		static constexpr Bool Value = False;
	};
	template<typename T, size_t S>
	struct Is_Array<T[S]>
	{
		static constexpr Bool Value = True;
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

	template<typename T>
	struct Get_Array_Size;
	template<typename T, size_t S>
	struct Get_Array_Size<T[S]>
	{
		static constexpr size_t Value = sizeof(T) * S;
	};

	template<typename T>
	struct Get_Array_Count;
	template<typename T, size_t S>
	struct Get_Array_Count<T[S]>
	{
		static constexpr size_t Value = S;
	};

	template<typename T>
	struct Get_Array_Type;
	template<typename T, size_t S>
	struct Get_Array_Type<T[S]>
	{
		typedef T Type;
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

	typedef uint32 TypeHash;

	template<typename T>
	struct Type
	{
#if defined(AL_RTTI)
		inline static const char*  Name = typeid(T).name();
#else
		inline static const char*  Name = []()
		{
			// TODO: clean this up

	#if defined(AL_COMPILER_GNU)
			return __PRETTY_FUNCTION__;
	#elif defined(AL_COMPILER_MSVC)
			return __FUNCSIG__;
	#elif defined(AL_COMPILER_CLANG)
			return __PRETTY_FUNCTION__;
	#endif
		}();
#endif

		inline static const TypeHash Hash = []()
		{
			TypeHash hash = 0x811C9DC5;

			for (auto lpName = Name; *lpName != 0; ++lpName)
			{
				hash ^= *lpName;
				hash *= 0x1000193;
			}

			return hash;
		}();
		static constexpr size_t    Size = sizeof(T);
	};

	template<typename T, typename T_BASE>
	struct _Integer;

	template<typename T>
	using Integer = _Integer<T, typename Get_Enum_Or_Integer_Base<T>::Type>;

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

	template<typename T, typename T_BASE>
	struct _Decimal;
	template<typename T>
	struct _Decimal<T, Float>
	{
		static constexpr Bool IsSigned   = Is_Signed<Float>::Value;
		static constexpr Bool IsUnsigned = Is_Unsigned<Float>::Value;

		static constexpr T Zero    = 0.0f;

		static constexpr T Minimum = FLT_MIN;
		static constexpr T Maximum = FLT_MAX;
	};
	template<typename T>
	struct _Decimal<T, Double>
	{
		static constexpr Bool IsSigned   = Is_Signed<Double>::Value;
		static constexpr Bool IsUnsigned = Is_Unsigned<Double>::Value;

		static constexpr T Zero    = 0.0;

		static constexpr T Minimum = DBL_MIN;
		static constexpr T Maximum = DBL_MAX;
	};

	template<typename T>
	using Decimal = _Decimal<T, typename Get_Decimal_Base<T>::Type>;

	template<typename T>
	struct _Integer<T, int8>
	{
		static constexpr Bool IsSigned         = True;
		static constexpr Bool IsUnsigned       = False;

		static constexpr T    Zero             = T(0x00);

		static constexpr T    Minimum          = T(-0x7F);
		static constexpr T    Maximum          = T(0x7F);

		static constexpr T    SignedCastMask   = T(0x7F);
		static constexpr T    UnsignedCastMask = T(0xFF);

		T                     Value;
	};
	template<typename T>
	struct _Integer<T, uint8>
	{
		static constexpr Bool IsSigned         = False;
		static constexpr Bool IsUnsigned       = True;

		static constexpr T    Zero             = T(0x00);

		static constexpr T    Minimum          = uint8(0x0);
		static constexpr T    Maximum          = uint8(0xFF);

		static constexpr T    SignedCastMask   = T(0x7F);
		static constexpr T    UnsignedCastMask = T(0xFF);

		T                     Value;
	};
	template<typename T>
	struct _Integer<T, int16>
	{
		static constexpr Bool IsSigned         = True;
		static constexpr Bool IsUnsigned       = False;

		static constexpr T    Zero             = T(0x0000);

		static constexpr T    Minimum          = T(-0x7FFF);
		static constexpr T    Maximum          = T(0x7FFF);

		static constexpr T    SignedCastMask   = T(0x7FFF);
		static constexpr T    UnsignedCastMask = T(0xFFFF);

		union
		{
			struct
			{
				_Integer<int8, int8>   Low;
				_Integer<uint8, uint8> High;
			};

			T Value;
		};
	};
	template<typename T>
	struct _Integer<T, uint16>
	{
		static constexpr Bool IsSigned         = False;
		static constexpr Bool IsUnsigned       = True;

		static constexpr T    Zero             = T(0x0000);

		static constexpr T    Minimum          = T(0x0);
		static constexpr T    Maximum          = T(0xFFFF);

		static constexpr T    SignedCastMask   = T(0x7FFF);
		static constexpr T    UnsignedCastMask = T(0xFFFF);

		union
		{
			struct
			{
				_Integer<uint8, uint8> Low;
				_Integer<uint8, uint8> High;
			};

			T Value;
		};
	};
	template<typename T>
	struct _Integer<T, int32>
	{
		static constexpr Bool IsSigned         = True;
		static constexpr Bool IsUnsigned       = False;

		static constexpr T    Zero             = T(0x00000000);

		static constexpr T    Minimum          = T(-0x7FFFFFFF);
		static constexpr T    Maximum          = T(0x7FFFFFFF);

		static constexpr T    SignedCastMask   = T(0x7FFFFFFF);
		static constexpr T    UnsignedCastMask = T(0xFFFFFFFF);

		union
		{
			struct
			{
				_Integer<int16, int16>   Low;
				_Integer<uint16, uint16> High;
			};

			T Value;
		};
	};
	template<typename T>
	struct _Integer<T, uint32>
	{
		static constexpr Bool IsSigned         = False;
		static constexpr Bool IsUnsigned       = True;

		static constexpr T    Zero             = T(0x00000000);

		static constexpr T    Minimum          = T(0x0);
		static constexpr T    Maximum          = T(0xFFFFFFFF);

		static constexpr T    SignedCastMask   = T(0x7FFFFFFF);
		static constexpr T    UnsignedCastMask = T(0xFFFFFFFF);

		union
		{
			struct
			{
				_Integer<uint16, uint16> Low;
				_Integer<uint16, uint16> High;
			};

			T Value;
		};
	};
	template<typename T>
	struct _Integer<T, int64>
	{
		static constexpr Bool IsSigned         = True;
		static constexpr Bool IsUnsigned       = False;

		static constexpr T    Zero             = T(0x0000000000000000);

		static constexpr T    Minimum          = T(-0x7FFFFFFFFFFFFFFF);
		static constexpr T    Maximum          = T(0x7FFFFFFFFFFFFFFF);

		static constexpr T    SignedCastMask   = T(0x7FFFFFFFFFFFFFFF);
		static constexpr T    UnsignedCastMask = T(0xFFFFFFFFFFFFFFFF);

		union
		{
			struct
			{
				_Integer<int32, int32>   Low;
				_Integer<uint32, uint32> High;
			};

			T Value;
		};
	};
	template<typename T>
	struct _Integer<T, uint64>
	{
		static constexpr Bool IsSigned         = False;
		static constexpr Bool IsUnsigned       = True;

		static constexpr T    Zero             = T(0x0000000000000000);

		static constexpr T    Minimum          = T(0x0);
		static constexpr T    Maximum          = T(0xFFFFFFFFFFFFFFFF);

		static constexpr T    SignedCastMask   = T(0x7FFFFFFFFFFFFFFF);
		static constexpr T    UnsignedCastMask = T(0xFFFFFFFFFFFFFFFF);

		union
		{
			struct
			{
				_Integer<uint32, uint32> Low;
				_Integer<uint32, uint32> High;
			};

			T Value;
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
