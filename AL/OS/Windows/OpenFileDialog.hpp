#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::OS::Windows
{
	class OpenFileDialog;
}