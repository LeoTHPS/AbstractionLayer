#pragma once
// Premature implementation is the root of all wasted time

#if defined(_DEBUG) || !defined(NDEBUG)
	#define AL_DEBUG
#endif

#if defined(_MSC_VER)
	#define AL_PLATFORM_WINDOWS

	#if defined(_M_IX86)
		#define AL_PLATFORM_WIN32
	#elif defined(_M_X64)
		#define AL_PLATFORM_WIN64
	#endif
#elif defined(__GNUC__)
	#define AL_PLATFORM_LINUX

	#if defined(__i386__)
		#define AL_PLATFORM_LINUX32
	#elif defined(__x86_64__)
		#define AL_PLATFORM_LINUX64
	#elif defined(__arm__)
		#define AL_PLATFORM_LINUX_ARM
	#elif defined(__aarch64__)
		#define AL_PLATFORM_LINUX_ARM64
	#endif
#else
	#error Could not detect platform
#endif

#if defined(AL_PLATFORM_WIN32) || defined(AL_PLATFORM_LINUX32)
	#define AL_X86
#elif defined(AL_PLATFORM_WIN64) || defined(AL_PLATFORM_LINUX64)
	#define AL_X86_64
#elif defined(AL_PLATFORM_LINUX_ARM)
	#define AL_ARM
#elif defined(AL_PLATFORM_LINUX_ARM64)
	#define AL_ARM64
#endif

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_X86)
		#define AL_CDECL       __attribute__((cdecl))
		#define AL_FASTCALL    __attribute__((fastcall))
	#endif

	#define AL_STDCALL         __attribute__((stdcall))
	#define AL_THISCALL        __attribute__((thiscall))
#elif defined(AL_PLATFORM_WINDOWS)
	#if defined(AL_X86)
		#define AL_CDECL       __cdecl
		#define AL_FASTCALL    __fastcall
	#endif

	#define AL_STDCALL         __stdcall
	#define AL_THISCALL        __thiscall
	#define AL_VECTORCALL      __vectorcall

	#if defined(__cplusplus_cli)
		#define AL_CLRCALL     __clrcall
	#endif
#endif

#define AL_DEPRECATED(__text__) [[deprecated(__text__)]]

#define AL_STRINGIFY(__value__)  _AL_STRINGIFY(__value__)
#define _AL_STRINGIFY(__value__) #__value__

#define AL_CONCAT(__value1__, __value2__)  _AL_CONCAT(__value1__, __value2__)
#define _AL_CONCAT(__value1__, __value2__) __value1__##__value2__

#if defined(AL_DEBUG)
	#if defined(AL_PLATFORM_LINUX)
		#define AL_ASSERT(__condition__, __message__) _GLIBCXX_DEBUG_ASSERT((__condition__))
	#elif defined(AL_PLATFORM_WINDOWS)
		#define AL_ASSERT(__condition__, __message__) _ASSERT_EXPR((__condition__), L##__message__)
	#endif
#else
	#define AL_ASSERT(__condition__, __message__)
#endif

#if defined(AL_PLATFORM_LINUX)
	#define AL_NAKED      __attribute__((naked))

	#define AL_INLINE     __attribute__((always_inline))
	#define AL_NO_INLINE  __attribute__((noinline))

	#define AL_DLL_EXPORT 
	#define AL_DLL_IMPORT 
#elif defined(AL_PLATFORM_WINDOWS)
	#define AL_NAKED      __declspec(naked)

	#define AL_INLINE     __forceinline
	#define AL_NO_INLINE  __declspec(noinline)

	#define AL_DLL_EXPORT __declspec(dllexport)
	#define AL_DLL_IMPORT __declspec(dllimport)
#endif

#define AL_DEFINE_ENUM_FLAG_OPERATORS(ENUM_TYPE) \
	constexpr ENUM_TYPE operator | (ENUM_TYPE a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return static_cast<ENUM_TYPE>(static_cast<ENUM_BASE>(a) | static_cast<ENUM_BASE>(b)); \
	} \
	constexpr ENUM_TYPE& operator |= (ENUM_TYPE& a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return reinterpret_cast<ENUM_TYPE&>(reinterpret_cast<ENUM_BASE&>(a) |= static_cast<ENUM_BASE>(b)); \
	} \
	\
	constexpr ENUM_TYPE operator & (ENUM_TYPE a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return static_cast<ENUM_TYPE>(static_cast<ENUM_BASE>(a) & static_cast<ENUM_BASE>(b)); \
	} \
	constexpr ENUM_TYPE& operator &= (ENUM_TYPE& a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return reinterpret_cast<ENUM_TYPE&>(reinterpret_cast<ENUM_BASE&>(a) &= static_cast<ENUM_BASE>(b)); \
	} \
	\
	constexpr ENUM_TYPE operator ~ (ENUM_TYPE a) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return static_cast<ENUM_TYPE>(~static_cast<ENUM_BASE>(a)); \
	} \
	constexpr ENUM_TYPE operator ^ (ENUM_TYPE a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return static_cast<ENUM_TYPE>(static_cast<ENUM_BASE>(a) ^ static_cast<ENUM_BASE>(b)); \
	} \
	constexpr ENUM_TYPE& operator ^= (ENUM_TYPE& a, ENUM_TYPE b) \
	{ \
		typedef AL::Get_Enum_Or_Integer_Base<ENUM_TYPE>::Type ENUM_BASE; \
		return reinterpret_cast<ENUM_TYPE&>(reinterpret_cast<ENUM_BASE&>(a) ^= static_cast<ENUM_BASE>(b)); \
	}

#include <cstdint>
#include <cstring>

#include <typeinfo>
#include <type_traits>

namespace AL
{
	typedef std::int8_t   int8;
	typedef std::int16_t  int16;
	typedef std::int32_t  int32;
	typedef std::int64_t  int64;

	typedef std::uint8_t  uint8;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

#if defined(AL_X86) || defined(AL_ARM)
	typedef uint32		  size_t;
#elif defined(AL_X86_64) || defined(AL_ARM64)
	typedef uint64		  size_t;
#endif

	enum class Platforms : uint8
	{
		x86        = 0x1,
		x86_64     = 0x2,

		ARM        = 0x4,
		ARM64      = 0x8,

		Linux      = 0x10,
		Windows    = 0x20,

		Win32      = Windows | x86,
		Win64      = Windows | x86_64,

		Linux32    = Linux | x86,
		Linux64    = Linux | x86_64,
		LinuxARM   = Linux | ARM,
		LinuxARM64 = Linux | ARM64,

#if defined(AL_PLATFORM_WIN32)
		Machine = Win32
#elif defined(AL_PLATFORM_WIN64)
		Machine = Win64
#elif defined(AL_PLATFORM_LINUX32)
		Machine = Linux32
#elif defined(AL_PLATFORM_LINUX64)
		Machine = Linux64
#elif defined(AL_PLATFORM_LINUX_ARM)
		Machine = LinuxARM
#elif defined(AL_PLATFORM_LINUX_ARM64)
		Machine = LinuxARM64
#endif
	};

	template<typename ...>
	struct Type_Sequence;

	template<size_t, typename ...>
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

	template<size_t ...>
	struct Index_Sequence
	{
	};

	template<size_t S, size_t ... I>
	struct Make_Index_Sequence
		: public Make_Index_Sequence<S - 1, S - 1, I ...>
	{
	};
	template<size_t ... I>
	struct Make_Index_Sequence<0, I ...>
	{
		typedef Index_Sequence<I ...> Type;
	};

	template<size_t S, size_t ... I>
	struct Make_Index_Sequence_Reverse
		: public Make_Index_Sequence_Reverse<S - 1, I ..., S - 1>
	{
	};
	template<size_t ... I>
	struct Make_Index_Sequence_Reverse<0, I ...>
	{
		typedef Index_Sequence<I ...> Type;
	};

	template<size_t BEGIN, size_t END, size_t ... I>
	struct Make_Index_Sequence_Range
		: public Make_Index_Sequence_Range<BEGIN, END - 1, END, I ...>
	{
	};
	template<size_t BEGIN, size_t ... I>
	struct Make_Index_Sequence_Range<BEGIN, BEGIN, I ...>
	{
		typedef Index_Sequence<BEGIN, I ...> Type;
	};

	template<size_t BEGIN, size_t END, size_t ... I>
	struct Make_Index_Sequence_Range_Reverse
		: public Make_Index_Sequence_Range_Reverse<BEGIN, END - 1, I ..., END>
	{
	};
	template<size_t BEGIN, size_t ... I>
	struct Make_Index_Sequence_Range_Reverse<BEGIN, BEGIN, I ...>
	{
		typedef Index_Sequence<I ..., BEGIN> Type;
	};

	template<bool CONDITION, typename CONDITION_1, typename CONDITION_2>
	struct Conditional
	{
		typedef CONDITION_1 Type;
	};
	template<typename CONDITION_1, typename CONDITION_2>
	struct Conditional<false, CONDITION_1, CONDITION_2>
	{
		typedef CONDITION_2 Type;
	};

	template<bool CONDITION, typename T, T VALUE_1, T VALUE_2>
	struct Conditional_Value
	{
		static constexpr T Value = VALUE_1;
	};
	template<typename T, T VALUE_1, T VALUE_2>
	struct Conditional_Value<false, T, VALUE_1, VALUE_2>
	{
		static constexpr T Value = VALUE_2;
	};

	template<bool CONDITION, typename T>
	struct Inherit_If
	{
		struct Empty
		{
		};

		typedef typename Conditional<CONDITION, T, Empty>::Type Type;
	};

	template<bool CONDITION, typename T, typename ELSE>
	struct Inherit_If_Else
	{
		typedef typename Conditional<CONDITION, T, ELSE>::Type Type;
	};

	template<bool CONDITION, typename T>
	struct _Enable_If
	{
		typedef T Type;
	};
	template<typename T>
	struct _Enable_If<false, T>
	{
	};

	template<bool CONDITION, typename T = void>
	using Enable_If = _Enable_If<CONDITION, T>;

	template<typename T, typename TYPE>
	struct Is_Type
	{
		static constexpr bool Value = std::is_same<T, TYPE>::value;
	};

	template<typename T>
	struct Is_POD
	{
		static constexpr bool Value = std::is_pod<T>::value;
	};

	template<typename T>
	struct Is_Enum
	{
		static constexpr bool Value = std::is_enum<T>::value;
	};

	template<typename T>
	using Is_Bool = Is_Type<T, bool>;

	template<typename T>
	using Is_Float = Is_Type<T, float>;

	template<typename T>
	using Is_Double = Is_Type<T, double>;

	template<typename T>
	struct Is_Integer
	{
		static constexpr bool Value = std::is_integral<T>::value;
	};

	template<typename T>
	struct Is_Enum_Or_Integer
	{
		static constexpr bool Value = Is_Enum<T>::Value || Is_Integer<T>::Value;
	};

	template<typename T>
	struct Is_Signed
	{
		static constexpr bool Value = std::is_signed<T>::value;
	};

