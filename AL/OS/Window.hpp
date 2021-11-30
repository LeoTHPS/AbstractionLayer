#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Window.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Window.hpp"
#endif

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include "AL/Hardware/Mouse.hpp"
#include "AL/Hardware/Keyboard.hpp"

namespace AL::OS
{
	class WindowGraphics
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Windows::GDI::Device* const lpDevice;
#endif

		WindowGraphics(WindowGraphics&&) = delete;
		WindowGraphics(const WindowGraphics&) = delete;

	public:
#if defined(AL_PLATFORM_LINUX)
		WindowGraphics()
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		explicit WindowGraphics(Windows::GDI::Device& device)
			: lpDevice(
				&device
			)
		{
		}
#endif

		virtual ~WindowGraphics()
		{
		}

		// @throw AL::Exception
		template<typename T>
		Void DrawString(const String& string, T x, T y)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			lpDevice->DrawString(
				string,
				x,
				y
			);
#endif
		}
		template<typename T>
		Void DrawString(const String& string, const Drawing::Rectangle<T>& rectangle)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			lpDevice->DrawString(
				string,
				rectangle
			);
#endif
		}

		// @throw AL::Exception
		template<typename T>
		Void FillRectangle(const Drawing::Rectangle<T>& rectangle, Drawing::Color color)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto brush = Windows::GDI::SolidColorBrush::Create(
				color
			);

			lpDevice->FillRectangle(
				rectangle,
				brush
			);
#endif
		}

		Bool operator == (const WindowGraphics& windowGraphics) const
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (lpDevice != windowGraphics.lpDevice)
			{

				return False;
			}
#endif

			return True;
		}
		Bool operator != (const WindowGraphics& windowGraphics) const
		{
			if (operator==(windowGraphics))
			{

				return False;
			}

			return True;
		}
	};

	class Window
	{
		class INativeWindow
		{
			INativeWindow(INativeWindow&&) = delete;
			INativeWindow(const INativeWindow&) = delete;

		public:
			INativeWindow()
			{
			}

			virtual ~INativeWindow()
			{
			}

			virtual Bool Native_IsOpen() const = 0;

			virtual Bool Native_IsContentLoaded() const = 0;

#if defined(AL_PLATFORM_LINUX)
			virtual Void* Native_GetHandle() const = 0;
#elif defined(AL_PLATFORM_WINDOWS)
			virtual ::HWND Native_GetHandle() const = 0;
#endif

			// @throw AL::Exception
			virtual Void Native_Open() = 0;

			// @throw AL::Exception
			virtual Void Native_Close() = 0;

			// @throw AL::Exception
			virtual Void Native_LoadContent() = 0;

			virtual Void Native_UnloadContent() = 0;

			// @throw AL::Exception
			// @return False on close
			virtual Bool Native_Update(TimeSpan delta) = 0;
		};

#if defined(AL_PLATFORM_LINUX)
		class NativeWindow
			: public INativeWindow
		{
			OS::Window* const lpWindow;

		public:
			NativeWindow(OS::Window& window, String&& name)
				: lpWindow(
					&window
				)
			{
			}

			virtual Bool Native_IsOpen() const override
			{
				throw NotImplementedException();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				throw NotImplementedException();
			}

			virtual Void* Native_GetHandle() const override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_Open() override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_Close() override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_LoadContent() override
			{
				throw NotImplementedException();
			}

			virtual Void Native_UnloadContent() override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			// @return False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				throw NotImplementedException();
			}
		};
