#pragma once
#include "AL/Common.hpp"

namespace AL::OS
{
	class Process;

#if defined(AL_PLATFORM_WINDOWS)
	AL_INLINE TEB* GetThreadEnvironmentBlock()
	{
		return reinterpret_cast<PTEB>(
#if defined(AL_X86)
			__readfsdword(
#elif defined(AL_X86_64)
			__readgsqword(
#endif
				reinterpret_cast<DWORD_PTR>(
					&static_cast<NT_TIB*>(nullptr)->Self
				)
			)
		);
	}

	AL_INLINE PEB* GetProcessEnvironmentBlock()
	{
		return GetThreadEnvironmentBlock()->ProcessEnvironmentBlock;
	}

	AL_INLINE bool IsDebuggerPresent()
	{
		return GetProcessEnvironmentBlock()->BeingDebugged != 0;
	}

	AL_INLINE void SetDebuggerPresent(bool set = true)
	{
		GetProcessEnvironmentBlock()->BeingDebugged = set ? 1 : 0;
	}
#endif
}
