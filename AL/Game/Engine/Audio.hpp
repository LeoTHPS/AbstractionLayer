#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/OS/Windows/DirectX/DirectSound.hpp"
#endif

namespace AL::Game::Engine
{
	class Audio;
}
