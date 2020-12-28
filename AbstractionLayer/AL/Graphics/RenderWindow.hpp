#pragma once
#include "AL/Common.hpp"

#include "RenderTarget.hpp"

#include "AL/OS/Window.hpp"

namespace AL::Graphics
{
	class RenderWindow
		: public OS::Window
	{
		bool isContentLoaded = false;

		bool isResizeInProgress = false;

		RenderTarget target;

	public:
		RenderWindow(String&& title, String&& className)
			: Window(
				Move(title),
				Move(className)
			),
			target(
				*this
			)
		{
		}

		bool IsContentLoaded() const
		{
			return isContentLoaded;
		}

		auto& GetTarget()
		{
			return target;
		}
		auto& GetTarget() const
		{
			return target;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnCreate() override
		{
			Window::OnCreate();

			try
			{
				GetTarget().Create();
			}
			catch (Exceptions::Exception& exception)
			{
				Window::OnDestroy();

				throw Exceptions::Exception(
					Move(exception),
					"Error creating RenderTarget"
				);
			}

			try
			{
				GetTarget().CreateTarget();
			}
			catch (Exceptions::Exception& exception)
			{
				GetTarget().Destroy();

				Window::OnDestroy();

				throw Exceptions::Exception(
					Move(exception),
					"Error creating RenderTarget target"
				);
			}

			try
			{
				LoadContent();
			}
			catch (Exceptions::Exception& exception)
			{
				GetTarget().DestroyTarget();
				GetTarget().Destroy();

				Window::OnDestroy();

				throw Exceptions::Exception(
					Move(exception),
					"Error loading RenderWindow content"
				);
			}
		}

		virtual void OnDestroy() override
		{
			UnloadContent();

			GetTarget().DestroyTarget();
			GetTarget().Destroy();

			Window::OnDestroy();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnLoadContent()
		{
		}

		virtual void OnUnloadContent()
		{
		}

		// @throw AL::Exceptions::Exception
		virtual void OnDraw(float delta) override
		{
			Window::OnDraw(
				delta
			);

			if (!isResizeInProgress)
			{
				try
				{
					GetTarget().Clear();
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error clearing RenderTarget"
					);
				}
			}
		}

		// @throw AL::Exceptions::Exception
		virtual void OnEndDraw(float delta) override
		{
			if (!isResizeInProgress)
			{
				bool recreateTarget;

				try
				{
					recreateTarget = !GetTarget().Present();
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error presenting RenderTarget"
					);
				}

				while (recreateTarget)
				{
					UnloadContent();
					GetTarget().DestroyTarget();

					try
					{
						GetTarget().CreateTarget();
					}
					catch (Exceptions::Exception& exception)
					{

						throw Exceptions::Exception(
							Move(exception),
							"Error recreating RenderTarget target"
						);
					}

					try
					{
						LoadContent();
					}
					catch (Exceptions::Exception& exception)
					{

						throw Exceptions::Exception(
							Move(exception),
							"Error reloading RenderWindow content"
						);
					}

					try
					{
						recreateTarget = !GetTarget().Present();
					}
					catch (Exceptions::Exception& exception)
					{

						throw Exceptions::Exception(
							Move(exception),
							"Error presenting RenderTarget"
						);
					}
				}
			}

			Window::OnEndDraw(
				delta
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false to prevent resize
		virtual bool OnResolutionChanging() override
		{
			if (!Window::OnResolutionChanging())
			{

				return false;
			}

			isResizeInProgress = true;

			return true;
		}

		// @throw AL::Exceptions::Exception
		virtual void OnResolutionChanged(const OS::WindowResolution& resolution) override
		{
			Window::OnResolutionChanged(
				resolution
			);

			isResizeInProgress = false;

			GetTarget().Destroy();

			try
			{
				GetTarget().Create();
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error recreating RenderTarget"
				);
			}
		}

	private:
		// @throw AL::Exceptions::Exception
		void LoadContent()
		{
			AL_ASSERT(!IsContentLoaded(), "RenderWindow content already loaded");

			OnLoadContent();

			isContentLoaded = true;
		}

		void UnloadContent()
		{
			if (IsContentLoaded())
			{
				OnUnloadContent();

				isContentLoaded = false;
			}
		}
	};
}

// @throw AL::Exceptions::Exception
inline void AL::Graphics::RenderTarget::Create()
{
	AL_ASSERT(!IsCreated(), "RenderTarget already created");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	try
	{
		GetDirect3D().Create(
			GetWindow().GetHandle()
		);
	}
	catch (Exceptions::Exception& exception)
	{

		throw Exceptions::Exception(
			Move(exception),
			"Error creating DirectX::Direct3D"
		);
	}

	try
	{
		GetDirect2D().Create();
	}
	catch (Exceptions::Exception& exception)
	{
		GetDirect3D().Destroy();

		throw Exceptions::Exception(
			Move(exception),
			"Error creating DirectX::Direct2D"
		);
	}
#endif

	isCreated = true;
}

inline void AL::Graphics::RenderTarget::Destroy()
{
	if (IsCreated())
	{
		DestroyTarget();

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		GetDirect2D().Destroy();
		GetDirect3D().Destroy();
#endif

		isCreated = false;
	}
}

// @throw AL::Exceptions::Exception
inline void AL::Graphics::RenderTarget::CreateTarget()
{
	AL_ASSERT(IsCreated(), "RenderTarget not created");
	AL_ASSERT(!IsTargetCreated(), "RenderTarget target already created");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	try
	{
		GetDirect3D().CreateTarget();
	}
	catch (Exceptions::Exception& exception)
	{

		throw Exceptions::Exception(
			Move(exception),
			"Error creating DirectX::Direct3D target"
		);
	}

	try
	{
		GetDirect2D().CreateTarget(
			GetDirect3D().GetSwapChain()
		);
	}
	catch (Exceptions::Exception& exception)
	{
		GetDirect3D().DestroyTarget();
		
		throw Exceptions::Exception(
			Move(exception),
			"Error creating DirectX::Direct2D target"
		);
	}
#endif

	isTargetCreated = true;
}

inline void AL::Graphics::RenderTarget::DestroyTarget()
{
	if (IsTargetCreated())
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		GetDirect2D().DestroyTarget();
		GetDirect3D().DestroyTarget();
#endif

		isTargetCreated = false;
	}
}
