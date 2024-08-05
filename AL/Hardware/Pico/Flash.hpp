#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include <hardware/flash.h>

namespace AL::Hardware::Pico
{
	class Flash
	{
		Flash() = delete;

	public:
		static uint64 GetUniqueId()
		{
			::uint8_t value[FLASH_UNIQUE_ID_SIZE_BYTES];
			static_assert(sizeof(uint64) == FLASH_UNIQUE_ID_SIZE_BYTES);

			::flash_get_unique_id(
				&value[0]
			);

			return *reinterpret_cast<const uint64*>(&value[0]);
		}

		// @return AL::False if program is running from flash
		static Bool Clear()
		{
#if defined(AL_PICO_NO_FLASH)
			Erase(
				0,
				AL_PICO_FLASH_SIZE
			);

			return True;
#else
			return False;
#endif
		}

		static Void Erase(uint32 offset, size_t size)
		{
			::flash_range_erase(
				offset,
				size
			);
		}

		static Void Write(uint32 offset, const Void* lpBuffer, size_t size)
		{
			::flash_range_program(
				offset,
				reinterpret_cast<const ::uint8_t*>(lpBuffer),
				size
			);
		}
	};
}
