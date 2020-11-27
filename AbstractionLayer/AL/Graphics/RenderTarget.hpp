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
	class RenderWindow;

	class RenderTarget
	{
		bool isCreated = false;

		RenderWindow* const lpWindow;

		DirectX::Direct2D direct2D;
		DirectX::Direct3D direct3D;

		RenderTarget(RenderTarget&&) = delete;
		RenderTarget(const RenderTarget&) = delete;

	public:
		explicit RenderTarget(RenderWindow& window)
			: lpWindow(
				&window
			)
		{
		}

		virtual ~RenderTarget()
		{
			Destroy();
		}

		bool IsCreated() const
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

		auto& GetDirect2D()
		{
			return direct2D;
		}
		auto& GetDirect2D() const
		{
			return direct2D;
		}

		auto& GetDirect3D()
		{
			return direct3D;
		}
		auto& GetDirect3D() const
		{
			return direct3D;
		}

		// @throw AL::Exceptions::Exception
		void Create()
		{
			AL_ASSERT(!IsCreated(), "RenderTarget already created");



			isCreated = true;
		}

		void Destroy()
		{
			if (IsCreated())
			{


				isCreated = false;
			}
		}

		// @throw AL::Exceptions::Exception
		void Clear(Color color)
		{
			AL_ASSERT(IsCreated(), "RenderTarget not created");


		}

		// @throw AL::Exceptions::Exception
		void Present(bool vsync)
		{
			AL_ASSERT(IsCreated(), "RenderTarget not created");


		}
	};
}
