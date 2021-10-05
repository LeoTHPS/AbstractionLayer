#pragma once
#include "AL/Common.hpp"

namespace AL
{
	enum class Platforms : uint8
	{
		x86        = 0x1,
		x86_64     = 0x2,

		ARM        = 0x4,
		ARM64      = 0x8,

		Linux      = 0x10,
		Windows    = 0x20,

#if defined(AL_PLATFORM_WINDOWS)
		Machine    = Windows
#elif defined(AL_PLATFORM_LINUX)
		Machine    = Linux
#else
		Machine    = 0
#endif

#if defined(AL_X86)
		| x86
#elif defined(AL_X86_64)
		| x86_64
#elif defined(AL_ARM)
		| ARM
#elif defined(AL_ARM64)
		| ARM64
#endif
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(Platforms);
}