	template<typename T>
	struct Is_Unsigned
	{
		static constexpr bool Value = std::is_unsigned<T>::value;
	};

	template<typename T>
	struct Is_Const
	{
		static constexpr bool Value = std::is_const<T>::value;
	};

	template<typename T>
	struct Is_Pointer
	{
		static constexpr bool Value = std::is_pointer<T>::value;
	};

	template<typename T>
	struct Is_Reference
	{
		static constexpr bool Value = std::is_reference<T>::value;
	};

	template<typename BASE, typename T>
	struct Is_Base_Of
	{
		static constexpr bool Value = std::is_base_of<BASE, T>::value;
	};

	template<typename TYPE, typename TYPE2>
	struct Is_Convertable
	{
		static constexpr bool Value = std::is_convertible<TYPE, TYPE2>::value;
	};

	template<typename T>
	struct Is_Copy_Constructible
	{
		static constexpr bool Value = std::is_copy_constructible<T>::value;
	};

	template<typename T>
	struct Is_Copy_Assignable
	{
		static constexpr bool Value = std::is_copy_assignable<T>::value;
	};
	
	template<typename T>
	struct Is_Move_Constructible
	{
		static constexpr bool Value = std::is_move_constructible<T>::value;
	};

	template<typename T>
	struct Is_Move_Assignable
	{
		static constexpr bool Value = std::is_move_assignable<T>::value;
	};

	template<typename T>
	struct Get_Type_Name
	{
		inline static const char* Value = typeid(T).name();
	};

	typedef uint32 TypeHash;

	template<typename T>
	class Get_Type_Hash
	{
		static constexpr TypeHash GetTypeHash()
		{
			return GetStringHash(
				__FUNCTION__
			);
		}

		static constexpr TypeHash GetStringHash(const char* lpString, TypeHash hash = 0x811C9DC5)
		{
			return *lpString ? GetStringHash(&lpString[1], (hash ^ *lpString) * 0x1000193llu) : hash;
		}

	public:
		static constexpr TypeHash Value = GetTypeHash();
	};

	template<typename T, size_t SIZE = sizeof(T)>
	struct Get_Integer_Base;
	template<typename T>
	struct Get_Integer_Base<T, 1>
	{
		typedef typename Conditional<Is_Signed<T>::Value, int8, uint8>::Type Type;
	};
	template<typename T>
	struct Get_Integer_Base<T, 2>
	{
		typedef typename Conditional<Is_Signed<T>::Value, int16, uint16>::Type Type;
	};
	template<typename T>
	struct Get_Integer_Base<T, 4>
	{
		typedef typename Conditional<Is_Signed<T>::Value, int32, uint32>::Type Type;
	};
	template<typename T>
	struct Get_Integer_Base<T, 8>
	{
		typedef typename Conditional<Is_Signed<T>::Value, int64, uint64>::Type Type;
	};

	template<typename T>
	using Get_Enum_Base = Get_Integer_Base<T>;

	template<typename T, size_t SIZE = sizeof(T)>
	struct Get_Decimal_Base;
	template<typename T>
	struct Get_Decimal_Base<T, 4>
	{
		typedef float Type;
	};
	template<typename T>
	struct Get_Decimal_Base<T, 8>
	{
		typedef double Type;
	};

	template<typename T>
	using Get_Enum_Or_Integer_Base = Conditional<Is_Enum<T>::Value, typename Get_Enum_Base<T>::Type, typename Get_Integer_Base<T>::Type>;

	template<typename T, bool IS_ENUM_OR_INTEGER = Is_Enum_Or_Integer<T>::Value>
	struct Get_Enum_Or_Integer_Or_Decimal_Base;
	template<typename T>
	struct Get_Enum_Or_Integer_Or_Decimal_Base<T, true>
	{
		typedef typename Get_Enum_Or_Integer_Base<T>::Type Type;
	};
	template<typename T>
	struct Get_Enum_Or_Integer_Or_Decimal_Base<T, false>
	{
		typedef typename Get_Decimal_Base<T>::Type Type;
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
	constexpr void Swap(T& source, T& destination)
	{
		std::swap(
			source,
			destination
		);
	}

	template<typename T, size_t S>
	constexpr void Swap(T(&source)[S], T(&destination)[S])
	{
		std::swap(
			source,
			destination
		);
	}

	template<typename T>
	constexpr typename Remove_Reference<T>::Type&& Move(T&& value)
	{
		return static_cast<typename Remove_Reference<T>::Type&&>(
			value
		);
	}
	
	template<typename T>
	constexpr T&& Forward(typename Remove_Reference<T>::Type& value)
	{
		return static_cast<T&&>(
			value
		);
	}
	template<typename T>
	constexpr T&& Forward(typename Remove_Reference<T>::Type&& value)
	{
		return static_cast<T&&>(
			value
		);
	}

	inline void memset(void* lpDest, uint8 value, size_t size)
	{
		::memset(
			lpDest,
			value,
			size
		);
	}

	template<typename T, size_t S>
	inline void memcpy(T(&dest)[S], const T(&source)[S])
	{
		::memcpy(
			&dest[0],
			&source[0],
			S * sizeof(T)
		);
	}
	inline void memcpy(void* lpDest, const void* lpSource, size_t size)
	{
		::memcpy(
			lpDest,
			lpSource,
			size
		);
	}

	template<typename T, size_t S>
	inline void memmove(T(&dest)[S], const T(&source)[S])
	{
		::memmove(
			&dest[0],
			&source[0],
			sizeof(T) * S
		);
	}
	inline void memmove(void* lpDest, const void* lpSource, size_t size)
	{
		::memmove(
			lpDest,
			lpSource,
			size
		);
	}

	template<typename T, size_t S>
	inline bool memcmp(const T(&array)[S], const T(&array2)[S])
	{
		return ::memcmp(
			&array[0],
			&array2[0],
			sizeof(T) * S
		) == 0;
	}
	inline bool memcmp(const void* lpValue, const void* lpValue2, size_t size)
	{
		return ::memcmp(
			lpValue,
			lpValue2,
			size
		) == 0;
	}

	AL_DEFINE_ENUM_FLAG_OPERATORS(Platforms);
}

#include "Collections/List.hpp"
#include "Collections/Array.hpp"
#include "Collections/Tuple.hpp"
#include "Collections/String.hpp"

#include <regex>

#include <cmath>
#include <cfloat>

#if defined(AL_PLATFORM_WINDOWS)
	#define VC_EXTRALEAN

	#define WIN32_LEAN_AND_MEAN

	#include <winapifamily.h>

	#if !defined(_WIN32_WINNT)
		#define _WIN32_WINNT_NT4          0x0400 // Windows NT 4.0
		#define _WIN32_WINNT_WIN2K        0x0500 // Windows 2000
		#define _WIN32_WINNT_WINXP        0x0501 // Windows XP
		#define _WIN32_WINNT_WS03         0x0502 // Windows Server 2003
		#define _WIN32_WINNT_WIN6         0x0600 // Windows Vista
		#define _WIN32_WINNT_VISTA        0x0600 // Windows Vista
		#define _WIN32_WINNT_WS08         0x0600 // Windows Server 2008
		#define _WIN32_WINNT_LONGHORN     0x0600 // Windows Vista
		#define _WIN32_WINNT_WIN7         0x0601 // Windows 7
		#define _WIN32_WINNT_WIN8         0x0602 // Windows 8
		#define _WIN32_WINNT_WINBLUE      0x0603 // Windows 8.1
		#define _WIN32_WINNT_WINTHRESHOLD 0x0A00 // Windows 10
		#define _WIN32_WINNT_WIN10        0x0A00 // Windows 10

		#define WINVER			_WIN32_WINNT_WIN7
		#define _WIN32_WINNT	_WIN32_WINNT_WIN7

		#define WINAPI_FAMILY	WINAPI_FAMILY_DESKTOP_APP
	#endif

	#include <intrin.h>
	#include <Windows.h>
	#include <winnt.h>
	#include <winternl.h>

	#undef GetObject
	#undef GetMessage
	#undef LoadLibrary
	#undef CreateProcess
#elif defined(AL_PLATFORM_LINUX)
	#include <unistd.h>
#endif

namespace AL
{
	typedef Collections::String String;
	typedef Collections::WString WString;

	template<size_t S>
	struct __Get_GUID_Container_Type;
	template<>
	struct __Get_GUID_Container_Type<1>
	{
		typedef uint8 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<2>
	{
		typedef uint16 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<3>
	{
		typedef uint32 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<4>
	{
		typedef uint32 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<5>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<6>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<7>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<8>
	{
		typedef uint64 Type;
	};

	template<size_t SIZE, typename ... T_FIELDS>
	struct __Get_GUID_Container_Size;
	template<size_t SIZE>
	struct __Get_GUID_Container_Size<SIZE>
	{
		static constexpr size_t Value = SIZE;
	};
	template<size_t SIZE, typename T_FIELD, typename ... T_FIELDS>
	struct __Get_GUID_Container_Size<SIZE, T_FIELD, T_FIELDS ...>
		: public __Get_GUID_Container_Size<SIZE + sizeof(T_FIELD), T_FIELDS ...>
	{
	};
	
	template<typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Container_Size = __Get_GUID_Container_Size<sizeof(T_COUNTER), T_FIELDS ...>;

	template<typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Container_Type = __Get_GUID_Container_Type<_Get_GUID_Container_Size<T_COUNTER, T_FIELDS ...>::Value>;

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Mask
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;
		typedef typename Get_Type_Sequence<I, T_FIELDS ..., T_COUNTER>::Type     TypeField;
		typedef typename Get_Enum_Or_Integer_Base<TypeField>::Type               TypeFieldBase;

		static constexpr Type Value = TypeFieldBase(~0);
	};

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Bit_Count
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;
		typedef typename Get_Type_Sequence<I, T_FIELDS ..., T_COUNTER>::Type     TypeField;

		static constexpr Type Value = sizeof(TypeField) * 8;
	};

	template<size_t I, size_t I_END, size_t BIT_COUNT, typename T_COUNTER, typename ... T_FIELDS>
	struct __Get_GUID_Fields_Bit_Count
		: public __Get_GUID_Fields_Bit_Count<I + 1, I_END, BIT_COUNT + _Get_GUID_Field_Bit_Count<I, T_COUNTER, T_FIELDS ...>::Value, T_COUNTER, T_FIELDS ...>
	{
	};
	template<size_t I_END, size_t BIT_COUNT, typename T_COUNTER, typename ... T_FIELDS>
	struct __Get_GUID_Fields_Bit_Count<I_END, I_END, BIT_COUNT, T_COUNTER, T_FIELDS ...>
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = static_cast<Type>(
			BIT_COUNT + _Get_GUID_Field_Bit_Count<I_END, T_COUNTER, T_FIELDS ...>::Value
		);
	};

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Fields_Bit_Count = __Get_GUID_Fields_Bit_Count<0, I, 0, T_COUNTER, T_FIELDS ...>;

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Shift_Count
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = _Get_GUID_Fields_Bit_Count<sizeof ...(T_FIELDS), T_COUNTER, T_FIELDS ...>::Value - _Get_GUID_Fields_Bit_Count<I, T_COUNTER, T_FIELDS ...>::Value;
	};
	template<typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Shift_Count<0, T_COUNTER, T_FIELDS ...>
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = _Get_GUID_Fields_Bit_Count<sizeof ...(T_FIELDS), T_COUNTER, T_FIELDS ...>::Value - _Get_GUID_Field_Bit_Count<0, T_COUNTER, T_FIELDS ...>::Value;
	};
	
