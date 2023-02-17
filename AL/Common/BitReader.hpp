#pragma once
#include "AL/Common.hpp"

namespace AL
{
	// @return AL::False to stop
	typedef Function<Bool(size_t index, size_t offset, Bool value)> BitReaderEnumCallback;

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
			auto lpByteBuffer = reinterpret_cast<const uint8*>(
				lpBuffer
			);

			lpByteBuffer += offset / 8;

			for (size_t i = (offset / 8); i < size; ++i, ++lpByteBuffer)
			{
				for (size_t j = (offset - (i * 8)); j < 8; ++j, ++offset)
				{
					if constexpr (Is_Type<decltype(callback(0, 0, False)), Void>::Value)
					{
						callback(
							i,
							j,
							(*lpByteBuffer >> j) & 0x01
						);
					}
					else if (!callback(i, j, (*lpByteBuffer >> j) & 0x01))
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
}
