#pragma once
#include "AL/Common.hpp"

// NOTE: Changes made to AL::Platforms should be updated in AL/Common/String.hpp

namespace AL
{
	enum class Platforms : uint16
	{
		Debug      = 0x1,

		x86        = 0x10,
		x86_64     = 0x20,

		ARM        = 0x40,
		ARM64      = 0x80,

		GNU        = 0x100,
		MSVC       = 0x200,
		Clang      = 0x400,

		Linux      = 0x1000,
		MinGW      = 0x2000,
		Windows    = 0x4000,

		Machine    =

#if defined(AL_DEBUG)
		Debug |
#endif

#if defined(AL_PLATFORM_LINUX)
		Linux |
#elif defined(AL_PLATFORM_WINDOWS)
		Windows |
#endif

#if defined(AL_PLATFORM_WINDOWS_MINGW32) || defined(AL_PLATFORM_WINDOWS_MINGW64)
		MinGW |
#endif

#if defined(AL_COMPILER_GNU)
		GNU |
#elif defined(AL_COMPILER_MSVC)
		MSVC |
#elif defined(AL_COMPILER_CLANG)
		Clang |
#endif

#if defined(AL_ARCH_X86)
		x86
#elif defined(AL_ARCH_X86_64)
		x86_64
#elif defined(AL_ARCH_ARM)
		ARM
#elif defined(AL_ARCH_ARM64)
		ARM64
#endif
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(Platforms);
}
