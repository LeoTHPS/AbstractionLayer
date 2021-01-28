#pragma once
#include "AL/Common.hpp"

#include "Color.hpp"
#include "Colors.hpp"
#include "Vector.hpp"
#include "Rectangle.hpp"

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
		bool isTargetCreated = false;

		bool isVSyncEnabled = true;

		RenderWindow* const lpWindow;

		Color backgroundColor;

#if defined(AL_PLATFORM_LINUX)
		OpenGL::Device openGL;
#elif defined(AL_PLATFORM_WINDOWS)
		DirectX::Direct2D direct2D;
		DirectX::Direct3D direct3D;
#endif

		RenderTarget(RenderTarget&&) = delete;
		RenderTarget(const RenderTarget&) = delete;

	public:
		explicit RenderTarget(RenderWindow& window)
			: lpWindow(
				&window
			),
			backgroundColor(
				Colors::CornflowerBlue
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

		bool IsTargetCreated() const
		{
			return isTargetCreated;
		}

		bool IsVSyncEnabled() const
		{
			return isVSyncEnabled;
		}

		auto& GetWindow()
		{
			return *lpWindow;
		}
		auto& GetWindow() const
		{
			return *lpWindow;
		}

#if defined(AL_PLATFORM_LINUX)
		auto& GetOpenGL()
		{
			return openGL;
		}
		auto& GetOpenGL() const
		{
			return openGL;
		}
#elif defined(AL_PLATFORM_WINDOWS)
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
#endif

		auto GetBackgroundColor() const
		{
			return backgroundColor;
		}

		void EnableVSync(bool set = true)
		{
			isVSyncEnabled = set;
		}

		void SetBackgroundColor(Color value)
		{
			backgroundColor = value;
		}

		// @throw AL::Exceptions::Exception
		void Create();

		void Destroy();

		// @throw AL::Exceptions::Exception
		void CreateTarget();

		void DestroyTarget();

		// @throw AL::Exceptions::Exception
		void Clear()
		{
			AL_ASSERT(IsCreated(), "RenderTarget not created");
			AL_ASSERT(IsTargetCreated(), "RenderTarget target not created");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				GetDirect3D().Clear(
					GetBackgroundColor()
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error clearing DirectX::Direct3D"
				);
			}

			try
			{
				GetDirect2D().BeginDraw();
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error beginning DirectX::Direct2D draw"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		// @return false if target needs recreated
		bool Present()
		{
			AL_ASSERT(IsCreated(), "RenderTarget not created");
			AL_ASSERT(IsTargetCreated(), "RenderTarget target not created");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				if (!GetDirect2D().EndDraw())
				{

					return false;
				}
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error ending DirectX::Direct2D draw"
				);
			}

			try
			{
				GetDirect3D().Present(
					IsVSyncEnabled()
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error presenting DirectX::Direct3D"
				);
			}
#endif

			return true;
		}
	};
}
