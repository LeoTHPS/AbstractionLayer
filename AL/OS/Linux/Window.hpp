#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#warning Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Linux
{
	class Window;
}
