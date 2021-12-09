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

	enum class WindowCursors
	{
		None,
		Arrow,
		Beam,
		Wait,
		Hand,
		Help
	};

	typedef Drawing::Color      WindowColor;
	typedef Drawing::Colors     WindowColors;

	typedef Drawing::Vector2I32 WindowPosition;
	typedef Drawing::SizeU32    WindowResolution;

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

			virtual Bool Native_IsPaintEnabled() const = 0;

			virtual Bool Native_IsContentLoaded() const = 0;

			virtual const String& Native_GetTitle() const = 0;

#if defined(AL_PLATFORM_LINUX)
			virtual Void* Native_GetHandle() const = 0;
#elif defined(AL_PLATFORM_WINDOWS)
			virtual ::HWND Native_GetHandle() const = 0;
#endif

			virtual WindowCursors Native_GetCursor() const = 0;

			virtual const WindowPosition& Native_GetPosition() const = 0;

			virtual const WindowResolution& Native_GetResolution() const = 0;

			virtual WindowColor Native_GetBackgroundColor() const = 0;

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

			// @throw AL::Exception
			virtual Bool Native_SetTitle(String&& value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetCursor(WindowCursors value) = 0;

			// @throw AL::Exception
			virtual Void Native_SetBackgroundColor(WindowColor value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetResolution(WindowResolution::Type width, WindowResolution::Type height) = 0;

			virtual Void Native_EnablePaint(Bool set) = 0;
		};

#if defined(AL_PLATFORM_LINUX)
		class NativeWindow
			: public INativeWindow
		{
			OS::Window* const lpWindow;

		public:
			NativeWindow(OS::Window& window, String&& name, String&& title)
				: lpWindow(
					&window
				)
			{
			}

			virtual Bool Native_IsOpen() const override
			{
				throw NotImplementedException();
			}

			virtual Bool Native_IsPaintEnabled() const override
			{
				throw NotImplementedException();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				throw NotImplementedException();
			}

			virtual const String& Native_GetTitle() const override
			{
				throw NotImplementedException();
			}

			virtual Void* Native_GetHandle() const override
			{
				throw NotImplementedException();
			}

			virtual WindowCursors Native_GetCursor() const override
			{
				throw NotImplementedException();
			}

			virtual const WindowPosition& Native_GetPosition() const override
			{
				throw NotImplementedException();
			}

			virtual const WindowResolution& Native_GetResolution() const override
			{
				throw NotImplementedException();
			}

			virtual WindowColor Native_GetBackgroundColor() const override
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

			// @throw AL::Exception
			virtual Bool Native_SetTitle(String&& value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetCursor(WindowCursors value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_SetBackgroundColor(WindowColor value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetResolution(WindowResolution::Type width, WindowResolution::Type height) override
			{
				throw NotImplementedException();
			}

			virtual Void Native_EnablePaint(Bool set) override
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
			NativeWindow(OS::Window& window, String&& name, String&& title)
				: Windows::Window(
					Move(name),
					Move(title)
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

			virtual Bool Native_IsPaintEnabled() const override
			{
				return Windows::Window::IsPaintEnabled();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				return Windows::Window::IsContentLoaded();
			}

			virtual const String& Native_GetTitle() const override
			{
				return Window::GetTitle();
			}

			virtual ::HWND Native_GetHandle() const override
			{
				return Windows::Window::GetHandle();
			}

			virtual WindowCursors Native_GetCursor() const override
			{
				switch (Windows::Window::GetCursor())
				{
					case Windows::WindowCursors::None:        return WindowCursors::None;
					case Windows::WindowCursors::Arrow:       return WindowCursors::Arrow;
					case Windows::WindowCursors::Beam:        return WindowCursors::Beam;
					case Windows::WindowCursors::Wait:        return WindowCursors::Wait;
					case Windows::WindowCursors::Hand:        return WindowCursors::Hand;
					case Windows::WindowCursors::Help:        return WindowCursors::Help;
					case Windows::WindowCursors::UserDefined: break;
				}

				throw NotImplementedException();
			}

			virtual const WindowPosition& Native_GetPosition() const override
			{
				return Windows::Window::GetPosition();
			}

			virtual const WindowResolution& Native_GetResolution() const override
			{
				return Windows::Window::GetResolution();
			}

			// @throw AL::Exception
			virtual WindowColor Native_GetBackgroundColor() const override
			{
				return Windows::Window::GetBackgroundColor();
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

			// @throw AL::Exception
			virtual Bool Native_SetTitle(String&& value) override
			{
				if (!Window::SetTitle(Move(value)))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Bool Native_SetCursor(WindowCursors value) override
			{
				Windows::WindowCursor cursor(
					[value]()
					{
						switch (value)
						{
							case WindowCursors::None:  return Windows::WindowCursors::None;
							case WindowCursors::Arrow: return Windows::WindowCursors::Arrow;
							case WindowCursors::Beam:  return Windows::WindowCursors::Beam;
							case WindowCursors::Wait:  return Windows::WindowCursors::Wait;
							case WindowCursors::Hand:  return Windows::WindowCursors::Hand;
							case WindowCursors::Help:  return Windows::WindowCursors::Help;
						}

						throw NotImplementedException();
					}()
				);

				if (!Windows::Window::SetCursor(Move(cursor)))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void Native_SetBackgroundColor(WindowColor value) override
			{
				Windows::Window::SetBackgroundColor(
					value
				);
			}

			// @throw AL::Exception
			virtual Bool Native_SetResolution(WindowResolution::Type width, WindowResolution::Type height) override
			{
				if (!Windows::Window::SetResolution(width, height))
				{

					return False;
				}

				return True;
			}

			virtual Void Native_EnablePaint(Bool set) override
			{
				Windows::Window::EnablePaint(
					set
				);
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
			virtual Void OnIconChanged(const Windows::WindowIcon& icon, Windows::WindowIconTypes type) override
			{
				Windows::Window::OnIconChanged(
					icon,
					type
				);


			}

			// @throw AL::Exception
			// @return False to cancel
			virtual Bool OnIconChanging(const Windows::WindowIcon& icon, Windows::WindowIconTypes type) override
			{
				if (!Windows::Window::OnIconChanging(icon, type))
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

				lpWindow->OnResolutionChanged(
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

				if (lpWindow->OnMouseEvent(event))
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

				if (lpWindow->OnKeyboardEvent(event))
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
		Window(String&& name, String&& title)
			: lpNativeWindow(
				new NativeWindow(
					*this,
					Move(name),
					Move(title)
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

		Bool IsPaintEnabled() const
		{
			return lpNativeWindow->Native_IsPaintEnabled();
		}

		Bool IsContentLoaded() const
		{
			return lpNativeWindow->Native_IsContentLoaded();
		}

		auto& GetTitle() const
		{
			return lpNativeWindow->Native_GetTitle();
		}

		auto GetHandle() const
		{
			return lpNativeWindow->Native_GetHandle();
		}

		auto GetCursor() const
		{
			return lpNativeWindow->Native_GetCursor();
		}

		auto& GetPosition() const
		{
			return lpNativeWindow->Native_GetPosition();
		}

		auto& GetResolution() const
		{
			return lpNativeWindow->Native_GetResolution();
		}

		auto GetBackgroundColor() const
		{
			return lpNativeWindow->Native_GetBackgroundColor();
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

		// @throw AL::Exception
		Bool SetTitle(String&& value)
		{
			if (!lpNativeWindow->Native_SetTitle(Move(value)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetTitle(const String& value)
		{
			String text(
				value
			);

			if (!SetTitle(Move(text)))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetCursor(WindowCursors value)
		{
			if (!lpNativeWindow->Native_SetCursor(value))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetResolution(const WindowResolution& value)
		{
			if (!SetResolution(value.Width, value.Height))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetResolution(WindowResolution::Type width, WindowResolution::Type height)
		{
			if (!lpNativeWindow->Native_SetResolution(width, height))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Void SetBackgroundColor(WindowColor value)
		{
			lpNativeWindow->Native_SetBackgroundColor(
				value
			);
		}

		Void EnablePaint(Bool set = True)
		{
			lpNativeWindow->Native_EnablePaint(
				set
			);
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

		// @throw AL::Exception
		virtual Void OnResolutionChanged(WindowResolution::Type width, WindowResolution::Type height)
		{
		}
	};
}
