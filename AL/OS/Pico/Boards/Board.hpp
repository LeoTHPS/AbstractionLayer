#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

namespace AL::OS::Pico::Boards
{
	template<uint32 S_FLASH_SIZE, typename T_ADDRESS_MAP, typename T_DEFAULT_PINS>
	struct Board
	{
		typedef T_ADDRESS_MAP  ADDRESS_MAP;
		typedef T_DEFAULT_PINS DEFAULT_PINS;

		static constexpr uint32 FLASH_SIZE = S_FLASH_SIZE;
	};
}
