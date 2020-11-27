#pragma once
#include "AL/Common.hpp"

#include "RenderTarget.hpp"

#include "AL/OS/Window.hpp"

namespace AL::Graphics
{
	class RenderWindow
		: public OS::Window
	{
		bool isVSync = true;
		bool isResizeInProgress = false;

		RenderTarget target;

	public:
		RenderWindow()
			: target(
				*this
			)
		{
		}

		bool IsVSync() const
		{
			return isVSync;
		}

		auto& GetTarget()
		{
			return target;
		}
		auto& GetTarget() const
		{
			return target;
		}

		void SetVSync(bool set = true)
		{
			isVSync = set;
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
		}

		virtual void OnDestroy() override
		{
			GetTarget().Destroy();

			Window::OnDestroy();
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
					GetTarget().Clear(
						GetBackgroundColor()
					);
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
				try
				{
					GetTarget().Present(
						IsVSync()
					);
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error presenting RenderTarget"
					);
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
	};
}
