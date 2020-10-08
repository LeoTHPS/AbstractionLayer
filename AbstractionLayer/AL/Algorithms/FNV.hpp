#pragma once
#include "AL/Common.hpp"

namespace AL::Algorithms
{
	template<typename>
	struct _FNV_Constants;
	template<>
	struct _FNV_Constants<uint32>
	{
		static constexpr uint32 Prime = 0x1000193;
		static constexpr uint32 Offset = 0x811C9DC5;
	};
	template<>
	struct _FNV_Constants<uint64>
	{
		static constexpr uint64 Prime = 0x100000001B3;
		static constexpr uint64 Offset = 0xCBF29CE484222325;
	};

	template<typename T>
	class FNV
	{
	public:
		typedef T Hash;

		virtual ~FNV()
		{
		}

		template<typename _T>
		static constexpr T Calculate(const _T& value)
		{
			return Calculate(
				&value,
				sizeof(_T)
			);
		}

		template<size_t S>
		static constexpr T Calculate(const char(&string)[S])
		{
			Hash hash = _FNV_Constants<T>::Offset;

			for (size_t i = 0; i < S; i++)
			{
				hash ^= static_cast<T>(
					string[i]
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}

		static constexpr T Calculate(const void* lpBuffer, size_t size)
		{
			Hash hash = _FNV_Constants<T>::Offset;

			for (size_t i = 0; i < size; i++)
			{
				hash ^= static_cast<T>(
					static_cast<const uint8*>(lpBuffer)[i]
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}
	};

	typedef FNV<uint32> FNV32;
	typedef FNV<uint64> FNV64;
}
