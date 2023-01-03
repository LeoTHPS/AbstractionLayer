#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Window.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Window.hpp"
#else
	#error Platform not supported
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

	enum class WindowIcons
	{
		None,
		Error,
		Shield,
		Default,
		Warning,
		Asterisk,
		Question,

		UserDefined
	};

	class WindowIcon
	{
		WindowIcons type;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Windows::WindowIcon::Handle    hIcon;
		Windows::WindowIcon::HInstance hInstance;
#endif

	public:
#if defined(AL_PLATFORM_LINUX)
		typedef void*                                   Handle;
		typedef void*                                   HInstance;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef typename Windows::WindowIcon::Handle    Handle;
		typedef typename Windows::WindowIcon::HInstance HInstance;
#endif

		// @throw AL::Exception
		static WindowIcon Load(uint32 id)
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto icon = Windows::WindowIcon::Load(
				id
			);

			return WindowIcon(
				icon.GetInstance(),
				icon.GetHandle()
			);
#endif
		}
		// @throw AL::Exception
		static WindowIcon Load(HInstance hInstance, uint32 id)
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto icon = Windows::WindowIcon::Load(
				hInstance,
				id
			);

			return WindowIcon(
				icon.GetInstance(),
				icon.GetHandle()
			);
#endif
		}

		WindowIcon(WindowIcons icon)
			: type(
				icon
			)
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			,
			hIcon(
				Handle()
			),
			hInstance(
				HInstance()
			)
#endif
		{
		}

		WindowIcon(HInstance hInstance, Handle handle)
			: type(
				WindowIcons::UserDefined
			)
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			,
			hIcon(
				handle
			),
			hInstance(
				hInstance
			)
#endif
		{
		}

		virtual ~WindowIcon()
		{
		}

		auto GetType() const
		{
			return type;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return hIcon;
#endif
		}

		auto GetInstance() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return hInstance;
#endif
		}
	};

	enum class WindowCursors
	{
		None,
		Arrow,
		Beam,
		Wait,
		Hand,
		Help,

		UserDefined
	};

	class WindowCursor
	{
		WindowCursors type;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Windows::WindowCursor::Handle    hCursor;
		Windows::WindowCursor::HInstance hInstance;
#endif

	public:
#if defined(AL_PLATFORM_LINUX)
		typedef void*                                     Handle;
		typedef void*                                     HInstance;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef typename Windows::WindowCursor::Handle    Handle;
		typedef typename Windows::WindowCursor::HInstance HInstance;
#endif

		// @throw AL::Exception
		static WindowCursor Load(uint32 id)
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto icon = Windows::WindowCursor::Load(
				id
			);

			return WindowCursor(
				icon.GetInstance(),
				icon.GetHandle()
			);
#endif
		}
		// @throw AL::Exception
		static WindowCursor Load(HInstance hInstance, uint32 id)
		{
#if defined(AL_PLATFORM_LINUX)
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto icon = Windows::WindowCursor::Load(
				hInstance,
				id
			);

			return WindowCursor(
				icon.GetInstance(),
				icon.GetHandle()
			);
#endif
		}

		WindowCursor(WindowCursors cursor)
			: type(
				cursor
			)
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			,
			hCursor(
				Handle()
			),
			hInstance(
				HInstance()
			)
#endif
		{
		}

		WindowCursor(HInstance hInstance, Handle handle)
			: type(
				WindowCursors::UserDefined
			)
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			,
			hCursor(
				handle
			),
			hInstance(
				hInstance
			)
#endif
		{
		}

		virtual ~WindowCursor()
		{
		}

		auto GetType() const
		{
			return type;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return hCursor;
#endif
		}

		auto GetInstance() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return hInstance;
#endif
		}
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

			virtual Bool Native_IsResizable() const = 0;

			virtual Bool Native_IsMinimizable() const = 0;

			virtual Bool Native_IsMaximizable() const = 0;

			virtual const WindowIcon& Native_GetIconBig() const = 0;

			virtual const WindowIcon& Native_GetIconSmall() const = 0;

			virtual const String& Native_GetTitle() const = 0;

#if defined(AL_PLATFORM_LINUX)
			virtual Void* Native_GetHandle() const = 0;
