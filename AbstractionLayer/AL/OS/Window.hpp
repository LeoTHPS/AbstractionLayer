#pragma once
#include "AL/Common.hpp"

#include "AL/Input/MouseState.hpp"
#include "AL/Input/KeyboardState.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::OS
{
	enum class WindowIcons
	{
		None,
		Custom,

		Error,
		Shield,
		Default,
		Warning,
		Asterisk,
		Question
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
	
	typedef Graphics::Rectangle<uint32> WindowBounds;
	typedef Graphics::Vector2<uint32>   WindowPosition;
	typedef Graphics::Vector2<uint32>   WindowResolution;

	class Window
	{
		bool isOpen = false;
		bool isCreated = false;
		bool isClosing = false;
		bool isCloseCancelled = false;

		bool isMovable = true;
		bool isClosable = true;
		bool isResizable = true;
		bool isMinimizable = true;
		bool isMaximizable = true;
		bool isFileDropEnabled = false;

		bool isFocus = false;
		bool isMoving = false;
		bool isTopMost = false;
		bool isResizing = false;
		bool isMinimized = false;
		bool isMaximized = false;
		bool isDefaultPosition = true;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		HWND handle;
		String wClassName;
		WNDCLASSEXA wClass;
#endif

		String title;
		WindowCursors cursor = WindowCursors::Arrow;

		WindowPosition position;
		WindowResolution resolution;

		Graphics::Color backgroundColor = Graphics::Colors::CornflowerBlue;

		Input::MouseState mouseState;
		Input::KeyboardState keyboardState;

		Input::MouseEvent lastMouseEvent;
		Input::KeyboardEvent lastKeyboardEvent;

		Window(Window&&) = delete;
		Window(const Window&) = delete;

	public:
		Window()
		{
		}

		virtual ~Window()
		{
			if (IsOpen())
			{
				Close();
			}
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		bool IsCreated() const
		{
			return isCreated;
		}

		bool IsFocus() const
		{
			return isFocus;
		}

		bool IsTopMost() const
		{
			return isTopMost;
		}

		bool IsMoving() const
		{
			return isMoving;
		}

		bool IsMovable() const
		{
			return isMovable;
		}

		bool IsClosing() const
		{
			return isClosing;
		}

		bool IsClosable() const
		{
			return isClosable;
		}

		bool IsResizing() const
		{
			return isResizing;
		}

		bool IsResizable() const
		{
			return isResizable;
		}

		bool IsMinimized() const
		{
			return isMinimized;
		}

		bool IsMaximized() const
		{
			return isMaximized;
		}

		bool IsMinimizable() const
		{
			return isMinimizable;
		}

		bool IsMaximizable() const
		{
			return isMaximizable;
		}

		bool IsFileDropEnabled() const
		{
			return isFileDropEnabled;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return handle;
#endif
		}

		auto& GetTitle() const
		{
			return title;
		}

		auto GetCursor() const
		{
			return cursor;
		}

		auto& GetPosition() const
		{
			return position;
		}

		auto& GetResolution() const
		{
			return resolution;
		}

		auto GetBackgroundColor() const
		{
			return backgroundColor;
		}

		// @throw AL::Exceptions::Exception
		void SetTitle(String&& value)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}
			else
			{
				title = Move(
					value
				);
			}
		}
		// @throw AL::Exceptions::Exception
		void SetTitle(const String& value)
		{
			SetTitle(
				String(value)
			);
		}

#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		void SetIcon(HICON hIcon)
		{
			if (IsCreated())
			{
				PostNativeMessage(
					WM_SETICON,
					ICON_BIG,
					reinterpret_cast<LPARAM>(
						hIcon
					)
				);

				PostNativeMessage(
					WM_SETICON,
					ICON_SMALL,
					reinterpret_cast<LPARAM>(
						hIcon
					)
				);
			}
			else
			{
				wClass.hIcon = hIcon;
				wClass.hIconSm = hIcon;
			}
		}
#endif
		// @throw AL::Exceptions::Exception
		void SetIcon(WindowIcons value)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			switch (value)
			{
				case WindowIcons::None:
					return SetIcon(nullptr);

				case WindowIcons::Custom:
					throw Exceptions::NotImplementedException();

				case WindowIcons::Default:
					return SetIcon(LoadIcon(NULL, IDI_APPLICATION));

				case WindowIcons::Asterisk:
					return SetIcon(LoadIcon(NULL, IDI_ASTERISK));

				case WindowIcons::Error:
					return SetIcon(LoadIcon(NULL, IDI_ERROR));

				case WindowIcons::Question:
					return SetIcon(LoadIcon(NULL, IDI_QUESTION));

				case WindowIcons::Shield:
					return SetIcon(LoadIcon(NULL, IDI_SHIELD));

				case WindowIcons::Warning:
					return SetIcon(LoadIcon(NULL, IDI_WARNING));
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		void SetCursor(WindowCursors value)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			cursor = value;
		}

		// @throw AL::Exceptions::Exception
		// @return false if not movable
		bool SetPosition(uint32 x, uint32 y)
		{
			if (!IsMovable())
			{

				return false;
			}

			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}
			else
			{
				position.X = x;
				position.Y = y;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		void SetPosition(const WindowPosition& value)
		{
			SetPosition(
				value.X,
				value.Y
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if not resizable
		bool SetResolution(uint32 width, uint32 height)
		{
			if (!IsResizable())
			{

				return false;
			}

			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}
			else
			{
				resolution.X = width;
				resolution.Y = height;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if not resizable
		bool SetResolution(const Graphics::Rectangle<uint32>& value)
		{
			return SetResolution(
				value.GetWidth(),
				value.GetHeight()
			);
		}

		// @throw AL::Exceptions::Exception
		void SetBackgroundColor(Graphics::Color value)
		{
			backgroundColor = value;

			if (IsCreated())
			{

				Paint();
			}
		}

		// @throw AL::Exceptions::Exception
		void EnableMove(bool set = true)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			isMovable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableResize(bool set = true)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			isResizable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableMinimize(bool set = true)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			isMinimizable = set;
		}
		
		// @throw AL::Exceptions::Exception
		void EnableMaximize(bool set = true)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			isMaximizable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableFileDrop(bool set = true)
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}

			isFileDropEnabled = set;
		}

		// @throw AL::Exceptions::Exception
		void Open(uint32 tickRate = 40)
		{
			AL_ASSERT(!IsOpen(), "Window already open");

			try
			{
				Create();
			}
			catch (Exceptions::Exception&)
			{

				throw;
			}

			isOpen = true;

			try
			{
				Run(
					tickRate
				);
			}
			catch (Exceptions::Exception&)
			{
				Destroy();

				isOpen = false;

				throw;
			}

			Destroy();

			isOpen = false;
		}

		// @return false if block && !OnClosing()
		bool Close(bool block = true)
		{
			if (IsOpen())
			{
				bool wasClosing;

				if (!(wasClosing = IsClosing()))
				{
					isClosing = true;
					isCloseCancelled = false;
				}

				if (block)
				{
					TimeSpan time(
						TimeSpan::FromMilliseconds(10)
					);

					while (IsOpen())
					{
						if (!wasClosing && !IsClosing() && isCloseCancelled)
						{

							return false;
						}

						Sleep(time);
					}
				}
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		void Paint()
		{
			auto& resolution = GetResolution();

			WindowBounds bounds(
				resolution.X,
				resolution.Y
			);

			Paint(
				bounds
			);
		}
		// @throw AL::Exceptions::Exception
		void Paint(const WindowBounds& bounds)
		{
			Paint(
				bounds.Left,
				bounds.Top,
				bounds.GetWidth(),
				bounds.GetHeight()
			);
		}
		// @throw AL::Exceptions::Exception
		void Paint(uint32 x, uint32 y, uint32 width, uint32 height)
		{
			AL_ASSERT(IsCreated(), "Window not created");
			
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			RECT rect;
			rect.left = x;
			rect.right = x + width;
			rect.top = y;
			rect.bottom = y + height;

			if (!RedrawWindow(GetHandle(), &rect, nullptr, RDW_INVALIDATE | RDW_NOCHILDREN))
			{
				
				throw Exceptions::SystemException(
					"RedrawWindow"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		void Focus(bool set = true)
		{
			AL_ASSERT(IsCreated(), "Window not created");

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif
		}

		// @throw AL::Exceptions::Exception
		// @return false if not minimizable
		bool Minimize(bool set = true)
		{
			AL_ASSERT(IsCreated(), "Window not created");

			if (!IsMinimizable())
			{

				return false;
			}


#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false if not maximizable
		bool Maximize(bool set = true)
		{
			AL_ASSERT(IsCreated(), "Window not created");

			if (!IsMaximizable())
			{

				return false;
			}

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)

#endif

			return true;
		}

#if defined(AL_PLATFORM_LINUX)
		// @throw AL::Exceptions::Exception
		void PostNativeMessage(int message, uint32 arg32, uint64 arg64)
		{
			AL_ASSERT(IsCreated(), "Window not created");

			throw Exceptions::NotImplementedException();
		}
#elif defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		void PostNativeMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{
			AL_ASSERT(IsCreated(), "Window not created");

			if (!PostMessage(GetHandle(), message, wParam, lParam))
			{

				throw Exceptions::SystemException(
					"PostMessage"
				);
			}
		}
#endif

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnCreate()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif
		}

		virtual void OnDestroy()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif
		}

		// @throw AL::Exceptions::Exception
		virtual void OnOpen()
		{
		}

		virtual void OnClose()
		{
		}

		// @return false to cancel
		virtual bool OnClosing()
		{

			return IsClosable();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnDraw(float delta)
		{
		}

		// @throw AL::Exceptions::Exception
		virtual void OnEndDraw(float delta)
		{
		}

		// @throw AL::Exceptions::Exception
		virtual void OnUpdate(TimeSpan delta)
		{
		}

		// @throw AL::Exceptions::Exception
		virtual void OnFileDrop(const String& path)
		{
		}

		// @throw AL::Exceptions::Exception
		// @return false to prevent move
		virtual bool OnPositionChanging()
		{

			return IsMovable();
		}
		
		// @throw AL::Exceptions::Exception
		virtual void OnPositionChanged(const WindowPosition& position)
		{
		}

		// @throw AL::Exceptions::Exception
		// @return false to prevent resize
		virtual bool OnResolutionChanging()
		{

			return IsResizable();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnResolutionChanged(const WindowResolution& size)
		{
		}

#if defined(AL_PLATFORM_LINUX)
		// @throw AL::Exceptions::Exception
		virtual bool HandleNativeMessage(int message, uint32 arg32, uint64 arg64)
		{

			return true;
		}
#elif defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		virtual LRESULT HandleNativeMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto UpdateMousePosition = [this](LPARAM _lParam)
			{
				auto points = MAKEPOINTS(_lParam);

				lastMouseEvent.Position.X = points.x;
				lastMouseEvent.Position.Y = points.y;
			};

			switch (message)
			{
				case WM_CHAR:
				{
					if (isprint(static_cast<int>(wParam)))
					{
						lastKeyboardEvent.Event = Input::KeyboardEvents::Char;
						lastKeyboardEvent.Char = static_cast<char>(wParam);
					}
				}
				break;

				case WM_KEYDOWN:
				{
					lastKeyboardEvent.Key = static_cast<Input::Keys>(wParam);
					lastKeyboardEvent.Event = Input::KeyboardEvents::KeyDown;

					// handle generic alt
					if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_MENU))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							true
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					// handle generic shift
					else if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_SHIFT))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							true
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					// handle generic control
					else if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_CONTROL))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							true
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					keyboardState.SetPressed(
						lastKeyboardEvent.Key,
						true
					);
				}
				break;

				case WM_KEYUP:
				{
					lastKeyboardEvent.Key = static_cast<Input::Keys>(wParam);
					lastKeyboardEvent.Event = Input::KeyboardEvents::KeyUp;

					// handle generic alt
					if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_MENU))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							false
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					// handle generic shift
					else if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_SHIFT))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							false
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					// handle generic control
					else if (lastKeyboardEvent.Key == static_cast<Input::Keys>(VK_CONTROL))
					{
						keyboardState.SetPressed(
							lastKeyboardEvent.Key,
							false
						);

						lastKeyboardEvent.Key = static_cast<Input::Keys>(
							MapVirtualKey(
								static_cast<UINT>((lParam & 0x00FF0000) >> 16),
								MAPVK_VSC_TO_VK_EX
							)
						);
					}

					keyboardState.SetPressed(
						lastKeyboardEvent.Key,
						false
					);
				}
				break;

				case WM_MOUSEMOVE:
				{
					UpdateMousePosition(lParam);

					lastMouseEvent.Event = Input::MouseEvents::Move;
				}
				break;

				case WM_MOUSEWHEEL:
				{
					auto delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

					// scroll up
					if (delta > 0)
					{
						lastMouseEvent.Event = Input::MouseEvents::ScrollUp;
					}

					// scroll down
					else if (delta < 0)
					{
						lastMouseEvent.Event = Input::MouseEvents::ScrollDown;
					}
				}
				break;

				// left

				case WM_LBUTTONDOWN:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Left, true);

					lastMouseEvent.Event = Input::MouseEvents::ButtonDown;
					lastMouseEvent.Button = Input::MouseButtons::Left;
				}
				break;

				case WM_LBUTTONUP:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Left, false);

					lastMouseEvent.Event = Input::MouseEvents::ButtonUp;
					lastMouseEvent.Button = Input::MouseButtons::Left;
				}
				break;

				// right

				case WM_RBUTTONDOWN:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Right, true);

					lastMouseEvent.Event = Input::MouseEvents::ButtonDown;
					lastMouseEvent.Button = Input::MouseButtons::Right;
				}
				break;

				case WM_RBUTTONUP:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Right, false);

					lastMouseEvent.Event = Input::MouseEvents::ButtonUp;
					lastMouseEvent.Button = Input::MouseButtons::Right;
				}
				break;

				// middle

				case WM_MBUTTONDOWN:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Middle, true);

					lastMouseEvent.Event = Input::MouseEvents::ButtonDown;
					lastMouseEvent.Button = Input::MouseButtons::Middle;
				}
				break;

				case WM_MBUTTONUP:
				{
					UpdateMousePosition(lParam);

					mouseState.SetPressed(Input::MouseButtons::Middle, false);

					lastMouseEvent.Event = Input::MouseEvents::ButtonUp;
					lastMouseEvent.Button = Input::MouseButtons::Middle;
				}
				break;

				// x1/x2

				case WM_XBUTTONDOWN:
				{
					UpdateMousePosition(lParam);

					switch (GET_XBUTTON_WPARAM(wParam))
					{
						case XBUTTON1:
						{
							mouseState.SetPressed(Input::MouseButtons::X1, true);

							lastMouseEvent.Event = Input::MouseEvents::ButtonDown;
							lastMouseEvent.Button = Input::MouseButtons::X1;
						}
						break;

						case XBUTTON2:
						{
							mouseState.SetPressed(Input::MouseButtons::X2, true);

							lastMouseEvent.Event = Input::MouseEvents::ButtonDown;
							lastMouseEvent.Button = Input::MouseButtons::X2;
						}
						break;
					}
				}
				break;

				case WM_XBUTTONUP:
				{
					UpdateMousePosition(lParam);

					switch (GET_XBUTTON_WPARAM(wParam))
					{
						case XBUTTON1:
						{
							mouseState.SetPressed(Input::MouseButtons::X1, false);

							lastMouseEvent.Event = Input::MouseEvents::ButtonUp;
							lastMouseEvent.Button = Input::MouseButtons::X1;
						}
						break;

						case XBUTTON2:
						{
							mouseState.SetPressed(Input::MouseButtons::X2, false);

							lastMouseEvent.Event = Input::MouseEvents::ButtonUp;
							lastMouseEvent.Button = Input::MouseButtons::X2;
						}
						break;
					}
				}
				break;
			}

			return DefWindowProc(
				GetHandle(),
				message,
				wParam,
				lParam
			);
		}
#endif

	private:
		// @throw AL::Exceptions::Exception
		void Create()
		{
			AL_ASSERT(!IsCreated(), "Window already created");

			OnCreate();

			isCreated = true;
		}

		void Destroy()
		{
			if (IsCreated())
			{
				OnDestroy();

				isCreated = false;
			}
		}

		// @throw AL::Exceptions::Exception
		void Run(uint32 tickRate)
		{
			AL_ASSERT(IsCreated(), "Window not created");
			AL_ASSERT(IsOpen(), "Window not open");

			do
			{

			} while (IsOpen());
		}

		// @throw AL::Exceptions::Exception
		void Draw(float delta)
		{
			AL_ASSERT(IsOpen(), "Window not open");
			AL_ASSERT(IsCreated(), "Window not created");

			OnDraw(
				delta
			);

			OnEndDraw(
				delta
			);
		}

		// @throw AL::Exceptions::Exception
		void Update(TimeSpan delta)
		{
			AL_ASSERT(IsOpen(), "Window not open");
			AL_ASSERT(IsCreated(), "Window not created");

			OnUpdate(
				delta
			);
		}
	};
}
