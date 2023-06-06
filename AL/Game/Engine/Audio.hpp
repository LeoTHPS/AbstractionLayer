#pragma once
#include "AL/Common.hpp"

#if !defined(AL_GAME_ENGINE_DXSOUND)
	#if defined(AL_PLATFORM_LINUX)
		#define AL_GAME_ENGINE_
	#elif defined(AL_PLATFORM_WINDOWS)
		#define AL_GAME_ENGINE_DXSOUND
	#endif
#endif

#if defined(AL_GAME_ENGINE_)

#elif defined(AL_GAME_ENGINE_DXSOUND)
	#include "AL/OS/Windows/DirectX/DirectSound.hpp"
#endif

namespace AL::Game::Engine
{
	class Window;

	class Audio
	{
		Bool isCreated = False;

		Window* const lpWindow;

#if defined(AL_GAME_ENGINE_)

#elif defined(AL_GAME_ENGINE_DXSOUND)
		OS::Windows::DirectX::DirectSound dSound;
#endif

		Audio(Audio&&) = delete;
		Audio(const Audio&) = delete;

	public:
		explicit Audio(Window& window)
			: lpWindow(
				&window
			)
		{
		}

		virtual ~Audio()
		{
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		auto& GetWindow()
		{
			return *lpWindow;
		}
		auto& GetWindow() const
		{
			return *lpWindow;
		}

		// @throw AL::Exception
		Void Create();

		Void Destroy()
		{
			if (IsCreated())
			{
#if defined(AL_GAME_ENGINE_)

#elif defined(AL_GAME_ENGINE_DXSOUND)
				dSound.Destroy();
#endif

				isCreated = False;
			}
		}
	};
}
