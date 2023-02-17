#pragma once
#include "AL/Common.hpp"

namespace AL
{
	// @return AL::False to stop
	typedef Function<Bool(size_t index, size_t offset, Bool value)> BitReaderEnumCallback;

	template<Endians ENDIAN>
	class BitReader
	{
		BitReader() = delete;

	public:
		template<typename F>
		static Void Enumerate(const Void* lpBuffer, size_t size, F&& callback)
		{
			Enumerate<F>(
				lpBuffer,
				size,
				0,
				Move(callback)
			);
		}
		template<typename F>
		static Void Enumerate(const Void* lpBuffer, size_t size, size_t offset, F&& callback)
		{
			Bool bit = False;

			auto lpByteBuffer = reinterpret_cast<const uint8*>(
				lpBuffer
			);

			lpByteBuffer += offset / 8;

			for (size_t i = (offset / 8); i < size; ++i, ++lpByteBuffer)
			{
				for (size_t j = (offset - (i * 8)); j < 8; ++j, ++offset)
				{
					if constexpr (ENDIAN == Endians::Big)
						bit = ((*lpByteBuffer >> (8 - j)) & 0x01) == 1;
					else if constexpr (ENDIAN == Endians::Little)
						bit = ((*lpByteBuffer >> j) & 0x01) == 1;

					if constexpr (Is_Type<decltype(callback(0, 0, False)), Void>::Value)
					{
						callback(
							i,
							j,
							bit
						);
					}
					else if (!callback(i, j, bit))
					{

						break;
					}
				}
			}
		}
		static Void Enumerate(const Void* lpBuffer, size_t size, const BitReaderEnumCallback& callback)
		{
			Enumerate(
				lpBuffer,
				size,
				0,
				callback
			);
		}
		static Void Enumerate(const Void* lpBuffer, size_t size, size_t offset, const BitReaderEnumCallback& callback)
		{
			Enumerate<const BitReaderEnumCallback&>(
				lpBuffer,
				size,
				offset,
				callback
			);
		}
	};

	typedef BitReader<Endians::Big>     BigBitReader;
	typedef BitReader<Endians::Little>  LittleBitReader;
	typedef BitReader<Endians::Machine> MachineBitReader;
}
