#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "AL/OpenGL/Device.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/DirectX/Direct2D.hpp"
	#include "AL/DirectX/Direct3D.hpp"
#endif

namespace AL::Graphics
{
	class RenderTarget;
}
