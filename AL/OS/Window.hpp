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
	class Window;

	class WindowGraphics
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Windows::GDI::Device device;
#endif

	public:
#if defined(AL_PLATFORM_LINUX)
		WindowGraphics()
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		explicit WindowGraphics(Windows::GDI::Device&& device)
			: device(
				Move(device)
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
			device.DrawString(
				string,
				x,
				y
			);
#else
			throw PlatformNotSupportedException();
#endif
		}
		template<typename T>
		Void DrawString(const String& string, const Drawing::Rectangle<T>& rectangle)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			device.DrawString(
				string,
				rectangle
			);
#else
			throw PlatformNotSupportedException();
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

			device.FillRectangle(
				rectangle,
				brush
			);
#else
			throw PlatformNotSupportedException();
#endif
		}

		WindowGraphics& operator = (WindowGraphics&& windowGraphics)
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			device = Move(
				windowGraphics.device
			);
#endif

			return *this;
		}

		Bool operator == (const WindowGraphics& windowGraphics) const
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (device != windowGraphics.device)
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
				: Window(
					Move(name)
				),
				lpWindow(
					&window
				)
			{
			}

			virtual Bool Native_IsOpen() const override
			{
				return Window::IsOpen();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				return Window::IsContentLoaded();
			}

			virtual ::HWND Native_GetHandle() const override
			{
				return Window::GetHandle();
			}

			// @throw AL::Exception
			virtual Void Native_Open() override
			{
				Window::Open();
			}

			// @throw AL::Exception
			virtual Void Native_Close() override
			{
				Window::Close();
			}

			// @throw AL::Exception
			virtual Void Native_LoadContent() override
			{
				Window::LoadContent();
			}

			virtual Void Native_UnloadContent() override
			{
				Window::UnloadContent();
			}

			// @throw AL::Exception
			// @return False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				if (!Window::Update(delta))
				{

					return False;
				}

				return True;
			}

		protected:
			// @throw AL::Exception
			virtual Void OnOpen() override
			{
				lpWindow->OnOpen();
			}

			virtual Void OnClose() override
			{
				lpWindow->OnClose();
			}

			// @throw AL::Exception
			virtual Void OnPaint(Windows::GDI::Device& device) override
			{
				WindowGraphics graphics(
					Move(device)
				);

				lpWindow->OnPaint(
					graphics
				);
			}

			// @throw AL::Exception
			virtual Void OnUpdate(TimeSpan delta) override
			{
				lpWindow->OnUpdate(
					delta
				);
			}

			// @throw AL::Exception
			virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
			{
				if (lpWindow->OnMouseEvent(event))
				{

					return True;
				}

				return False;
			}

			// @throw AL::Exception
			virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
			{
				if (lpWindow->OnKeyboardEvent(event))
				{

					return True;
				}

				return False;
			}
		};
#endif

		INativeWindow* const lpNativeWindow;

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

	protected:
		// @throw AL::Exception
		virtual Void OnOpen()
		{
		}

		virtual Void OnClose()
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
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event)
		{

			return False;
		}

		// @throw AL::Exception
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event)
		{

			return False;
		}
	};
}
