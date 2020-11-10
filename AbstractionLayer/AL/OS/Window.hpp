#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#include "AL/DirectX/Direct2D.hpp"
#else
	#include "AL/OpenGL/Device.hpp"
#endif

namespace AL::OS
{
	class Window;
}
