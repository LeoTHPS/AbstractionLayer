#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#if defined(AL_COMPILER_MSVC)
		#pragma warning(disable: 4307)
	#endif
#endif

namespace AL::Algorithms
{
	template<typename>
	struct _FNV_Constants;
	template<>
	struct _FNV_Constants<uint32>
	{
		static constexpr uint32 Prime  = 0x1000193;
		static constexpr uint32 Offset = 0x811C9DC5;
	};
	template<>
	struct _FNV_Constants<uint64>
	{
		static constexpr uint64 Prime  = 0x100000001B3;
		static constexpr uint64 Offset = 0xCBF29CE484222325;
	};

	template<typename T>
	class FNV
	{
		T hash;

	public:
		typedef T Hash;

		template<size_t S>
		static constexpr Hash Calculate(const char(&string)[S], size_t index = 0, Hash hash = _FNV_Constants<T>::Offset)
		{
			for (; index < (S - 1); ++index)
			{
				hash ^= static_cast<T>(
					static_cast<uint8>(string[index])
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}

		static constexpr Hash Calculate(const Void* lpBuffer, size_t size, size_t index = 0, Hash hash = _FNV_Constants<T>::Offset)
		{
			for (; index < size; ++index)
			{
				hash ^= static_cast<T>(
					static_cast<const uint8*>(lpBuffer)[index]
				);

				hash *= _FNV_Constants<T>::Prime;
			}

			return hash;
		}

		FNV()
			: hash(
				_FNV_Constants<T>::Offset
			)
		{
		}

		FNV(FNV&& fnv)
			: hash(
				fnv.hash
			)
		{
			fnv.hash = _FNV_Constants<T>::Offset;
		}
		FNV(const FNV& fnv)
			: hash(
				fnv.hash
			)
		{
		}

		Void Add(const Void* lpBuffer, size_t size)
		{
			for (size_t i = 0; i < size; ++i)
			{
				hash ^= static_cast<T>(
					static_cast<const uint8*>(lpBuffer)[i]
				);

				hash *= _FNV_Constants<T>::Prime;
			}
		}

		Void Reset()
		{
			hash = _FNV_Constants<T>::Offset;
		}

		Hash Calculate() const
		{
			return hash;
		}

		FNV& operator = (FNV&& fnv)
		{
			hash = fnv.hash;
			fnv.hash = _FNV_Constants<T>::Offset;

			return *this;
		}
		FNV& operator = (const FNV& fnv)
		{
			hash = fnv.hash;

			return *this;
		}

		Bool operator == (const FNV& fnv) const
		{
			if (Calculate() != fnv.Calculate())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const FNV& fnv) const
		{
			if (operator==(fnv))
			{

				return False;
			}

			return True;
		}
	};

	typedef FNV<uint32> FNV32;
	typedef FNV<uint64> FNV64;
}
