#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "AL/OpenGL/Context.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/OS/Windows/DirectX/Direct2D.hpp"
#endif

namespace AL::Game::Engine
{
	class Graphics
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		OS::Windows::DirectX::Direct2D direct2D;
#endif

	public:
	};
}
