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

	struct WindowMouseEvent
	{
		Hardware::MouseEvent Data;
		Bool                 Handled = False;
	};

	struct WindowKeyboardEvent
	{
		Hardware::KeyboardEvent Data;
		Bool                    Handled = False;
	};

	// @throw AL::Exception
	typedef EventHandler<Void()>                           WindowOnOpenEventHandler;

	typedef EventHandler<Void()>                           WindowOnCloseEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void()>                           WindowOnCreateEventHandler;

	typedef EventHandler<Void()>                           WindowOnDestroyEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void()>                           WindowOnLoadContentEventHandler;

	typedef EventHandler<Void()>                           WindowOnUnloadContentEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(WindowGraphics& graphics)>   WindowOnPaintEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(TimeSpan delta)>             WindowOnUpdateEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(WindowMouseEvent& event)>    WindowOnMouseEventEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(WindowKeyboardEvent& event)> WindowOnKeyboardEventEventHandler;

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
			: public INativeWindow
		{
			OS::Window* const   lpWindow;
			OS::Windows::Window nativeWindow;

		public:
			NativeWindow(OS::Window& window, String&& name)
				: lpWindow(
					&window
				),
				nativeWindow(
					Move(name)
				)
			{
				RegisterEventHandlers();
			}

			virtual Bool Native_IsOpen() const override
			{
				return nativeWindow.IsOpen();
			}

			virtual Bool Native_IsContentLoaded() const override
			{
				return nativeWindow.IsContentLoaded();
			}

			virtual ::HWND Native_GetHandle() const override
			{
				return nativeWindow.GetHandle();
			}

			// @throw AL::Exception
			virtual Void Native_Open() override
			{
				nativeWindow.Open();
			}

			// @throw AL::Exception
			virtual Void Native_Close() override
			{
				nativeWindow.Close();
			}

			// @throw AL::Exception
			virtual Void Native_LoadContent() override
			{
				nativeWindow.LoadContent();
			}

			virtual Void Native_UnloadContent() override
			{
				nativeWindow.UnloadContent();
			}

			// @throw AL::Exception
			// @return False on close
			virtual Bool Native_Update(TimeSpan delta) override
			{
				if (!nativeWindow.Update(delta))
				{

					return False;
				}

				return True;
			}

		private: // Event Handlers
			// @throw AL::Exception
			Void EventHandler_OnOpen()
			{
				lpWindow->OnOpen.Execute();
			}

			Void EventHandler_OnClose()
			{
				lpWindow->OnClose.Execute();
			}

			// @throw AL::Exception
			Void EventHandler_OnCreate()
			{
				lpWindow->OnCreate.Execute();
			}

			Void EventHandler_OnDestroy()
			{
				lpWindow->OnDestroy.Execute();
			}

			// @throw AL::Exception
			Void EventHandler_OnLoadContent()
			{
				lpWindow->OnLoadContent.Execute();
			}

			Void EventHandler_OnUnloadContent()
			{
				lpWindow->OnUnloadContent.Execute();
			}

			// @throw AL::Exception
			Void EventHandler_OnPaint(Windows::GDI::Device& device)
			{
				WindowGraphics graphics(
					Move(device)
				);

				lpWindow->OnPaint.Execute(
					graphics
				);
			}

			// @throw AL::Exception
			Void EventHandler_OnUpdate(TimeSpan delta)
			{
				lpWindow->OnUpdate.Execute(
					delta
				);
			}

			// @throw AL::Exception
			Void EventHandler_OnMouseEvent(Windows::WindowMouseEvent& event)
			{
				WindowMouseEvent _event =
				{
					.Data    = event.Data,
					.Handled = event.Handled
				};

				lpWindow->OnMouseEvent.Execute(
					_event
				);

				event.Handled = _event.Handled;
			}

			// @throw AL::Exception
			Void EventHandler_OnKeyboardEvent(Windows::WindowKeyboardEvent& event)
			{
				WindowKeyboardEvent _event =
				{
					.Data    = event.Data,
					.Handled = event.Handled
				};

				lpWindow->OnKeyboardEvent.Execute(
					_event
				);

				event.Handled = _event.Handled;
			}

		private:
			Void RegisterEventHandlers()
			{
				#define AL_OS_WINDOW_REGISTER_EVENT_HANDLER(__event__) \
					nativeWindow.__event__.Register<EventPriorities::Highest>( \
						Windows::Window##__event__##EventHandler( \
							&NativeWindow::EventHandler_##__event__, \
							*this \
						) \
					)

				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnOpen);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnClose);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnCreate);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnDestroy);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnLoadContent);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnUnloadContent);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnPaint);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnUpdate);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnMouseEvent);
				AL_OS_WINDOW_REGISTER_EVENT_HANDLER(OnKeyboardEvent);

				#undef AL_OS_WINDOW_REGISTER_EVENT_HANDLER
			}
		};
#endif

		INativeWindow* const lpNativeWindow;

		Window(Window&&) = delete;
		Window(const Window&) = delete;

	public:
		// @throw AL::Exception
		Event<WindowOnOpenEventHandler>          OnOpen;

		Event<WindowOnCloseEventHandler>         OnClose;

		// @throw AL::Exception
		Event<WindowOnCreateEventHandler>        OnCreate;

		Event<WindowOnDestroyEventHandler>       OnDestroy;

		// @throw AL::Exception
		Event<WindowOnLoadContentEventHandler>   OnLoadContent;

		Event<WindowOnUnloadContentEventHandler> OnUnloadContent;

		// @throw AL::Exception
		Event<WindowOnPaintEventHandler>         OnPaint;

		// @throw AL::Exception
		Event<WindowOnUpdateEventHandler>        OnUpdate;

		// @throw AL::Exception
		Event<WindowOnMouseEventEventHandler>    OnMouseEvent;

		// @throw AL::Exception
		Event<WindowOnKeyboardEventEventHandler> OnKeyboardEvent;

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
	};
}