#elif defined(AL_PLATFORM_WINDOWS)
			virtual ::HWND Native_GetHandle() const = 0;
#endif

			virtual const WindowCursor& Native_GetCursor() const = 0;

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
			// @return AL::False on close
			virtual Bool Native_Update(TimeSpan delta) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetIconBig(WindowIcon&& value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetIconSmall(WindowIcon&& value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetTitle(String&& value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetCursor(WindowCursor&& value) = 0;

			// @throw AL::Exception
			virtual Void Native_SetBackgroundColor(WindowColor value) = 0;

			// @throw AL::Exception
			virtual Bool Native_SetResolution(WindowResolution::Type width, WindowResolution::Type height) = 0;

			virtual Void Native_EnablePaint(Bool set) = 0;

			// @throw AL::Exception
			virtual Void Native_EnableResize(Bool set = True) = 0;

			// @throw AL::Exception
			virtual Void Native_EnableMinimize(Bool set = True) = 0;

			// @throw AL::Exception
			virtual Void Native_EnableMaximize(Bool set = True) = 0;
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

			virtual Bool Native_IsResizable() const override
			{
				throw NotImplementedException();
			}

			virtual Bool Native_IsMinimizable() const override
			{
				throw NotImplementedException();
			}

			virtual Bool Native_IsMaximizable() const override
			{
				throw NotImplementedException();
			}

			virtual const WindowIcon& Native_GetIconBig() const override
			{
				throw NotImplementedException();
			}

			virtual const WindowIcon& Native_GetIconSmall() const override
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

			virtual const WindowCursor& Native_GetCursor() const override
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
			// @return AL::False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetIconBig(WindowIcon&& value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetIconSmall(WindowIcon&& value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetTitle(String&& value) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Bool Native_SetCursor(WindowCursor&& value) override
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

			// @throw AL::Exception
			virtual Void Native_EnableResize(Bool set = True) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_EnableMinimize(Bool set = True) override
			{
				throw NotImplementedException();
			}

			// @throw AL::Exception
			virtual Void Native_EnableMaximize(Bool set = True) override
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

			WindowIcon   iconBig;
			WindowIcon   iconSmall;

			WindowCursor cursor;

		public:
			NativeWindow(OS::Window& window, String&& name, String&& title)
				: Window(
					Move(name),
					Move(title)
				),
				lpWindow(
					&window
				),
				iconBig(
					Window::GetIconBig().GetInstance(),
					Window::GetIconBig().GetHandle()
				),
				iconSmall(
					Window::GetIconSmall().GetInstance(),
					Window::GetIconSmall().GetHandle()
				),
				cursor(
					Window::GetCursor().GetInstance(),
					Window::GetCursor().GetHandle()
				)
			{
			}

			virtual Bool Native_IsOpen() const override
			{
				return Window::IsOpen();
			}

			virtual Bool Native_IsPaintEnabled() const override
			{
				return Window::IsPaintEnabled();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				return Window::IsContentLoaded();
			}

			virtual Bool Native_IsResizable() const override
			{
				return lpWindow->IsResizable();
			}

			virtual Bool Native_IsMinimizable() const override
			{
				return lpWindow->IsMinimizable();
			}

			virtual Bool Native_IsMaximizable() const override
			{
				return lpWindow->IsMaximizable();
			}

			virtual const WindowIcon& Native_GetIconBig() const override
			{
				return iconBig;
			}

			virtual const WindowIcon& Native_GetIconSmall() const override
			{
				return iconSmall;
			}

			virtual const String& Native_GetTitle() const override
			{
				return Window::GetTitle();
			}

			virtual ::HWND Native_GetHandle() const override
			{
				return Window::GetHandle();
			}

			virtual const WindowCursor& Native_GetCursor() const override
			{
				return cursor;
			}

			virtual const WindowPosition& Native_GetPosition() const override
			{
				return Window::GetPosition();
			}

			virtual const WindowResolution& Native_GetResolution() const override
			{
				return Window::GetResolution();
			}

			// @throw AL::Exception
			virtual WindowColor Native_GetBackgroundColor() const override
			{
				return Window::GetBackgroundColor();
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
			// @return AL::False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				if (!Window::Update(delta))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Bool Native_SetIconBig(WindowIcon&& value) override
			{
				if (value.GetType() == WindowIcons::UserDefined)
				{
					Windows::WindowIcon icon(
						value.GetInstance(),
						value.GetHandle()
					);

					if (!Window::SetIcon(Move(icon), Windows::WindowIconTypes::Big))
					{

						return False;
					}
				}
				else
				{
					Windows::WindowIcon icon(
						ToNativeIcon(value.GetType())
					);

					if (!Window::SetIcon(Move(icon), Windows::WindowIconTypes::Big))
					{

						return False;
					}
				}

				return True;
			}

			// @throw AL::Exception
			virtual Bool Native_SetIconSmall(WindowIcon&& value) override
			{
				if (value.GetType() == WindowIcons::UserDefined)
				{
					Windows::WindowIcon icon(
						value.GetInstance(),
						value.GetHandle()
					);

					if (!Window::SetIcon(Move(icon), Windows::WindowIconTypes::Small))
					{

						return False;
					}
				}
				else
				{
					Windows::WindowIcon icon(
						ToNativeIcon(value.GetType())
					);

					if (!Window::SetIcon(Move(icon), Windows::WindowIconTypes::Small))
					{

						return False;
					}
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
			virtual Bool Native_SetCursor(WindowCursor&& value) override
			{
				if (value.GetType() == WindowCursors::UserDefined)
				{
					Windows::WindowCursor cursor(
						value.GetInstance(),
						value.GetHandle()
					);

					if (!Window::SetCursor(Move(cursor)))
					{

						return False;
					}
				}
				else
				{
					Windows::WindowCursor cursor(
						ToNativeCursor(value.GetType())
					);

					if (!Window::SetCursor(Move(cursor)))
					{

						return False;
					}
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void Native_SetBackgroundColor(WindowColor value) override
			{
				Window::SetBackgroundColor(
					value
				);
			}

			// @throw AL::Exception
			virtual Bool Native_SetResolution(WindowResolution::Type width, WindowResolution::Type height) override
			{
				if (!Window::SetResolution(width, height))
				{

					return False;
				}

				return True;
			}

			virtual Void Native_EnablePaint(Bool set) override
			{
				Window::EnablePaint(
					set
				);
			}

			// @throw AL::Exception
			virtual Void Native_EnableResize(Bool set = True) override
			{
				Window::EnableResize(
					set
				);
			}

			// @throw AL::Exception
			virtual Void Native_EnableMinimize(Bool set = True) override
			{
				Window::EnableMinimize(
					set
				);
			}

			// @throw AL::Exception
			virtual Void Native_EnableMaximize(Bool set = True) override
			{
				Window::EnableMaximize(
					set
				);
			}

		protected:
			// @throw AL::Exception
			virtual Void OnCreate() override
			{
				Window::OnCreate();

				try
				{
					lpWindow->OnCreate();
				}
				catch (Exception& exception)
				{
					Window::OnDestroy();

					throw;
				}
			}

			virtual Void OnDestroy() override
			{
				lpWindow->OnDestroy();

				Window::OnDestroy();
			}

			// @throw AL::Exception
			virtual Void OnLoadContent() override
			{
				Window::OnLoadContent();

				try
				{
					lpWindow->OnLoadContent();
				}
				catch (Exception& exception)
				{
					Window::OnUnloadContent();

					throw;
				}
			}

			virtual Void OnUnloadContent() override
			{
				lpWindow->OnUnloadContent();

				Window::OnUnloadContent();
			}

			// @throw AL::Exception
			virtual Void OnOpen() override
			{
				Window::OnOpen();

				try
				{
					lpWindow->OnOpen();
				}
				catch (Exception& exception)
				{
					Window::OnClose();

					throw;
				}
			}

			// @return AL::False to cancel
			virtual Bool OnClosing() override
			{
				if (!Window::OnClosing())
				{

					return False;
				}

				return True;
			}

			virtual Void OnClose() override
			{
				lpWindow->OnClose();

				Window::OnClose();
			}

			// @throw AL::Exception
			virtual Void OnPaint(Windows::GDI::Device& device) override
			{
				Window::OnPaint(
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
				Window::OnUpdate(
					delta
				);

				lpWindow->OnUpdate(
					delta
				);
			}

			// @throw AL::Exception
			virtual Void OnFileDrop(const String& path) override
			{
				Window::OnFileDrop(
					path
				);


			}

			// @throw AL::Exception
			virtual Void OnIconChanged(const Windows::WindowIcon& icon, Windows::WindowIconTypes type) override
			{
				Window::OnIconChanged(
					icon,
					type
				);

				switch (type)
				{
					case Windows::WindowIconTypes::Big:
					{
						if (icon.GetType() == Windows::WindowIcons::UserDefined)
						{
							iconBig = WindowIcon(
								icon.GetInstance(),
								icon.GetHandle()
							);
						}
						else
						{
							iconBig = WindowIcon(
								FromNativeIcon(icon.GetType())
							);
						}
					}
					break;

					case Windows::WindowIconTypes::Small:
					{
						if (icon.GetType() == Windows::WindowIcons::UserDefined)
						{
							iconSmall = WindowIcon(
								icon.GetInstance(),
								icon.GetHandle()
							);
						}
						else
						{
							iconSmall = WindowIcon(
								FromNativeIcon(icon.GetType())
							);
						}
					}
					break;
				}
			}

			// @throw AL::Exception
			// @return AL::False to cancel
			virtual Bool OnIconChanging(const Windows::WindowIcon& icon, Windows::WindowIconTypes type) override
			{
				if (!Window::OnIconChanging(icon, type))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnTitleChanged(const String& title) override
			{
				Window::OnTitleChanged(
					title
				);


			}

			// @throw AL::Exception
			// @return AL::False to cancel
			virtual Bool OnTitleChanging(const String& title) override
			{
				if (!Window::OnTitleChanging(title))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnCursorChanged(const Windows::WindowCursor& cursor) override
			{
				Window::OnCursorChanged(
					cursor
				);

				if (cursor.GetType() == Windows::WindowCursors::UserDefined)
				{
					this->cursor = WindowCursor(
						cursor.GetInstance(),
						cursor.GetHandle()
					);
				}
				else
				{
					this->cursor = WindowCursor(
						FromNativeCursor(cursor.GetType())
					);
				}
			}

			// @throw AL::Exception
			// @return AL::False to cancel
			virtual Bool OnCursorChanging(const Windows::WindowCursor& cursor) override
			{
				if (!Window::OnCursorChanging(cursor))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnPositionChanged(Windows::WindowPosition::Type x, Windows::WindowPosition::Type y) override
			{
				Window::OnPositionChanged(
					x,
					y
				);


			}

			// @throw AL::Exception
			// @return AL::False to cancel
			virtual Bool OnPositionChanging(Windows::WindowPosition::Type x, Windows::WindowPosition::Type y) override
			{
				if (!Window::OnPositionChanging(x, y))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			virtual Void OnResolutionChanged(Windows::WindowResolution::Type width, Windows::WindowResolution::Type height) override
			{
				Window::OnResolutionChanged(
					width,
					height
				);

				lpWindow->OnResolutionChanged(
					width,
					height
				);
			}

			// @throw AL::Exception
			// @return AL::False to cancel
			virtual Bool OnResolutionChanging(Windows::WindowResolution::Type width, Windows::WindowResolution::Type height) override
			{
				if (!Window::OnResolutionChanging(width, height))
				{

					return False;
				}

				return True;
			}

			// @throw AL::Exception
			// @return AL::True if handled
			virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
			{
				if (Window::OnMouseEvent(event))
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
			// @return AL::True if handled
			virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
			{
				if (Window::OnKeyboardEvent(event))
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
				Window::OnClipboardDataChanged(
					format
				);


			}

		private:
			static Windows::WindowIcons   ToNativeIcon(WindowIcons value)
			{
				switch (value)
				{
					case WindowIcons::None:        return Windows::WindowIcons::None;
					case WindowIcons::Error:       return Windows::WindowIcons::Error;
					case WindowIcons::Shield:      return Windows::WindowIcons::Shield;
					case WindowIcons::Default:     return Windows::WindowIcons::Default;
					case WindowIcons::Warning:     return Windows::WindowIcons::Warning;
					case WindowIcons::Asterisk:    return Windows::WindowIcons::Asterisk;
					case WindowIcons::Question:    return Windows::WindowIcons::Question;
					case WindowIcons::UserDefined: return Windows::WindowIcons::UserDefined;
				}

				return Windows::WindowIcons::UserDefined;
			}
			static Windows::WindowCursors ToNativeCursor(WindowCursors value)
			{
				switch (value)
				{
					case WindowCursors::None:        return Windows::WindowCursors::None;
					case WindowCursors::Arrow:       return Windows::WindowCursors::Arrow;
					case WindowCursors::Beam:        return Windows::WindowCursors::Beam;
					case WindowCursors::Wait:        return Windows::WindowCursors::Wait;
					case WindowCursors::Hand:        return Windows::WindowCursors::Hand;
					case WindowCursors::Help:        return Windows::WindowCursors::Help;
					case WindowCursors::UserDefined: return Windows::WindowCursors::UserDefined;
				}

				return Windows::WindowCursors::UserDefined;
			}

			static WindowIcons   FromNativeIcon(Windows::WindowIcons value)
			{
				switch (value)
				{
					case Windows::WindowIcons::None:        return WindowIcons::None;
					case Windows::WindowIcons::Error:       return WindowIcons::Error;
					case Windows::WindowIcons::Shield:      return WindowIcons::Shield;
					case Windows::WindowIcons::Default:     return WindowIcons::Default;
					case Windows::WindowIcons::Warning:     return WindowIcons::Warning;
					case Windows::WindowIcons::Asterisk:    return WindowIcons::Asterisk;
					case Windows::WindowIcons::Question:    return WindowIcons::Question;
					case Windows::WindowIcons::UserDefined: return WindowIcons::UserDefined;
				}

				return WindowIcons::UserDefined;
			}
			static WindowCursors FromNativeCursor(Windows::WindowCursors value)
			{
				switch (value)
				{
					case Windows::WindowCursors::None:        return WindowCursors::None;
					case Windows::WindowCursors::Arrow:       return WindowCursors::Arrow;
					case Windows::WindowCursors::Beam:        return WindowCursors::Beam;
					case Windows::WindowCursors::Wait:        return WindowCursors::Wait;
					case Windows::WindowCursors::Hand:        return WindowCursors::Hand;
					case Windows::WindowCursors::Help:        return WindowCursors::Help;
					case Windows::WindowCursors::UserDefined: return WindowCursors::UserDefined;
				}

				return WindowCursors::UserDefined;
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

		Bool IsResizable() const
		{
			return lpNativeWindow->Native_IsResizable();
		}

		Bool IsMinimizable() const
		{
			return lpNativeWindow->Native_IsMinimizable();
		}

		Bool IsMaximizable() const
		{
			return lpNativeWindow->Native_IsMaximizable();
		}

		auto& GetIconBig() const
		{
			return lpNativeWindow->Native_GetIconBig();
		}

		auto& GetIconSmall() const
		{
			return lpNativeWindow->Native_GetIconSmall();
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
		// @return AL::False on close
		Bool Update(TimeSpan delta)
		{
			if (!lpNativeWindow->Native_Update(delta))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetIconBig(WindowIcon&& value)
		{
			if (!lpNativeWindow->Native_SetIconBig(Move(value)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetIconBig(const WindowIcon& value)
		{
			WindowIcon icon(
				value
			);

			if (!SetIconBig(Move(icon)))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetIconSmall(WindowIcon&& value)
		{
			if (!lpNativeWindow->Native_SetIconSmall(Move(value)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetIconSmall(const WindowIcon& value)
		{
			WindowIcon icon(
				value
			);

			if (!SetIconSmall(Move(icon)))
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
		Bool SetCursor(WindowCursor&& value)
		{
			if (!lpNativeWindow->Native_SetCursor(Move(value)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetCursor(const WindowCursor& value)
		{
			WindowCursor cursor(
				value
			);

			if (!SetCursor(Move(cursor)))
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

		// @throw AL::Exception
		Void EnableResize(Bool set = True)
		{
			lpNativeWindow->Native_EnableResize(
				set
			);
		}

		// @throw AL::Exception
		Void EnableMinimize(Bool set = True)
		{
			lpNativeWindow->Native_EnableMinimize(
				set
			);
		}

		// @throw AL::Exception
		Void EnableMaximize(Bool set = True)
		{
			lpNativeWindow->Native_EnableMaximize(
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
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event)
		{

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
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
