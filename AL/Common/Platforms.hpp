#pragma once
#include "AL/Common.hpp"

// NOTE: Changes made to AL::Platforms must be updated in AL/Common/String.hpp

namespace AL
{
	// 0 = Debug
	// 1 = Compiler
	// 2 = Architecture
	// 3 = Operating System
	// ???? 3333 2222 1110
	// 0000 0000 0000 0000
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
		Linux      = 0x0200,
		MinGW      = 0x0400,
		Windows    = 0x0800,

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

#if defined(AL_PLATFORM_PICO)
		Pico
#elif defined(AL_PLATFORM_LINUX)
		Linux
#elif defined(AL_PLATFORM_WINDOWS)
		Windows
	#if defined(AL_PLATFORM_WINDOWS_MINGW32) || defined(AL_PLATFORM_WINDOWS_MINGW64)
		| MinGW
	#endif
#endif
	};

	enum class PlatformShifts : uint8
	{
		Debug        = 0,
		Compiler     = 1,
		Architecture = 4,
		OS           = 8
	};

	enum class PlatformMasks : uint16
	{
		Debug        = 0b0000000000000001,
		Compiler     = 0b0000000000001110,
		Architecture = 0b0000000011110000,
		OS           = 0b0000111100000000
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(Platforms);
	AL_DEFINE_ENUM_FLAG_OPERATORS(PlatformMasks);

	AL_DEFINE_ENUM_FLAG_OPERATORS_2(Platforms, PlatformMasks);

	AL_DEFINE_ENUM_FLAG_OPERATORS_LSHIFT_2(Platforms, PlatformShifts);
	AL_DEFINE_ENUM_FLAG_OPERATORS_RSHIFT_2(Platforms, PlatformShifts);
}