	template<typename T_COUNTER, typename ... T_FIELDS>
	class GUID
	{
		typedef _Get_GUID_Container_Size<T_COUNTER, T_FIELDS ...> Get_GUID_Container_Size;
		typedef _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...> Get_GUID_Container_Type;

		template<size_t I>
		using Get_GUID_Field_Mask = _Get_GUID_Field_Mask<I, T_COUNTER, T_FIELDS ...>;
		
		template<size_t I>
		using Get_GUID_Field_Shift_Count = _Get_GUID_Field_Shift_Count<I, T_COUNTER, T_FIELDS ...>;

		Collections::Tuple<T_FIELDS ...> fields;
		T_COUNTER                        counter;

		static constexpr size_t FIELD_INDEX_COUNTER = sizeof ...(T_FIELDS);

		template<size_t ... INDEXES>
		GUID(typename Get_GUID_Container_Type::Type value, Index_Sequence<INDEXES ...>)
			: fields(
				GetFieldFromContainer<INDEXES>(value) ...
			),
			counter(
				(value >> Get_GUID_Field_Shift_Count<FIELD_INDEX_COUNTER>::Value) & Get_GUID_Field_Mask<FIELD_INDEX_COUNTER>::Value
			)
		{
		}

	public:
		typedef Type_Sequence<T_FIELDS ...>            Fields;
		typedef T_COUNTER                              Counter;

		typedef typename Get_GUID_Container_Type::Type Container;

		GUID()
			: GUID(
				0
			)
		{
		}

		GUID(Container value)
			: GUID(
				value,
				typename Make_Index_Sequence<sizeof ...(T_FIELDS)>::Type{}
			)
		{
		}

		GUID(Counter counter, T_FIELDS ... fields)
			: fields(
				Forward<T_FIELDS>(fields) ...
			),
			counter(
				counter
			)
		{
		}

		virtual ~GUID()
		{
		}

		template<size_t I>
		auto GetField() const
		{
			return fields.template Get<I>();
		}

		static constexpr size_t GetFieldCount()
		{
			return sizeof ...(T_FIELDS);
		}

		auto GetCounter() const
		{
			return counter;
		}

		Container GetValue() const
		{
			auto fields = GetFieldsAsContainer(
				typename Make_Index_Sequence<GetFieldCount()>::Type{}
			);

			auto counter = static_cast<Container>(
				GetCounter()
			);

			return fields | (counter << Get_GUID_Field_Shift_Count<FIELD_INDEX_COUNTER>::Value);
		}

		template<size_t I>
		void SetField(typename Get_Type_Sequence<I, Fields>::Type value)
		{
			fields.Set<I>(
				value
			);
		}

		operator Container () const
		{
			return GetValue();
		}

		bool operator == (const GUID& id) const
		{
			if (GetCounter() != id.GetCounter())
			{

				return false;
			}

			typedef typename Make_Index_Sequence<GetFieldCount()>::Type Field_Count_Index_Sequence;

			if (GetFieldsAsContainer(Field_Count_Index_Sequence{}) != id.GetFieldsAsContainer(Field_Count_Index_Sequence{}))
			{

				return false;
			}

			return true;
		}
		bool operator != (const GUID& id) const
		{
			return !operator==(
				id
			);
		}

	private:
		template<size_t ... INDEXES>
		constexpr Container GetFieldsAsContainer(Index_Sequence<INDEXES ...>) const
		{
			return ((static_cast<Container>(GetField<INDEXES>()) << Get_GUID_Field_Shift_Count<INDEXES>::Value) | ...);
		}

		template<size_t I, typename T_FIELD = typename Get_Type_Sequence<I, T_FIELDS ...>::Type>
		static constexpr T_FIELD GetFieldFromContainer(Container value)
		{
			return static_cast<T_FIELD>(
				(value >> Get_GUID_Field_Shift_Count<I>::Value) & Get_GUID_Field_Mask<I>::Value
			);
		}
	};

	template<typename T>
	class _Regex
	{
		typedef std::basic_regex<typename T::Char> Regex;

		typedef std::match_results<const typename T::Char*> MatchResults;

		Regex regex;

	public:
		typedef T String;
		typedef typename String::Char Char;

		typedef Collections::Array<String> MatchCollection;

		_Regex(const String& pattern)
			: regex(
				pattern.GetCString(),
				pattern.GetLength()
			)
		{
		}

		template<size_t SIZE>
		_Regex(const Char(&pattern)[SIZE])
			: regex(
				&pattern[0],
				SIZE - 1
			)
		{
		}

		virtual ~_Regex()
		{
		}

		bool IsMatch(const String& buffer) const
		{
			MatchResults matches;

			return std::regex_match(
				buffer.GetCString(),
				matches,
				regex
			);
		}

		static bool IsMatch(const String& pattern, const String& buffer)
		{
			return _Regex(pattern).IsMatch(buffer);
		}

		bool Match(MatchCollection& match, const String& buffer) const
		{
			MatchResults matches;

			if (std::regex_search(
				buffer.GetCString(),
				matches,
				regex
			))
			{
				match.SetSize(
					matches.size()
				);

				size_t i = 0;

				for (auto it = matches.begin(); it != matches.end(); ++it, ++i)
				{
					match[i] = String(
						it->str().c_str(),
						it->str().length()
					);
				}

				return true;
			}

			return false;
		}

		static bool Match(MatchCollection& match, const String& pattern, const String& buffer)
		{
			return _Regex(pattern).Match(
				match,
				buffer
			);
		}
		template<size_t SIZE>
		static bool Match(MatchCollection& match, const Char(&pattern)[SIZE], const String& buffer)
		{
			return _Regex(pattern).Match(
				match,
				buffer
			);
		}

		auto& operator = (const String& pattern)
		{
			regex = Regex(
				pattern.GetCString(),
				pattern.GetLength()
			);

			return *this;
		}

		bool operator == (const _Regex& regex) const
		{
			return this->regex == regex.regex;
		}
		bool operator != (const _Regex& regex) const
		{
			return !operator==(regex);
		}
	};

	typedef _Regex<String> Regex;
	typedef _Regex<WString> RegexW;

	template<char VALUE>
	struct Char_To_WChar
	{
		static constexpr wchar_t Value = L'\0' + static_cast<uint8>(VALUE);
	};

	template<typename T, typename T_BASE>
	union _Integer;
	template<typename T>
	union _Integer<T, int8>
	{
		static constexpr bool IsSigned = true;
		static constexpr bool IsUnsigned = false;

