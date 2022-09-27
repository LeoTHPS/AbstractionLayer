#pragma once
#include "AL/Common.hpp"

// NOTE: Changes made to AL::Platforms must be updated in AL/Common/String.hpp

namespace AL
{
	enum class Platforms : uint16
	{
		// Debug

		Debug      = 0x0001,

		// Compiler

		GNU        = 0x0002,
		MSVC       = 0x0004,
		Clang      = 0x0008,

		// CPU Architecture

		ARM        = 0x0010,
		ARM64      = 0x0020,

		x86        = 0x0040,
		x86_64     = 0x0080,

		// Operating System

		Pico       = 0x0100,
		PicoW      = 0x0200,
		RP2040     = 0x0400,
		Linux      = 0x0800,
		MinGW      = 0x1000,
		Windows    = 0x2000,

		// Machine Constant

		Machine    =

#if defined(AL_DEBUG)
		Debug |
#endif

#if defined(AL_COMPILER_GNU)
		GNU |
#elif defined(AL_COMPILER_MSVC)
		MSVC |
#elif defined(AL_COMPILER_CLANG)
		Clang |
#endif

#if defined(AL_ARCH_ARM)
		ARM |
#elif defined(AL_ARCH_ARM64)
		ARM64 |
#elif defined(AL_ARCH_X86)
		x86 |
#elif defined(AL_ARCH_X86_64)
		x86_64 |
#endif

#if defined(AL_PLATFORM_RP2040)
		RP2040

		#if defined(AL_PLATFORM_PICO)
			| Pico
		#elif defined(AL_PLATFORM_PICO_W)
			| PicoW
		#endif
#elif defined(AL_PLATFORM_LINUX)
		Linux
#elif defined(AL_PLATFORM_WINDOWS)
		Windows

	#if defined(AL_PLATFORM_WINDOWS_MINGW32) || defined(AL_PLATFORM_WINDOWS_MINGW64)
		| MinGW
	#endif
#endif
	};
}
