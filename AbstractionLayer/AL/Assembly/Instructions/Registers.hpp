#pragma once
#include "AL/Common.hpp"

namespace AL::Assembly::Instructions
{
	enum class Registers
	{
#if defined(AL_X86)
		EAX,
		ECX,
		EDX,
		EBX,
		ESP,
		EBP,
		ESI,
		EDI
#elif defined(AL_X86_64)
		RAX,
		RCX,
		RDX,
		RBX,
		RSP,
		RBP,
		RSI,
		RDI,

		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15
#elif defined(AL_ARM) || defined(AL_ARM64)
		R0,
		R1,
		R2,
		R3,
		R4,
		R5,
		R6,
		R7,
		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15,
		CPSR
#endif
	};
}