		static constexpr T Minimum = T(-0x7F);
		static constexpr T Maximum = T(0x7F);

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%i",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%i",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, uint8>
	{
		static constexpr bool IsSigned = false;
		static constexpr bool IsUnsigned = true;

		static constexpr T Minimum = T(0x0);
		static constexpr T Maximum = T(0xFF);

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%u",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%u",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, int16>
	{
		static constexpr bool IsSigned = true;
		static constexpr bool IsUnsigned = false;

		static constexpr T Minimum = T(-0x7FFF);
		static constexpr T Maximum = T(0x7FFF);

		struct
		{
			_Integer<int8, int8> Low;
			_Integer<int8, int8> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%i",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%i",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, uint16>
	{
		static constexpr bool IsSigned = false;
		static constexpr bool IsUnsigned = true;

		static constexpr T Minimum = T(0x0);
		static constexpr T Maximum = T(0xFFFF);

		struct
		{
			_Integer<uint8, uint8> Low;
			_Integer<uint8, uint8> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%u",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%u",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, int32>
	{
		static constexpr bool IsSigned = true;
		static constexpr bool IsUnsigned = false;

		static constexpr T Minimum = T(-0x7FFFFFFF);
		static constexpr T Maximum = T(0x7FFFFFFF);

		struct
		{
			_Integer<int16, int16> Low;
			_Integer<int16, int16> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%i",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%i",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstol(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, uint32>
	{
		static constexpr bool IsSigned = false;
		static constexpr bool IsUnsigned = true;

		static constexpr T Minimum = T(0x0);
		static constexpr T Maximum = T(0xFFFFFFFF);

		struct
		{
			_Integer<uint16, uint16> Low;
			_Integer<uint16, uint16> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%u",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%u",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstoul(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, int64>
	{
		static constexpr bool IsSigned = true;
		static constexpr bool IsUnsigned = false;

		static constexpr T Minimum = T(-0x7FFFFFFFFFFFFFFF);
		static constexpr T Maximum = (0x7FFFFFFFFFFFFFFF);

		struct
		{
			_Integer<int32, int32> Low;
			_Integer<int32, int32> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%lli",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%lli",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtoll(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromString(const WString& value)
		{
			return static_cast<T>(
				std::wcstoll(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};
	template<typename T>
	union _Integer<T, uint64>
	{
		static constexpr bool IsSigned = false;
		static constexpr bool IsUnsigned = true;

		static constexpr T Minimum = T(0x0);
		static constexpr T Maximum = T(0xFFFFFFFFFFFFFFFF);

		struct
		{
			_Integer<uint32, uint32> Low;
			_Integer<uint32, uint32> High;
		};

		T Value;

		static String ToString(T value)
		{
			return String::Format(
				"%llu",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%llu",
				value
			);
		}

		static auto FromString(const String& value)
		{
			return static_cast<T>(
				std::strtoull(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}

		static auto FromString(const WString& value)
		{
			return static_cast<T>(
				std::wcstoull(
					value.GetCString(),
					nullptr,
					0
				)
			);
		}
	};

	template<typename T>
	using Integer = _Integer<T, typename Get_Enum_Or_Integer_Base<T>::Type>;

	template<typename C, size_t ... DIGITS>
	struct _Integer_To_String_To_Chars;
	template<size_t ... DIGITS>
	struct _Integer_To_String_To_Chars<char, DIGITS ...>
	{
		static constexpr char Value[] = { '0' + DIGITS ..., 0 };
	};
	template<size_t ... DIGITS>
	struct _Integer_To_String_To_Chars<wchar_t, DIGITS ...>
	{
		static constexpr char Value[] = { L'0' + DIGITS ..., 0 };
	};

	template<typename C, size_t VALUE, size_t ... DIGITS>
	struct _Integer_To_String_Explode
		: public _Integer_To_String_Explode<C, VALUE / 10, VALUE % 10, DIGITS ...>
	{
	};
	template<typename C, size_t ... DIGITS>
	struct _Integer_To_String_Explode<C, 0, DIGITS ...>
		: public _Integer_To_String_To_Chars<C, DIGITS ...>
	{
	};

	template<size_t VALUE>
	struct Integer_To_String
		: public _Integer_To_String_Explode<char, VALUE>
	{
	};

	template<size_t VALUE>
	struct Integer_To_WString
		: public _Integer_To_String_Explode<wchar_t, VALUE>
	{
	};

	template<typename T>
	struct Float
	{
		static_assert(
			Is_Float<T>::Value,
			"T must be float"
		);

		static constexpr bool IsSigned = Is_Signed<T>::Value;
		static constexpr bool IsUnsigned = Is_Unsigned<T>::Value;

		static constexpr T Minimum = FLT_MIN;
		static constexpr T Maximum = FLT_MAX;

		static String ToString(T value)
		{
			return String::Format(
				"%f",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%f",
				value
			);
		}

		static T FromString(const String& value)
		{
			return static_cast<T>(
				std::strtof(
					value.GetCString(),
					nullptr
				)
			);
		}

		static T FromWString(const WString& value)
		{
			return static_cast<T>(
				std::wcstof(
					value.GetCString(),
					nullptr
				)
			);
		}
	};

	template<typename T>
	struct Double
	{
		static_assert(
			Is_Double<T>::Value,
			"T must be double"
		);

		static constexpr bool IsSigned = Is_Signed<T>::Value;
		static constexpr bool IsUnsigned = Is_Unsigned<T>::Value;

		static constexpr T Minimum = DBL_MIN;
		static constexpr T Maximum = DBL_MAX;

		static String ToString(T value)
		{
			return String::Format(
				"%f",
				value
			);
		}

		static WString ToWString(T value)
		{
			return WString::Format(
				L"%f",
				value
			);
		}

		static T FromString(const String& value)
		{
			return static_cast<T>(
				std::strtod(
					value.GetCString(),
					nullptr
				)
			);
		}

		static T FromString(const WString& value)
		{
			return static_cast<T>(
				std::wcstod(
					value.GetCString(),
					nullptr
				)
			);
		}
	};

	template<typename T>
	struct Bool
	{
		static_assert(
			Is_Bool<T>::Value,
			"T must be bool"
		);

		static String ToString(T value)
		{
			return value ? String("true") : String("false");
		}

		static WString ToWString(T value)
		{
			return value ? WString(L"true") : WString(L"false");
		}

		static T FromString(const String& value)
		{
			return value.ToLower() == "true";
		}

		static T FromWString(const WString& value)
		{
			return value.ToLower() == L"true";
		}
	};

	template<typename T, typename BASE = typename Get_Enum_Or_Integer_Base<T>::Type>
	constexpr T IndexToFlag(T index)
	{
		return static_cast<T>(
			static_cast<BASE>(1) << static_cast<BASE>(index)
		);
	}

	template<typename T, T VALUE, typename BASE = typename Get_Enum_Or_Integer_Base<T>::Type>
	struct Index_To_Flag
	{
		static constexpr T Value = static_cast<T>(
			static_cast<BASE>(1) << static_cast<BASE>(VALUE)
		);
	};

	template<typename T, typename BASE = typename Get_Enum_Or_Integer_Base<T>::Type>
	constexpr T FlagToIndex(T flag, BASE index = 0)
	{
		return (static_cast<BASE>(flag) != 1) ? FlagToIndex(static_cast<BASE>(flag) >> 1, ++index) : index;
	}

	template<typename T, T VALUE, T INDEX>
	struct _Flag_To_Index;
	template<uint8 VALUE, uint8 INDEX>
	struct _Flag_To_Index<uint8, VALUE, INDEX>
		: public _Flag_To_Index<uint8, static_cast<uint8>(VALUE >> 1), static_cast<uint8>(INDEX + 1)>
	{
	};
	template<uint8 INDEX>
	struct _Flag_To_Index<uint8, 1, INDEX>
	{
		static constexpr uint8 Value = INDEX;
	};
	template<uint16 VALUE, uint16 INDEX>
	struct _Flag_To_Index<uint16, VALUE, INDEX>
		: public _Flag_To_Index<uint16, static_cast<uint16>(VALUE >> 1), static_cast<uint16>(INDEX + 1)>
	{
	};
	template<uint16 INDEX>
	struct _Flag_To_Index<uint16, 1, INDEX>
	{
		static constexpr uint16 Value = INDEX;
	};
	template<uint32 VALUE, uint32 INDEX>
	struct _Flag_To_Index<uint32, VALUE, INDEX>
		: public _Flag_To_Index<uint32, static_cast<uint32>(VALUE >> 1), static_cast<uint32>(INDEX + 1)>
	{
	};
	template<uint32 INDEX>
	struct _Flag_To_Index<uint32, 1, INDEX>
	{
		static constexpr uint32 Value = INDEX;
	};
	template<uint64 VALUE, uint64 INDEX>
	struct _Flag_To_Index<uint64, VALUE, INDEX>
		: public _Flag_To_Index<uint64, static_cast<uint64>(VALUE >> 1), static_cast<uint64>(INDEX + 1)>
	{
	};
	template<uint64 INDEX>
	struct _Flag_To_Index<uint64, 1, INDEX>
	{
		static constexpr uint64 Value = INDEX;
	};

	template<typename T, T VALUE, typename BASE = typename Get_Enum_Or_Integer_Base<T>::Type>
	struct Flag_To_Index
	{
		static constexpr T Value = static_cast<T>(
			_Flag_To_Index<BASE, static_cast<BASE>(VALUE), 0>::Value
		);
	};

	template<typename T, typename BASE = typename Get_Enum_Or_Integer_Base<T>::Type>
	struct BitMask
	{
		union
		{
			T Mask;
			BASE Value;
		};

		BitMask()
			: Value(
				0
			)
		{
		}
		
		BitMask(BitMask&& mask)
			: Mask(
				mask.Mask
			)
		{
			mask.Value = 0;
		}
		
		BitMask(const BitMask& mask)
			: Mask(
				mask.Mask
			)
		{
		}

		BitMask(T value)
			: Mask(
				value
			)
		{
		}

		bool IsSet(T flag) const
		{
			return static_cast<BASE>(flag) ? ((Value & static_cast<BASE>(flag)) == static_cast<BASE>(flag)) : true;
		}

		template<T FLAG>
		static constexpr bool IsSet(T mask)
		{
			return static_cast<BASE>(FLAG) ? ((static_cast<BASE>(mask) & static_cast<BASE>(FLAG)) == static_cast<BASE>(FLAG)) : true;
		}

		void Add(T flag)
		{
			Value |= static_cast<BASE>(flag);
		}

		template<T FLAG>
		static constexpr T Add(T mask)
		{
			return static_cast<T>(
				static_cast<BASE>(mask) | static_cast<BASE>(FLAG)
			);
		}

		void Remove(T flag)
		{
			Value &= ~static_cast<BASE>(flag);
		}

		template<T FLAG>
		static constexpr T Remove(T mask)
		{
			return static_cast<T>(
				static_cast<BASE>(mask) & ~static_cast<BASE>(FLAG)
			);
		}

		void Set(T flag, bool set)
		{
			set ? Add(flag) : Remove(flag);
		}

		template<T FLAG>
		static constexpr T Set(T mask, bool set)
		{
			return set ? Add<FLAG>(mask) : Remove<FLAG>(mask);
		}

		operator bool() const
		{
			return Value != 0;
		}

		bool operator [] (T value) const
		{
			return IsSet(
				value
			);
		}

		auto& operator =(T value)
		{
			Mask = value;

			return *this;
		}

		auto& operator =(BitMask&& mask)
		{
			Value = mask.Value;
			mask.Value = 0;

			return *this;
		}
		auto& operator =(const BitMask& mask)
		{
			Value = mask.Value;

			return *this;
		}
	};

	class Math final
	{
		Math() = delete;

	public:
		static constexpr double PI = 3.14159265358979323846;

		static constexpr double Rad2Deg = 360 / (PI * 2);

		template<typename T>
		static constexpr bool IsPowerOf2(T value)
		{
			return value && !(value & (value - static_cast<T>(1)));
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

		template<typename T>
		static constexpr T Clamp(T value, T min, T max)
		{
			if (value < min)
				value = min;

			else if (value > max)
				value = max;

			return value;
		}

		template<typename T>
		static constexpr T Round(T value, T low, T high)
		{
			return ((value - low) < (high - value)) ? low : high;
		}
	};

	enum class Endians : uint32
	{
		Big,
		Little,

		Machine = ((0xFFFFFFFF & 0x00000001) == 0x00000001) ? Little : Big
	};

	class BitConverter final
	{
		template<typename T, size_t BIT_COUNT = sizeof(T) * 8, bool IS_ENUM_OR_INTEGER = Is_Enum_Or_Integer<T>::Value>
		struct Flip_Bytes;
		template<typename T, bool IS_ENUM_OR_INTEGER>
		struct Flip_Bytes<T, 8, IS_ENUM_OR_INTEGER>
		{
			static constexpr T Flip(T value)
			{
				return value;
			}
		};
		template<typename T>
		struct Flip_Bytes<T, 16, true>
		{
			static constexpr T Flip(T value)
			{
				typedef typename Get_Enum_Or_Integer_Base<T>::Type Base;

				return static_cast<T>(
					((static_cast<Base>(value) & 0x00FF) << 8) |
					((static_cast<Base>(value) & 0xFF00) >> 8)
				);
			}
		};
		template<typename T>
		struct Flip_Bytes<T, 16, false>
		{
			static constexpr T Flip(T value)
			{
				return static_cast<T>(
					((static_cast<uint16>(value) & 0x00FF) << 8) |
					((static_cast<uint16>(value) & 0xFF00) >> 8)
				);
			}
		};
		template<typename T>
		struct Flip_Bytes<T, 32, true>
		{
			static constexpr T Flip(T value)
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
		struct Flip_Bytes<T, 32, false>
		{
			static constexpr T Flip(T value)
			{
				return static_cast<T>(
					((static_cast<uint32>(value) & 0xFF000000) >> 24) |
					((static_cast<uint32>(value) & 0x00FF0000) >> 8) |
					((static_cast<uint32>(value) & 0x0000FF00) << 8) |
					((static_cast<uint32>(value) & 0x000000FF) << 24)
				);
			}
		};
		template<typename T>
		struct Flip_Bytes<T, 64, true>
		{
			static constexpr T Flip(T value)
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
		template<typename T>
		struct Flip_Bytes<T, 64, false>
		{
			static constexpr T Flip(T value)
			{
				return static_cast<T>(
					((static_cast<uint64>(value) & 0xFF00000000000000) >> 56) |
					((static_cast<uint64>(value) & 0x00FF000000000000) >> 40) |
					((static_cast<uint64>(value) & 0x0000FF0000000000) >> 24) |
					((static_cast<uint64>(value) & 0x000000FF00000000) >> 8) |
					((static_cast<uint64>(value) & 0x00000000FF000000) << 8) |
					((static_cast<uint64>(value) & 0x0000000000FF0000) << 24) |
					((static_cast<uint64>(value) & 0x000000000000FF00) << 40) |
					((static_cast<uint64>(value) & 0x00000000000000FF) << 56)
				);
			}
		};

		BitConverter() = delete;

	public:
		static constexpr bool IsBigEndian()
		{
			return Endians::Machine == Endians::Big;
		}

		static constexpr bool IsLittleEndian()
		{
			return Endians::Machine == Endians::Little;
		}

		template<typename T>
		static constexpr T Flip(T value)
		{
			static_assert(
				Is_Enum<T>::Value || Is_Integer<T>::Value || Is_Float<T>::Value || Is_Double<T>::Value,
				"T must be Enum, Integer, Float or Double"
			);

			return Flip_Bytes<T>::Flip(
				value
			);
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

		template<typename T>
		static T FromFloat(float value, bool roundUp = true)
		{
			float fraction;

			return FromFloat<T>(
				value,
				fraction,
				roundUp
			);
		}

		template<typename T>
		static T FromFloat(float value, float& fraction, bool roundUp = true)
		{
			float baseValue;

			fraction = modff(
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
		static T FromDouble(double value, bool roundUp = true)
		{
			double fraction;

			return FromDouble<T>(
				value,
				fraction,
				roundUp
			);
		}

		template<typename T>
		static T FromDouble(double value, double& fraction, bool roundUp = true)
		{
			double baseValue;
			fraction = modf(value, &baseValue);

			if (roundUp && Math::Round(fraction, 0.0, 1.0))
				return static_cast<T>(baseValue + 1);

			return static_cast<T>(baseValue);
		}
	};
	
	typedef Collections::Array<uint8> BaseConverterBuffer;

	template<size_t BASE>
	class _BaseConverter_Converter;
	template<>
	class _BaseConverter_Converter<16>
	{
		static constexpr char HEX_TABLE[16] =
		{
			'0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9',

			'A', 'B', 'C', 'D', 'E', 'F'
		};

		static constexpr uint8 HEX_TABLE_REVERSE[0xFF] =
		{
			/* 0x00 */ 0,
			/* 0x01 */ 0,
			/* 0x02 */ 0,
			/* 0x03 */ 0,
			/* 0x04 */ 0,
			/* 0x05 */ 0,
			/* 0x06 */ 0,
			/* 0x07 */ 0,
			/* 0x08 */ 0,
			/* 0x09 */ 0,
			/* 0x0A */ 0,
			/* 0x0B */ 0,
			/* 0x0C */ 0,
			/* 0x0D */ 0,
			/* 0x0E */ 0,
			/* 0x0F */ 0,
			/* 0x10 */ 0,
			/* 0x11 */ 0,
			/* 0x12 */ 0,
			/* 0x13 */ 0,
			/* 0x14 */ 0,
			/* 0x15 */ 0,
			/* 0x16 */ 0,
			/* 0x17 */ 0,
			/* 0x18 */ 0,
			/* 0x19 */ 0,
			/* 0x1A */ 0,
			/* 0x1B */ 0,
			/* 0x1C */ 0,
			/* 0x1D */ 0,
			/* 0x1E */ 0,
			/* 0x1F */ 0,
			/* 0x20 */ 0,
			/* 0x21 */ 0,
			/* 0x22 */ 0,
			/* 0x23 */ 0,
			/* 0x24 */ 0,
			/* 0x25 */ 0,
			/* 0x26 */ 0,
			/* 0x27 */ 0,
			/* 0x28 */ 0,
			/* 0x29 */ 0,
			/* 0x2A */ 0,
			/* 0x2B */ 0,
			/* 0x2C */ 0,
			/* 0x2D */ 0,
			/* 0x2E */ 0,
			/* 0x2F */ 0,
			/* 0x30 */ 0x30, // 0
			/* 0x31 */ 0x31, // 1
			/* 0x32 */ 0x32, // 2
			/* 0x33 */ 0x33, // 3
			/* 0x34 */ 0x34, // 4
			/* 0x35 */ 0x35, // 5
			/* 0x36 */ 0x36, // 6
			/* 0x37 */ 0x37, // 7
			/* 0x38 */ 0x38, // 8
			/* 0x39 */ 0x39, // 9
			/* 0x3A */ 0,
			/* 0x3B */ 0,
			/* 0x3C */ 0,
			/* 0x3D */ 0,
			/* 0x3E */ 0,
			/* 0x3F */ 0,
			/* 0x40 */ 0,
			/* 0x41 */ 0x41, // A
			/* 0x42 */ 0x42, // B
			/* 0x43 */ 0x43, // C
			/* 0x44 */ 0x44, // D
			/* 0x45 */ 0x45, // E
			/* 0x46 */ 0x46, // F
			/* 0x47 */ 0,
			/* 0x48 */ 0,
			/* 0x49 */ 0,
			/* 0x4A */ 0,
			/* 0x4B */ 0,
			/* 0x4C */ 0,
			/* 0x4D */ 0,
			/* 0x4E */ 0,
			/* 0x4F */ 0,
			/* 0x50 */ 0,
			/* 0x51 */ 0,
			/* 0x52 */ 0,
			/* 0x53 */ 0,
			/* 0x54 */ 0,
			/* 0x55 */ 0,
			/* 0x56 */ 0,
			/* 0x57 */ 0,
			/* 0x58 */ 0,
			/* 0x59 */ 0,
			/* 0x5A */ 0,
			/* 0x5B */ 0,
			/* 0x5C */ 0,
			/* 0x5D */ 0,
			/* 0x5E */ 0,
			/* 0x5F */ 0,
			/* 0x60 */ 0,
			/* 0x61 */ 0,
			/* 0x62 */ 0,
			/* 0x63 */ 0,
			/* 0x64 */ 0,
			/* 0x65 */ 0,
			/* 0x66 */ 0,
			/* 0x67 */ 0,
			/* 0x68 */ 0,
			/* 0x69 */ 0,
			/* 0x6A */ 0,
			/* 0x6B */ 0,
			/* 0x6C */ 0,
			/* 0x6D */ 0,
			/* 0x6E */ 0,
			/* 0x6F */ 0,
			/* 0x70 */ 0,
			/* 0x71 */ 0,
			/* 0x72 */ 0,
			/* 0x73 */ 0,
			/* 0x74 */ 0,
			/* 0x75 */ 0,
			/* 0x76 */ 0,
			/* 0x77 */ 0,
			/* 0x78 */ 0,
			/* 0x79 */ 0,
			/* 0x7A */ 0,
			/* 0x7B */ 0,
			/* 0x7C */ 0,
			/* 0x7D */ 0,
			/* 0x7E */ 0,
			/* 0x7F */ 0,
			/* 0x80 */ 0,
			/* 0x81 */ 0,
			/* 0x82 */ 0,
			/* 0x83 */ 0,
			/* 0x84 */ 0,
			/* 0x85 */ 0,
			/* 0x86 */ 0,
			/* 0x87 */ 0,
			/* 0x88 */ 0,
			/* 0x89 */ 0,
			/* 0x8A */ 0,
			/* 0x8B */ 0,
			/* 0x8C */ 0,
			/* 0x8D */ 0,
			/* 0x8E */ 0,
			/* 0x8F */ 0,
			/* 0x90 */ 0,
			/* 0x91 */ 0,
			/* 0x92 */ 0,
			/* 0x93 */ 0,
			/* 0x94 */ 0,
			/* 0x95 */ 0,
			/* 0x96 */ 0,
			/* 0x97 */ 0,
			/* 0x98 */ 0,
			/* 0x99 */ 0,
			/* 0x9A */ 0,
			/* 0x9B */ 0,
			/* 0x9C */ 0,
			/* 0x9D */ 0,
			/* 0x9E */ 0,
			/* 0x9F */ 0,
			/* 0xA0 */ 0,
			/* 0xA1 */ 0,
			/* 0xA2 */ 0,
			/* 0xA3 */ 0,
			/* 0xA4 */ 0,
			/* 0xA5 */ 0,
			/* 0xA6 */ 0,
			/* 0xA7 */ 0,
			/* 0xA8 */ 0,
			/* 0xA9 */ 0,
			/* 0xAA */ 0,
			/* 0xAB */ 0,
			/* 0xAC */ 0,
			/* 0xAD */ 0,
			/* 0xAE */ 0,
			/* 0xAF */ 0,
			/* 0xB0 */ 0,
			/* 0xB1 */ 0,
			/* 0xB2 */ 0,
			/* 0xB3 */ 0,
			/* 0xB4 */ 0,
			/* 0xB5 */ 0,
			/* 0xB6 */ 0,
			/* 0xB7 */ 0,
			/* 0xB8 */ 0,
			/* 0xB9 */ 0,
			/* 0xBA */ 0,
			/* 0xBB */ 0,
			/* 0xBC */ 0,
			/* 0xBD */ 0,
			/* 0xBE */ 0,
			/* 0xBF */ 0,
			/* 0xC0 */ 0,
			/* 0xC1 */ 0,
			/* 0xC2 */ 0,
			/* 0xC3 */ 0,
			/* 0xC4 */ 0,
			/* 0xC5 */ 0,
			/* 0xC6 */ 0,
			/* 0xC7 */ 0,
			/* 0xC8 */ 0,
			/* 0xC9 */ 0,
			/* 0xCA */ 0,
			/* 0xCB */ 0,
			/* 0xCC */ 0,
			/* 0xCD */ 0,
			/* 0xCE */ 0,
			/* 0xCF */ 0,
			/* 0xD0 */ 0,
			/* 0xD1 */ 0,
			/* 0xD2 */ 0,
			/* 0xD3 */ 0,
			/* 0xD4 */ 0,
			/* 0xD5 */ 0,
			/* 0xD6 */ 0,
			/* 0xD7 */ 0,
			/* 0xD8 */ 0,
			/* 0xD9 */ 0,
			/* 0xDA */ 0,
			/* 0xDB */ 0,
			/* 0xDC */ 0,
			/* 0xDD */ 0,
			/* 0xDE */ 0,
			/* 0xDF */ 0,
			/* 0xE0 */ 0,
			/* 0xE1 */ 0,
			/* 0xE2 */ 0,
			/* 0xE3 */ 0,
			/* 0xE4 */ 0,
			/* 0xE5 */ 0,
			/* 0xE6 */ 0,
			/* 0xE7 */ 0,
			/* 0xE8 */ 0,
			/* 0xE9 */ 0,
			/* 0xEA */ 0,
			/* 0xEB */ 0,
			/* 0xEC */ 0,
			/* 0xED */ 0,
			/* 0xEE */ 0,
			/* 0xEF */ 0,
			/* 0xF0 */ 0,
			/* 0xF1 */ 0,
			/* 0xF2 */ 0,
			/* 0xF3 */ 0,
			/* 0xF4 */ 0,
			/* 0xF5 */ 0,
			/* 0xF6 */ 0,
			/* 0xF7 */ 0,
			/* 0xF8 */ 0,
			/* 0xF9 */ 0,
			/* 0xFA */ 0,
			/* 0xFB */ 0,
			/* 0xFC */ 0,
			/* 0xFD */ 0,
			/* 0xFE */ 0
		};

		static constexpr bool HEX_TABLE_VALID_BYTES[0xFF] =
		{
			/* 0x00 */ false,
			/* 0x01 */ false,
			/* 0x02 */ false,
			/* 0x03 */ false,
			/* 0x04 */ false,
			/* 0x05 */ false,
			/* 0x06 */ false,
			/* 0x07 */ false,
			/* 0x08 */ false,
			/* 0x09 */ false,
			/* 0x0A */ false,
			/* 0x0B */ false,
			/* 0x0C */ false,
			/* 0x0D */ false,
			/* 0x0E */ false,
			/* 0x0F */ false,
			/* 0x10 */ false,
			/* 0x11 */ false,
			/* 0x12 */ false,
			/* 0x13 */ false,
			/* 0x14 */ false,
			/* 0x15 */ false,
			/* 0x16 */ false,
			/* 0x17 */ false,
			/* 0x18 */ false,
			/* 0x19 */ false,
			/* 0x1A */ false,
			/* 0x1B */ false,
			/* 0x1C */ false,
			/* 0x1D */ false,
			/* 0x1E */ false,
			/* 0x1F */ false,
			/* 0x20 */ false,
			/* 0x21 */ false,
			/* 0x22 */ false,
			/* 0x23 */ false,
			/* 0x24 */ false,
			/* 0x25 */ false,
			/* 0x26 */ false,
			/* 0x27 */ false,
			/* 0x28 */ false,
			/* 0x29 */ false,
			/* 0x2A */ false,
			/* 0x2B */ false,
			/* 0x2C */ false,
			/* 0x2D */ false,
			/* 0x2E */ false,
			/* 0x2F */ false,
			/* 0x30 */ true, // 0
			/* 0x31 */ true, // 1
			/* 0x32 */ true, // 2
			/* 0x33 */ true, // 3
			/* 0x34 */ true, // 4
			/* 0x35 */ true, // 5
			/* 0x36 */ true, // 6
			/* 0x37 */ true, // 7
			/* 0x38 */ true, // 8
			/* 0x39 */ true, // 9
			/* 0x3A */ false,
			/* 0x3B */ false,
			/* 0x3C */ false,
			/* 0x3D */ false,
			/* 0x3E */ false,
			/* 0x3F */ false,
			/* 0x40 */ false,
			/* 0x41 */ true, // A
			/* 0x42 */ true, // B
			/* 0x43 */ true, // C
			/* 0x44 */ true, // D
			/* 0x45 */ true, // E
			/* 0x46 */ true, // F
			/* 0x47 */ false,
			/* 0x48 */ false,
			/* 0x49 */ false,
			/* 0x4A */ false,
			/* 0x4B */ false,
			/* 0x4C */ false,
			/* 0x4D */ false,
			/* 0x4E */ false,
			/* 0x4F */ false,
			/* 0x50 */ false,
			/* 0x51 */ false,
			/* 0x52 */ false,
			/* 0x53 */ false,
			/* 0x54 */ false,
			/* 0x55 */ false,
			/* 0x56 */ false,
			/* 0x57 */ false,
			/* 0x58 */ false,
			/* 0x59 */ false,
			/* 0x5A */ false,
			/* 0x5B */ false,
			/* 0x5C */ false,
			/* 0x5D */ false,
			/* 0x5E */ false,
			/* 0x5F */ false,
			/* 0x60 */ false,
			/* 0x61 */ false,
			/* 0x62 */ false,
			/* 0x63 */ false,
			/* 0x64 */ false,
			/* 0x65 */ false,
			/* 0x66 */ false,
			/* 0x67 */ false,
			/* 0x68 */ false,
			/* 0x69 */ false,
			/* 0x6A */ false,
			/* 0x6B */ false,
			/* 0x6C */ false,
			/* 0x6D */ false,
			/* 0x6E */ false,
			/* 0x6F */ false,
			/* 0x70 */ false,
			/* 0x71 */ false,
			/* 0x72 */ false,
			/* 0x73 */ false,
			/* 0x74 */ false,
			/* 0x75 */ false,
			/* 0x76 */ false,
			/* 0x77 */ false,
			/* 0x78 */ false,
			/* 0x79 */ false,
			/* 0x7A */ false,
			/* 0x7B */ false,
			/* 0x7C */ false,
			/* 0x7D */ false,
			/* 0x7E */ false,
			/* 0x7F */ false,
			/* 0x80 */ false,
			/* 0x81 */ false,
			/* 0x82 */ false,
			/* 0x83 */ false,
			/* 0x84 */ false,
			/* 0x85 */ false,
			/* 0x86 */ false,
			/* 0x87 */ false,
			/* 0x88 */ false,
			/* 0x89 */ false,
			/* 0x8A */ false,
			/* 0x8B */ false,
			/* 0x8C */ false,
			/* 0x8D */ false,
			/* 0x8E */ false,
			/* 0x8F */ false,
			/* 0x90 */ false,
			/* 0x91 */ false,
			/* 0x92 */ false,
			/* 0x93 */ false,
			/* 0x94 */ false,
			/* 0x95 */ false,
			/* 0x96 */ false,
			/* 0x97 */ false,
			/* 0x98 */ false,
			/* 0x99 */ false,
			/* 0x9A */ false,
			/* 0x9B */ false,
			/* 0x9C */ false,
			/* 0x9D */ false,
			/* 0x9E */ false,
			/* 0x9F */ false,
			/* 0xA0 */ false,
			/* 0xA1 */ false,
			/* 0xA2 */ false,
			/* 0xA3 */ false,
			/* 0xA4 */ false,
			/* 0xA5 */ false,
			/* 0xA6 */ false,
			/* 0xA7 */ false,
			/* 0xA8 */ false,
			/* 0xA9 */ false,
			/* 0xAA */ false,
			/* 0xAB */ false,
			/* 0xAC */ false,
			/* 0xAD */ false,
			/* 0xAE */ false,
			/* 0xAF */ false,
			/* 0xB0 */ false,
			/* 0xB1 */ false,
			/* 0xB2 */ false,
			/* 0xB3 */ false,
			/* 0xB4 */ false,
			/* 0xB5 */ false,
			/* 0xB6 */ false,
			/* 0xB7 */ false,
			/* 0xB8 */ false,
			/* 0xB9 */ false,
			/* 0xBA */ false,
			/* 0xBB */ false,
			/* 0xBC */ false,
			/* 0xBD */ false,
			/* 0xBE */ false,
			/* 0xBF */ false,
			/* 0xC0 */ false,
			/* 0xC1 */ false,
			/* 0xC2 */ false,
			/* 0xC3 */ false,
			/* 0xC4 */ false,
			/* 0xC5 */ false,
			/* 0xC6 */ false,
			/* 0xC7 */ false,
			/* 0xC8 */ false,
			/* 0xC9 */ false,
			/* 0xCA */ false,
			/* 0xCB */ false,
			/* 0xCC */ false,
			/* 0xCD */ false,
			/* 0xCE */ false,
			/* 0xCF */ false,
			/* 0xD0 */ false,
			/* 0xD1 */ false,
			/* 0xD2 */ false,
			/* 0xD3 */ false,
			/* 0xD4 */ false,
			/* 0xD5 */ false,
			/* 0xD6 */ false,
			/* 0xD7 */ false,
			/* 0xD8 */ false,
			/* 0xD9 */ false,
			/* 0xDA */ false,
			/* 0xDB */ false,
			/* 0xDC */ false,
			/* 0xDD */ false,
			/* 0xDE */ false,
			/* 0xDF */ false,
			/* 0xE0 */ false,
			/* 0xE1 */ false,
			/* 0xE2 */ false,
			/* 0xE3 */ false,
			/* 0xE4 */ false,
			/* 0xE5 */ false,
			/* 0xE6 */ false,
			/* 0xE7 */ false,
			/* 0xE8 */ false,
			/* 0xE9 */ false,
			/* 0xEA */ false,
			/* 0xEB */ false,
			/* 0xEC */ false,
			/* 0xED */ false,
			/* 0xEE */ false,
			/* 0xEF */ false,
			/* 0xF0 */ false,
			/* 0xF1 */ false,
			/* 0xF2 */ false,
			/* 0xF3 */ false,
			/* 0xF4 */ false,
			/* 0xF5 */ false,
			/* 0xF6 */ false,
			/* 0xF7 */ false,
			/* 0xF8 */ false,
			/* 0xF9 */ false,
			/* 0xFA */ false,
			/* 0xFB */ false,
			/* 0xFC */ false,
			/* 0xFD */ false,
			/* 0xFE */ false
		};

	public:
		template<typename T>
		static String Encode(const T& value)
		{
			return Encode(
				&value,
				sizeof(T)
			);
		}
		static String Encode(const void* lpBuffer, size_t size)
		{
			String value;

			char buffer[3] = { 0 };

			auto BUFFER_BEGIN = static_cast<const uint8*>(
				lpBuffer
			);

			auto BUFFER_END = BUFFER_BEGIN + size;

			for (auto _lpBuffer = BUFFER_BEGIN; _lpBuffer != BUFFER_END; _lpBuffer++)
			{
				buffer[0] = EncodeH(*_lpBuffer);
				buffer[1] = EncodeL(*_lpBuffer);

				value.Append(
					buffer
				);
			}

			return value;
		}

		template<typename T>
		static auto Decode(const String& string)
		{
			BaseConverterBuffer buffer;
			Decode(buffer, string);

			return *reinterpret_cast<T*>(&buffer[0]);
		}
		static bool Decode(BaseConverterBuffer& buffer, const String& string)
		{
			auto length = string.GetLength();

			if ((length % 2) != 0)
			{

				return false;
			}

			buffer.SetSize(
				length / 2
			);

			for (size_t i = 0; i < length; )
			{
				auto c1 = string[i++];
				auto c2 = string[i++];

				if (!HEX_TABLE_VALID_BYTES[static_cast<uint8>(c1)] ||
					!HEX_TABLE_VALID_BYTES[static_cast<uint8>(c2)])
				{

					return false;
				}

				buffer[i] = Decode(
					c1,
					c2
				);
			}

			return true;
		}

	private:
		static constexpr char EncodeH(uint8 value)
		{
			return HEX_TABLE[(value >> 4) & 0xF];
		}
		static constexpr char EncodeL(uint8 value)
		{
			return HEX_TABLE[value & 0xF];
		}

		static constexpr uint8 Decode(char high, char low)
		{
			return
				(HEX_TABLE_REVERSE[static_cast<size_t>(low)] & 0xF) |
				((HEX_TABLE_REVERSE[static_cast<size_t>(high)] << 4) & 0xF);
		}
	};
	template<>
	class _BaseConverter_Converter<32>
	{
	public:
		template<typename T>
		static String Encode(const T& value);
		static String Encode(const void* lpBuffer, size_t size);

		template<typename T>
		static auto Decode(const String& string);
		static bool Decode(BaseConverterBuffer& buffer, const String& string);
	};
	template<>
	class _BaseConverter_Converter<64>
	{
	public:
		template<typename T>
		static String Encode(const T& value);
		static String Encode(const void* lpBuffer, size_t size);

		template<typename T>
		static auto Decode(const String& string);
		static bool Decode(BaseConverterBuffer& buffer, const String& string);
	};

	class BaseConverter final
	{
		BaseConverter() = delete;

	public:
		typedef BaseConverterBuffer Buffer;

		template<typename T>
		static String ToBase16(const T& value)
		{
			return _BaseConverter_Converter<16>::Encode(
				value
			);
		}
		static String ToBase16(const void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<16>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static auto FromBase16(const String& string)
		{
			return _BaseConverter_Converter<16>::Decode<T>(
				string
			);
		}
		static bool FromBase16(Buffer& buffer, const String& string)
		{
			return _BaseConverter_Converter<16>::Decode(
				buffer,
				string
			);
		}

		template<typename T>
		static String ToBase32(const T& value)
		{
			return _BaseConverter_Converter<32>::Encode(
				value
			);
		}
		static String ToBase32(const void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<32>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static auto FromBase32(const String& string)
		{
			return _BaseConverter_Converter<32>::Decode<T>(
				string
			);
		}
		static bool FromBase32(Buffer& buffer, const String& string)
		{
			return _BaseConverter_Converter<32>::Decode(
				buffer,
				string
			);
		}

		template<typename T>
		static String ToBase64(const T& value)
		{
			return _BaseConverter_Converter<64>::Encode(
				value
			);
		}
		static String ToBase64(const void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<64>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static auto FromBase64(const String& string)
		{
			return _BaseConverter_Converter<64>::Decode<T>(
				string
			);
		}
		static bool FromBase64(Buffer& buffer, const String& string)
		{
			return _BaseConverter_Converter<64>::Decode(
				buffer,
				string
			);
		}
	};

	class HexConverter final
	{
		HexConverter() = delete;

	public:
		typedef typename BaseConverter::Buffer Buffer;

		template<typename T>
		static String Encode(const T& value)
		{
			return BaseConverter::ToBase16(
				value
			);
		}
		static String Encode(const void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase16(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static auto Decode(const String& string)
		{
			return BaseConverter::FromBase16<T>(
				string
			);
		}
		static bool Decode(Buffer& buffer, const String& string)
		{
			return BaseConverter::FromBase16(
				buffer,
				string
			);
		}
	};

	template<typename T>
	struct Safe_Static_Cast
	{
		template<typename TYPE>
		static constexpr TYPE Cast(T value)
		{
			if constexpr (Is_Float<T>::Value)
			{
				return BitConverter::FromFloat<TYPE>(
					value
				);
			}
			else if constexpr (Is_Double<T>::Value)
			{
				return BitConverter::FromDouble<TYPE>(
					value
				);
			}
			else
			{
				return static_cast<TYPE>(
					value
				);
			}
		}
	};

	template<typename F>
	struct Get_Function_Traits;
	template<typename T, typename ... TArgs>
	struct Get_Function_Traits<T(TArgs ...)>
	{
		typedef T Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr bool IsMember = false;
	};
	template<typename T, typename ... TArgs>
	struct Get_Function_Traits<T(*)(TArgs ...)>
	{
		typedef T Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr bool IsMember = false;
	};
	template<typename T, typename C, typename ... TArgs>
	struct Get_Function_Traits<T(C::*)(TArgs ...)>
	{
		typedef T Return;
		typedef Type_Sequence<TArgs ...> Arguments;

		static constexpr bool IsMember = true;
	};

	template<typename F, size_t ARG_INDEX>
	using Get_Function_Arg_Type = Get_Type_Sequence<ARG_INDEX, typename Get_Function_Traits<F>::Arguments>;

	template<typename F>
	class Function;

	template<typename T, typename ... TArgs>
	class Function<T(TArgs ...)>
	{
		class ILambda
		{
		public:
			template<typename F>
			static ILambda* Create(F&& function)
			{
				return new Lambda<F>(
					Move(function)
				);
			}

			static void Destroy(ILambda* lpLambda)
			{
				delete lpLambda;
			}

			virtual ~ILambda()
			{
			}

			virtual uint32 GetHash() const = 0;
			
			virtual ILambda* Copy() const = 0;

			virtual T Execute(TArgs ... args) const = 0;
		};

		template<typename F>
		class Lambda
			: public ILambda
		{
			F function;

			Lambda(Lambda&&) = delete;
			Lambda(const Lambda&) = delete;

		public:
			explicit Lambda(F&& function)
				: function(
					Move(function)
				)
			{
			}

			explicit Lambda(const F& function)
				: function(
					function
				)
			{
			}

			virtual uint32 GetHash() const override
			{
				return Get_Type_Hash<F>::Value;
			}

			virtual ILambda* Copy() const override
			{
				return new Lambda<F>(
					function
				);
			}

			virtual T Execute(TArgs ... args) const override
			{
				return function(
					Forward<TArgs>(args) ...
				);
			}
		};

		ILambda* lpLambda;

	public:
		Function()
			: lpLambda(
				nullptr
			)
		{
		}

		Function(Function&& function)
			: lpLambda(
				function.lpLambda
			)
		{
			function.lpLambda = nullptr;
		}

		Function(const Function& function)
			: lpLambda(
				function.lpLambda ? function.lpLambda->Copy() : nullptr
			)
		{
		}

		template<typename F>
		Function(F&& function)
			: lpLambda(
				ILambda::Create(
					Move(function)
				)
			)
		{
		}

		Function(T(*lpFunction)(TArgs ...))
			: lpLambda(
				ILambda::Create([lpFunction](TArgs ... _args)
					{
						return lpFunction(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}

		template<typename C>
		Function(T(C::*lpFunction)(TArgs ...), C& instance)
			: lpLambda(
				ILambda::Create([lpInstance = &instance, lpFunction](TArgs ... _args)
					{
						return (lpInstance->*lpFunction)(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}

		template<typename C>
		Function(T(C::*lpFunction)(TArgs ...) const, const C& instance)
			: lpLambda(
				ILambda::Create([lpInstance = &instance, lpFunction](TArgs ... _args)
					{
						return (lpInstance->*lpFunction)(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}

		virtual ~Function()
		{
			if (lpLambda)
			{
				ILambda::Destroy(lpLambda);
			}
		}

		auto GetHash() const
		{
			return lpLambda ? lpLambda->GetHash() : Get_Type_Hash<T(TArgs ...)>::Value;
		}

		template<typename F>
		void Bind(F&& function)
		{
			if (lpLambda)
			{

				ILambda::Destroy(lpLambda);
			}

			lpLambda = ILambda::Create(
				Move(function)
			);
		}

		void Bind(T(*lpFunction)(TArgs ...))
		{
			Bind([lpFunction](TArgs ... _args)
				{
					return lpFunction(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}

		template<typename C>
		void Bind(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			Bind([lpInstance = &instance, lpFunction](TArgs ... _args)
				{
					return (lpInstance->*lpFunction)(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}

		template<typename C>
		void Bind(T(C::*lpFunction)(TArgs ...) const, const C& instance)
		{
			Bind([lpInstance = &instance, lpFunction](TArgs ... _args)
				{
					return (lpInstance->*lpFunction)(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}

		void Unbind()
		{
			if (lpLambda)
			{
				delete lpLambda;

				lpLambda = nullptr;
			}
		}

		operator bool() const
		{
			return lpLambda != nullptr;
		}

		T operator () (TArgs ... args) const
		{
			return lpLambda->Execute(
				Forward<TArgs>(args) ...
			);
		}

		auto& operator = (Function&& function)
		{
			if (lpLambda)
			{

				ILambda::Destroy(lpLambda);
			}

			lpLambda = function.lpLambda;
			function.lpLambda = nullptr;

			return *this;
		}

		auto& operator = (const Function& function)
		{
			if (lpLambda)
			{

				ILambda::Destroy(lpLambda);
			}

			lpLambda = function.lpLambda ? function.lpLambda->Copy() : nullptr;

			return *this;
		}

		template<typename F>
		auto& operator = (F&& function)
		{
			Bind(
				Move(function)
			);

			return *this;
		}

		auto& operator = (T(*lpFunction)(TArgs ...))
		{
			Bind(
				lpFunction
			);

			return *this;
		}

		bool operator == (const Function& function) const
		{
			if (lpLambda && function.lpLambda)
			{

				return GetHash() == function.GetHash();
			}

			return !lpLambda && !function.lpLambda;
		}
		bool operator != (const Function& function) const
		{
			return !operator==(function);
		}
	};

	template<typename F>
	class Event;

	template<typename F>
	using EventCallback = Function<F>;

	template<typename T, typename ... TArgs>
	class Event<T(TArgs ...)>
	{
		Collections::List<EventCallback<T(TArgs ...)>> callbacks;
		Collections::List<const EventCallback<T(TArgs ...)>*> callbackRefs;

	public:
		typedef EventCallback<T(TArgs ...)> Callback;

		virtual ~Event()
		{
		}

		auto GetSize() const
		{
			return callbackRefs.GetSize();
		}

		void Register(Callback&& callback)
		{
			callbacks.PushBack(
				Move(callback)
			);

			callbackRefs.PushBack(
				callbacks.GetBack()
			);
		}
		void Register(const Callback& callback)
		{
			callbackRefs.PushBack(
				&callback
			);
		}

		void Unregister(const Callback& callback)
		{
			callbackRefs.Remove(
				&callback
			);
		}

		void Clear()
		{
			callbacks.Clear();
			callbackRefs.Clear();
		}

		T Execute(TArgs ... args) const
		{
			if constexpr (Is_Type<T, void>::Value)
			{
				for (auto lpCallback : callbackRefs)
				{
					(*lpCallback)(
						Forward<TArgs>(args) ...
					);
				}
			}
			else
			{
				auto result = T();

				for (auto lpCallback : callbackRefs)
				{
					if (!(result = (*lpCallback)(Forward<TArgs>(args) ...)))
					{

						return result;
					}
				}

				return result;
			}
		}
	};

	template<typename T, typename ... TArgs>
	class Event<EventCallback<T(TArgs ...)>>
		: public Event<T(TArgs ...)>
	{
	public:
		using Event<T(TArgs ...)>::Event;
	};

	class TimeSpan
	{
		uint64 value;

	public:
		static constexpr uint64 Infinite = 0x3E7FFFFFC18;

		static TimeSpan FromMicroseconds(uint64 value)
		{
			return TimeSpan(
				value
			);
		}

		static TimeSpan FromMilliseconds(uint64 value)
		{
			return TimeSpan(
				value * 1000
			);
		}

		static TimeSpan FromSeconds(uint64 value)
		{
			return TimeSpan(
				value * 1000000
			);
		}

		static TimeSpan FromMinutes(uint64 value)
		{
			return TimeSpan(
				value * 60000000
			);
		}

		static TimeSpan FromHours(uint64 value)
		{
			return TimeSpan(
				value * 3600000000
			);
		}

		static TimeSpan FromDays(uint64 value)
		{
			return TimeSpan(
				value * 86400000000
			);
		}

		TimeSpan()
			: TimeSpan(
				0
			)
		{
		}

		TimeSpan(uint64 microseconds)
			: value(
				microseconds
			)
		{
		}

		virtual ~TimeSpan()
		{
		}

		auto ToMicroseconds() const
		{
			return value;
		}

		auto ToMilliseconds() const
		{
			return value / 1000;
		}

		auto ToSeconds() const
		{
			return value / 1000000;
		}

		auto ToMinutes() const
		{
			return value / 60000000;
		}

		auto ToHours() const
		{
			return value / 3600000000;
		}

		auto ToDays() const
		{
			return value / 86400000000;
		}

		auto& operator += (TimeSpan time)
		{
			value += time.ToMicroseconds();

			return *this;
		}
		auto& operator -= (TimeSpan time)
		{
			auto ms = time.ToMicroseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}
		auto& operator /= (TimeSpan time)
		{
			value /= time.ToMicroseconds();

			return *this;
		}
		auto& operator *= (TimeSpan time)
		{
			value *= time.ToMicroseconds();

			return *this;
		}

		auto operator + (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time += time;

			return _time;
		}
		auto operator - (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time -= time;

			return _time;
		}
		auto operator / (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time /= time;

			return _time;
		}
		auto operator * (TimeSpan time) const
		{
			TimeSpan _time = *this;
			_time *= time;

			return _time;
		}

		bool operator > (TimeSpan time) const
		{
			return value > time.value;
		}
		bool operator < (TimeSpan time) const
		{
			return value < time.value;
		}

		bool operator >= (TimeSpan time) const
		{
			return value >= time.value;
		}
		bool operator <= (TimeSpan time) const
		{
			return value <= time.value;
		}

		bool operator == (TimeSpan time) const
		{
			return value == time.value;
		}
		bool operator != (TimeSpan time) const
		{
			return !operator==(time);
		}
	};

	class Timestamp
	{
		uint64 value;

	public:
		static Timestamp FromMicroseconds(uint64 value)
		{
			return Timestamp(
				value
			);
		}

		static Timestamp FromMilliseconds(uint64 value)
		{
			return Timestamp(
				value * 1000
			);
		}

		static Timestamp FromSeconds(uint64 value)
		{
			return Timestamp(
				value * 1000000
			);
		}

		static Timestamp FromMinutes(uint64 value)
		{
			return Timestamp(
				value * 60000000
			);
		}

		static Timestamp FromHours(uint64 value)
		{
			return Timestamp(
				value * 3600000000
			);
		}

		static Timestamp FromDays(uint64 value)
		{
			return Timestamp(
				value * 86400000000
			);
		}

		Timestamp()
			: Timestamp(
				0
			)
		{
		}

		Timestamp(uint64 microseconds)
			: value(
				microseconds
			)
		{
		}

		virtual ~Timestamp()
		{
		}

		auto ToMicroseconds() const
		{
			return value;
		}

		auto ToMilliseconds() const
		{
			return value / 1000;
		}

		auto ToSeconds() const
		{
			return value / 1000000;
		}

		auto ToMinutes() const
		{
			return value / 60000000;
		}

		auto ToHours() const
		{
			return value / 3600000000;
		}

		auto ToDays() const
		{
			return value / 86400000000;
		}

		operator TimeSpan() const
		{
			return TimeSpan(value);
		}

		auto& operator += (TimeSpan time)
		{
			value += time.ToMicroseconds();

			return *this;
		}

		auto& operator -= (TimeSpan time)
		{
			auto ms = time.ToMicroseconds();

			value = (value >= ms) ? (value - ms) : ((~0 - ms) + value);

			return *this;
		}

		auto operator - (TimeSpan time) const
		{
			auto ms = time.ToMicroseconds();

			return TimeSpan(
				(value >= ms) ? (value - ms) : ((~0 - ms) + value)
			);
		}

		auto operator - (Timestamp time) const
		{
			return TimeSpan(
				value - time.value
			);
		}

		bool operator > (Timestamp time) const
		{
			return value > time.value;
		}
		bool operator < (Timestamp time) const
		{
			return value < time.value;
		}

		bool operator >= (Timestamp time) const
		{
			return value >= time.value;
		}
		bool operator <= (Timestamp time) const
		{
			return value <= time.value;
		}

		bool operator == (Timestamp time) const
		{
			return value == time.value;
		}
		bool operator != (Timestamp time) const
		{
			return !operator==(time);
		}
	};

	struct DateTime
	{
		enum class Months : uint8
		{
			January = 1,
			February,
			March,
			April,
			May,
			June,
			July,
			August,
			September,
			October,
			November,
			December
		};

		enum class DaysOfWeek : uint8
		{
			Sunday,
			Monday,
			Tuesday,
			Wednesday,
			Thursday,
			Friday,
			Saturday
		};

		uint32 Year;
		Months Month;
		uint32 Day;
		DaysOfWeek DayOfWeek;
		uint32 Hour;
		uint32 Minutes;
		uint32 Seconds;
		uint32 Milliseconds;

		DateTime()
			: Year(0),
			Month(Months::January),
			Day(0),
			DayOfWeek(DaysOfWeek::Sunday),
			Hour(0),
			Minutes(0),
			Seconds(0),
			Milliseconds(0)
		{
		}
	};

	struct Time
	{
		uint32 Hours;
		uint32 Minutes;
		uint32 Seconds;
		uint32 Milliseconds;

		Time()
			: Hours(
				0
			),
			Minutes(
				0
			),
			Seconds(
				0
			),
			Milliseconds(
				0
			)
		{
		}

		Time(const DateTime& dateTime)
			: Hours(
				dateTime.Hour
			),
			Minutes(
				dateTime.Minutes
			),
			Seconds(
				dateTime.Seconds
			),
			Milliseconds(
				dateTime.Milliseconds
			)
		{
		}
	};

	void Spin(TimeSpan duration);

	inline void Sleep(TimeSpan duration)
	{
#if defined(AL_PLATFORM_LINUX)
		::usleep(
			static_cast<useconds_t>(duration.ToMicroseconds())
		);
#elif defined(AL_PLATFORM_WINDOWS)
		::Sleep(
			static_cast<DWORD>(duration.ToMilliseconds())
		);
#endif
	}

	template<typename T>
	class Pointer
	{
		struct Context
		{
			T*     lpValue;
			size_t RefCount;
		};

		mutable Context* lpContext;

	public:
		Pointer()
			: Pointer(
				nullptr
			)
		{
		}

		Pointer(T* lpValue)
			: lpContext(
				(lpValue != nullptr) ? new Context() : nullptr
			)
		{
			if (lpValue != nullptr)
			{
				lpContext->lpValue = lpValue;
				lpContext->RefCount = 1;
			}
		}

		Pointer(Pointer&& pointer)
			: lpContext(
				pointer.lpContext
			)
		{
			pointer.lpContext = nullptr;
		}

		Pointer(const Pointer& pointer)
			: lpContext(
				pointer.lpContext
			)
		{
			if (lpContext != nullptr)
			{
				++lpContext->RefCount;
			}
		}

		virtual ~Pointer()
		{
			Release();
		}

		void AddReference()
		{
			if (lpContext != nullptr)
			{
				++lpContext->RefCount;
			}
		}
		
		void Release()
		{
			if (lpContext != nullptr)
			{
				if (--lpContext->RefCount == 0)
				{
					delete lpContext->lpValue;
					delete lpContext;
				}

				lpContext = nullptr;
			}
		}

		operator bool () const
		{
			if (lpContext == nullptr)
			{

				return false;
			}

			if (lpContext->lpValue == nullptr)
			{

				return false;
			}

			return true;
		}

		T* operator -> ()
		{
			return (lpContext != nullptr) ? lpContext->lpValue : nullptr;
		}
		const T* operator -> () const
		{
			return (lpContext != nullptr) ? lpContext->lpValue : nullptr;
		}

		Pointer& operator = (T* lpValue)
		{
			Release();

			if ((lpContext = (lpValue ? new Context() : nullptr)) != nullptr)
			{
				lpContext->lpValue = lpValue;
				lpContext->RefCount = 1;
			}

			return *this;
		}
		Pointer& operator = (Pointer&& pointer)
		{
			Release();

			lpContext = pointer.lpContext;
			pointer.lpContext = nullptr;

			return *this;
		}
		Pointer& operator = (const Pointer& pointer)
		{
			Release();

			if ((lpContext = pointer.lpContext) != nullptr)
			{
				++lpContext->RefCount;
			}

			return *this;
		}
	};

#if defined(AL_PLATFORM_LINUX)
	typedef int(Main)(int argc, char* argv[]);
#elif defined(AL_PLATFORM_WINDOWS)
	typedef int(WINAPIV Main)(int argc, char* argv[], char* envp[]);
	typedef int(WINAPI WinMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
	typedef int(WINAPI wWinMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

	typedef BOOL(WINAPI DllMain)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
#endif
}

#include "Exceptions/Exception.hpp"
#include "Exceptions/SystemException.hpp"
#include "Exceptions/NotImplementedException.hpp"
#include "Exceptions/DependencyMissingException.hpp"

#if defined(AL_CLRCALL)
	#include "Exceptions/DotNETException.hpp"
#endif

#if defined(AL_PLATFORM_WINDOWS)
	#include "OS/ntdll.hpp"
#endif

#include "OS/Debug.hpp"
#include "OS/Console.hpp"

#include "OS/ErrorCode.hpp"

#include "OS/System.hpp"
#include "OS/Environment.hpp"

#include "OS/Library.hpp"
#include "OS/Process.hpp"

#include "OS/Timer.hpp"

#include "OS/Mutex.hpp"
#include "OS/MutexCondition.hpp"

#include "OS/Thread.hpp"

#include "OS/AsyncFunction.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#include "OS/NamedPipe.hpp"
	#include "OS/CompletionPort.hpp"
#endif

#include "OS/Dialog.hpp"
#include "OS/FileDialog.hpp"
#include "OS/OpenFileDialog.hpp"
#include "OS/SaveFileDialog.hpp"

#include "OS/Window.hpp"

#include "OS/Detour.hpp"

#if defined(AL_CLRCALL)
	#include "DotNET/Marshal.hpp"
#endif

inline void AL::Spin(TimeSpan duration)
{
	OS::Timer timer;

	while (timer.GetElapsed() < duration)
	{
	}
}
