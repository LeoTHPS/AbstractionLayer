#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#pragma warning(disable: 4307)
#endif

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
		FNV() = delete;

	public:
		typedef T Hash;

		template<size_t S>
		static constexpr Hash Calculate(const char(&string)[S], size_t index = 0, Hash hash = _FNV_Constants<T>::Offset)
		{
			for (index = 0; index < (S - 1); ++index)
			{
				hash ^= static_cast<T>(
					static_cast<uint8>(string[index])
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}

		static Hash Calculate(const void* lpBuffer, size_t size, size_t index = 0, Hash hash = _FNV_Constants<T>::Offset)
		{
			for (index = 0; index < size; ++index)
			{
				hash ^= static_cast<T>(
					static_cast<const uint8*>(lpBuffer)[index]
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}
	};

	typedef FNV<uint32> FNV32;
	typedef FNV<uint64> FNV64;
}
