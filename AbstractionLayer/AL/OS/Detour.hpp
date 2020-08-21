#pragma once
#include "AL/Common.hpp"

namespace AL::OS
{
	class Detour
	{
		Detour() = delete;

	public:
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void HookFunction(F* lpSource, F* lpDestination)
		{
#pragma pack(push, 1)
			struct Jump
			{
				uint8 OPCode;
				uint32 Offset;
			};
#pragma pack(pop)

#if defined(AL_PLATFORM_WINDOWS)
			DWORD protection;

			if (!VirtualProtect(lpSource, sizeof(Jump), PAGE_EXECUTE_READWRITE, &protection))
			{

				throw Exceptions::SystemException(
					"VirtualProtect"
				);
			}
#endif

			auto lpJump = reinterpret_cast<Jump*>(
				lpSource
			);

			lpJump->OPCode = 0xE9;
			lpJump->Offset = static_cast<uint32>(
				reinterpret_cast<uint64>(lpDestination) - (reinterpret_cast<uint64>(lpSource) + sizeof(Jump))
			);

#if defined(AL_PLATFORM_WINDOWS)
			if (!VirtualProtect(lpSource, sizeof(Jump), protection, &protection))
			{

				throw Exceptions::SystemException(
					"VirtualProtect"
				);
			}
#endif
		}
	};
}
