#pragma once
#include "AL/Common.hpp"

#include <cstring>
#include <algorithm>

namespace AL
{
	template<typename T>
	constexpr T declval();

	template<typename T>
	inline Void Swap(T& source, T& destination)
	{
		::std::swap(
			source,
			destination
		);
	}
	template<typename T, size_t S>
	inline Void Swap(T(&source)[S], T(&destination)[S])
	{
		::std::swap(
			source,
			destination
		);
	}

	template<typename T>
	inline typename Remove_Reference<T>::Type&& Move(T&& value)
	{
		return static_cast<typename Remove_Reference<T>::Type&&>(
			value
		);
	}

	template<typename T>
	inline typename Remove_Reference<T>::Type&& Forward(typename Remove_Reference<T>::Type& value)
	{
		return static_cast<T&&>(
			value
		);
	}
	template<typename T>
	inline typename Remove_Reference<T>::Type&& Forward(typename Remove_Reference<T>::Type&& value)
	{
		return static_cast<T&&>(
			value
		);
	}

	template<typename T, size_t S, size_t I>
	inline typename Remove_Reference<T>::Type&& Expand_Move(T(&values)[S])
	{
		return Move(
			values[I]
		);
	}
	template<typename T, size_t S, size_t ... I>
	inline typename Remove_Reference<T>::Type&& Expand_Move(T(&values)[S], Index_Sequence<I ...>)
	{
		return (Expand_Move<T, S, I>(values), ...);
	}
	template<typename T, size_t S>
	inline typename Remove_Reference<T>::Type&& Expand_Move(T(&values)[S])
	{
		return Expand_Move(
			values,
			typename Make_Index_Sequence<S>::Type {}
		);
	}

	template<typename T, size_t S, size_t I>
	inline const typename Remove_Reference<T>::Type& Expand_Copy(const T(&values)[S])
	{
		return values[I];
	}
	template<typename T, size_t S, size_t ... I>
	inline const typename Remove_Reference<T>::Type& Expand_Copy(const T(&values)[S], Index_Sequence<I ...>)
	{
		return (Expand_Copy<T, S, I>(values), ...);
	}
	template<typename T, size_t S>
	inline const typename Remove_Reference<T>::Type& Expand_Copy(const T(&values)[S])
	{
		return Expand_Copy(
			values,
			typename Make_Index_Sequence<S>::Type {}
		);
	}

	inline Void memset(Void* lpDestination, uint8 value, size_t size)
	{
		::std::memset(
			lpDestination,
			value,
			size
		);
	}

	template<typename T, size_t S>
	inline Void memcpy(T(&destination)[S], const T(&source)[S])
	{
		::std::memcpy(
			&destination[0],
			&source[0],
			S * sizeof(T)
		);
	}
	inline Void memcpy(Void* lpDestination, const Void* lpSource, size_t size)
	{
		::std::memcpy(
			lpDestination,
			lpSource,
			size
		);
	}

	template<typename T, size_t S>
	inline Void memmove(T(&destination)[S], const T(&source)[S])
	{
		::std::memmove(
			&destination[0],
			&source[0],
			S * sizeof(T)
		);
	}
	inline Void memmove(Void* lpDestination, const Void* lpSource, size_t size)
	{
		::std::memmove(
			lpDestination,
			lpSource,
			size
		);
	}

	template<typename T, size_t S>
	inline Bool memcmp(const T(&array)[S], const T(&array2)[S])
	{
		if (::std::memcmp(&array[0], &array2[0], S * sizeof(T)) != 0)
		{

			return False;
		}

		return True;
	}
	inline Bool memcmp(const Void* lpValue, const Void* lpValue2, size_t size)
	{
		if (::std::memcmp(lpValue, lpValue2, size) != 0)
		{

			return False;
		}

		return True;
	}
}