#elif defined(AL_PLATFORM_WINDOWS)
		class NativeWindow
			: public INativeWindow,
			public Windows::Window
		{
			OS::Window* const lpWindow;

		public:
			NativeWindow(OS::Window& window, String&& name)
				: Windows::Window(
					Move(name)
				),
				lpWindow(
					&window
				)
			{
			}

			virtual Bool Native_IsOpen() const override
			{
				return Windows::Window::IsOpen();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				return Windows::Window::IsContentLoaded();
			}

			virtual ::HWND Native_GetHandle() const override
			{
				return Windows::Window::GetHandle();
			}

			// @throw AL::Exception
			virtual Void Native_Open() override
			{
				Windows::Window::Open();
			}

			// @throw AL::Exception
			virtual Void Native_Close() override
			{
				Windows::Window::Close();
			}

			// @throw AL::Exception
			virtual Void Native_LoadContent() override
			{
				Windows::Window::LoadContent();
			}

			virtual Void Native_UnloadContent() override
			{
				Windows::Window::UnloadContent();
			}

			// @throw AL::Exception
			// @return False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				if (!Windows::Window::Update(delta))
				{

					return False;
				}

				return True;
			}

		protected:
			// @throw AL::Exception
			virtual Void OnCreate() override
			{
				Windows::Window::OnCreate();

				try
				{
					lpWindow->OnCreate();
				}
				catch (Exception& exception)
				{
					Windows::Window::OnDestroy();

					throw;
				}
			}

			virtual Void OnDestroy() override
			{
				lpWindow->OnDestroy();

				Windows::Window::OnDestroy();
			}

			// @throw AL::Exception
			virtual Void OnLoadContent() override
			{
				Windows::Window::OnLoadContent();

				try
				{
					lpWindow->OnLoadContent();
				}
				catch (Exception& exception)
				{
					Windows::Window::OnUnloadContent();

					throw;
				}
			}

			virtual Void OnUnloadContent() override
			{
				lpWindow->OnUnloadContent();

				Windows::Window::OnUnloadContent();
			}

			// @throw AL::Exception
			virtual Void OnOpen() override
			{
				Windows::Window::OnOpen();

				try
				{
					lpWindow->OnOpen();
				}
				catch (Exception& exception)
				{
					Windows::Window::OnClose();

					throw;
				}
			}

			// @return False to cancel
			virtual Bool OnClosing() override
			{
				if (!Windows::Window::OnClosing())
				{

					return False;
				}

				return True;
			}

			virtual Void OnClose() override
			{
				lpWindow->OnClose();

				Windows::Window::OnClose();
			}

			// @throw AL::Exception
			virtual Void OnPaint(Windows::GDI::Device& device) override
			{
				Windows::Window::OnPaint(
					device
				);

				{
					WindowGraphics graphics(
						device
					);

					lpWindow->OnPaint(
						graphics
					);
				}
			}

			// @throw AL::Exception
			virtual Void OnUpdate(TimeSpan delta) override
			{
				Windows::Window::OnUpdate(
					delta
				);

				lpWindow->OnUpdate(
					delta
				);
			}

			// @throw AL::Exception
			virtual Void OnFileDrop(const String& path) override
			{
				Windows::Window::OnFileDrop(
					path
				);


			}

			// @throw AL::Exception
			virtual Void OnIconChanged(const Windows::WindowIcon& icon) override
			{
				Windows::Window::OnIconChanged(
					icon
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnIconChanging(const Windows::WindowIcon& icon) override
			{
				if (!Windows::Window::OnIconChanging(icon))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnTitleChanged(const String& title) override
			{
				Windows::Window::OnTitleChanged(
					title
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnTitleChanging(const String& title) override
			{
				if (!Windows::Window::OnTitleChanging(title))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnCursorChanged(const Windows::WindowCursor& cursor) override
			{
				Windows::Window::OnCursorChanged(
					cursor
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnCursorChanging(const Windows::WindowCursor& cursor) override
			{
				if (!Windows::Window::OnCursorChanging(cursor))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnPositionChanged(Windows::WindowPosition::Type x, Windows::WindowPosition::Type y) override
			{
				Windows::Window::OnPositionChanged(
					x,
					y
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnPositionChanging(Windows::WindowPosition::Type x, Windows::WindowPosition::Type y) override
			{
				if (!Windows::Window::OnPositionChanging(x, y))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnResolutionChanged(Windows::WindowResolution::Type width, Windows::WindowResolution::Type height) override
			{
				Windows::Window::OnResolutionChanged(
					width,
					height
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnResolutionChanging(Windows::WindowResolution::Type width, Windows::WindowResolution::Type height) override
			{
				if (!Windows::Window::OnResolutionChanging(width, height))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			// @return True if handled
			virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
			{
				if (Windows::Window::OnMouseEvent(event))
				{

					return True;
				}

				return False;
			}

			// @throw AL::Exception
			// @return True if handled
			virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
			{
				if (Windows::Window::OnKeyboardEvent(event))
				{

					return True;
				}

				return False;
			}

			// @throw AL::Exception
			virtual Void OnClipboardDataChanged(Windows::WindowClipboardDataFormats format) override
			{
				Windows::Window::OnClipboardDataChanged(
					format
				);


			}
		};
#endif

		INativeWindow* lpNativeWindow;

		Window(Window&&) = delete;
		Window(const Window&) = delete;

	public:
		explicit Window(String&& name)
			: lpNativeWindow(
				new NativeWindow(
					*this,
					Move(name)
				)
			)
		{
		}

		virtual ~Window()
		{
			delete lpNativeWindow;
		}

		Bool IsOpen() const
		{
			return lpNativeWindow->Native_IsOpen();
		}

		Bool IsContentLoaded() const
		{
			return lpNativeWindow->Native_IsContentLoaded();
		}

		auto GetHandle() const
		{
			return lpNativeWindow->Native_GetHandle();
		}

		// @throw AL::Exception
		Void Open()
		{
			lpNativeWindow->Native_Open();
		}

		// @throw AL::Exception
		Void Close()
		{
			lpNativeWindow->Native_Close();
		}

		// @throw AL::Exception
		Void LoadContent()
		{
			lpNativeWindow->Native_LoadContent();
		}

		Void UnloadContent()
		{
			lpNativeWindow->Native_UnloadContent();
		}

		// @throw AL::Exception
		// @return False on close
		Bool Update(TimeSpan delta)
		{
			if (!lpNativeWindow->Native_Update(delta))
			{

				return False;
			}

			return True;
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnOpen()
		{
		}

		virtual Void OnClose()
		{
		}

		// @throw AL::Exception
		virtual Void OnCreate()
		{
		}

		virtual Void OnDestroy()
		{
		}

		// @throw AL::Exception
		virtual Void OnLoadContent()
		{
		}

		virtual Void OnUnloadContent()
		{
		}

		// @throw AL::Exception
		virtual Void OnPaint(WindowGraphics& graphics)
		{
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta)
		{
		}

		// @throw AL::Exception
		// @return True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event)
		{

			return False;
		}

		// @throw AL::Exception
		// @return True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event)
		{

			return False;
		}
	};
}
