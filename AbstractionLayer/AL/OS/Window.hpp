#pragma once
#include "AL/Common.hpp"

#include "AL/Input/MouseState.hpp"
#include "AL/Input/KeyboardState.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <shellapi.h>
#endif

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
		bool isCreating = false;
		bool isClosing = false;
		bool isForceClosing = false;
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

		// this variable is set true by Close(true)
		// if Close(false) is called then Destroy() will be invoked on the last window update
		// in windows builds this last update is from WM_CLOSE, linux isn't implemented
		bool destroyOnClose = false;
		
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		HWND handle;
		WNDCLASSEXA wClass;
#endif

		String title;
		String className;
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
		Window(String&& title, String&& className)
			: title(
				Move(title)
			),
			className(
				Move(className)
			),
			resolution(
				1366,
				768
			)
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			ZeroMemory(&wClass, sizeof(wClass));
			wClass.cbSize = sizeof(wClass);
			wClass.hIcon = LoadNativeIcon(WindowIcons::Default);
			wClass.hIconSm = LoadNativeIcon(WindowIcons::Default);
			//wClass.hCursor = LoadNativeCursor(WindowCursors::Arrow);
			wClass.hInstance = GetModuleHandle(nullptr);
			wClass.lpfnWndProc = &Window::NativeWindowProc;
			wClass.lpszClassName = this->className.GetCString();
#endif

			SetIcon(
				WindowIcons::Default
			);

			SetCursor(
				WindowCursors::Arrow
			);

			SetBackgroundColor(
				Graphics::Colors::White
			);
		}

		virtual ~Window()
		{
			if (IsOpen())
			{
				Close();
			}

#if defined(AL_PLATFORM_WINDOWS)
			if (auto hBackground = wClass.hbrBackground)
			{
				DeleteObject(
					hBackground
				);
			}
#endif
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
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				if (!SetWindowTextA(GetHandle(), value.GetCString()))
				{

					throw Exceptions::SystemException(
						"SetWindowTextA"
					);
				}
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
			if (IsCreated() || isCreating)
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
				case WindowIcons::Custom:
					throw Exceptions::NotImplementedException();

				case WindowIcons::None:
				case WindowIcons::Default:
				case WindowIcons::Asterisk:
				case WindowIcons::Error:
				case WindowIcons::Question:
				case WindowIcons::Shield:
				case WindowIcons::Warning:
					return SetIcon(LoadNativeIcon(value));
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		void SetCursor(WindowCursors value)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				wClass.hCursor = LoadNativeCursor(
					value
				);

				::SetCursor(
					wClass.hCursor
				);
#endif
			}

			cursor = value;
		}

		// @throw AL::Exceptions::Exception
		void SetTopMost(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				if (!SetWindowPos(GetHandle(), set ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
				{

					throw Exceptions::SystemException(
						"SetWindowPos"
					);
				}
#endif
			}
			else
			{
				isTopMost = set;
			}
		}

		// @throw AL::Exceptions::Exception
		// @return false if not movable
		bool SetPosition(uint32 x, uint32 y)
		{
			if (!IsMovable())
			{

				return false;
			}

			isDefaultPosition = false;

			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				RECT rect;

				if (!GetWindowRect(GetHandle(), &rect))
				{

					throw Exceptions::SystemException(
						"GetWindowRect"
					);
				}

				if (!MoveWindow(GetHandle(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), TRUE))
				{

					throw Exceptions::SystemException(
						"MoveWindow"
					);
				}
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

			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				RECT currentRect;

				if (!GetWindowRect(GetHandle(), &currentRect))
				{

					throw Exceptions::SystemException(
						"GetWindowRect"
					);
				}

				RECT rect;
				rect.left = currentRect.left;
				rect.right = rect.left + static_cast<LONG>(width);
				rect.top = currentRect.top;
				rect.bottom = rect.top + static_cast<LONG>(height);

				if (!MoveWindow(GetHandle(), static_cast<int>(rect.left), static_cast<int>(rect.top), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), TRUE))
				{

					throw Exceptions::SystemException(
						"MoveWindow"
					);
				}
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
		void EnableClose(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				if (EnableMenuItem(GetSystemMenu(GetHandle(), FALSE), SC_CLOSE, set ? MF_ENABLED : MF_GRAYED) == -1)
				{

					throw Exceptions::SystemException(
						"EnableMenuItem"
					);
				}
#endif
			}

			isClosable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableResize(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_THICKFRAME,
					set
				);
#endif
			}

			isResizable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableMinimize(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_MINIMIZEBOX,
					set
				);
#endif
			}

			isMinimizable = set;
		}
		
		// @throw AL::Exceptions::Exception
		void EnableMaximize(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_MAXIMIZEBOX,
					set
				);
#endif
			}

			isMaximizable = set;
		}

		// @throw AL::Exceptions::Exception
		void EnableFileDrop(bool set = true)
		{
			if (IsCreated() || isCreating)
			{
#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				DragAcceptFiles(
					GetHandle(),
					set ? TRUE : FALSE
				);
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
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error creating Window"
				);
			}

			try
			{
				OnOpen();
			}
			catch (Exceptions::Exception&)
			{
				Destroy();

				throw;
			}

			isOpen = true;
		}

		// @return false if block && !OnClosing()
		bool Close(bool block = true)
		{
			if (IsOpen())
			{
				bool wasClosing;
				bool doDestroyOnClose = false;

				if (!(wasClosing = IsClosing()))
				{
					isClosing = true;
					isCloseCancelled = false;

					doDestroyOnClose = block;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
					try
					{
						PostNativeMessage(
							WM_CLOSE,
							0,
							0
						);
					}
					catch (const Exceptions::Exception&)
					{
						isForceClosing = true;
					}
#endif
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

					if (doDestroyOnClose)
					{
						Destroy();

						destroyOnClose = false;
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
			ShowWindow(
				GetHandle(),
				set ? SW_SHOW : SW_HIDE
			);
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
			ShowWindow(
				GetHandle(),
				set ? SW_MINIMIZE : SW_RESTORE
			);
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
			ShowWindow(
				GetHandle(),
				set ? SW_MAXIMIZE : SW_RESTORE
			);
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

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnCreate()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			if (!RegisterClassExA(&wClass))
			{

				throw Exceptions::SystemException(
					"RegisterClassExA"
				);
			}

			BitMask<DWORD> style(
				WS_BORDER | WS_CAPTION | WS_SYSMENU
			);

			if (IsResizable())   style.Add(WS_THICKFRAME);
			if (IsMinimized())   style.Add(WS_MINIMIZE);
			if (IsMinimizable()) style.Add(WS_MINIMIZEBOX);
			if (IsMaximized())   style.Add(WS_MAXIMIZE);
			if (IsMaximizable()) style.Add(WS_MAXIMIZEBOX);

			auto& position = GetPosition();
			auto& resolution = GetResolution();

			RECT rect;
			rect.left = static_cast<LONG>(position.X);
			rect.right = rect.left + static_cast<LONG>(resolution.X);
			rect.top = static_cast<LONG>(position.Y);
			rect.bottom = rect.top + static_cast<LONG>(resolution.Y);

			if (!AdjustWindowRectEx(&rect, style.Mask, FALSE, WS_EX_OVERLAPPEDWINDOW))
			{

				throw Exceptions::SystemException(
					"AdjustWindowRectEx"
				);
			}

			auto useDefaultPosition = isDefaultPosition;

			if ((handle = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, wClass.lpszClassName, GetTitle().GetCString(), style.Mask, static_cast<int>(useDefaultPosition ? CW_USEDEFAULT : rect.left), static_cast<int>(useDefaultPosition ? CW_USEDEFAULT : rect.top), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), NULL, NULL, wClass.hInstance, this)) == NULL)
			{
				UnregisterClassA(
					wClass.lpszClassName,
					wClass.hInstance
				);

				throw Exceptions::SystemException(
					"CreateWindowExA"
				);
			}
#endif
		}

		virtual void OnDestroy()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			UnregisterClassA(
				wClass.lpszClassName,
				wClass.hInstance
			);

			handle = NULL;
#endif
		}

		// @throw AL::Exceptions::Exception
		virtual void OnOpen()
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			ShowWindow(
				GetHandle(),
				SW_SHOW
			);
#endif
			
			try
			{
#if defined(AL_PLATFORM_WINDOWS)
				SetIcon(
					wClass.hIcon
				);
#endif

				SetTitle(
					GetTitle()
				);

				SetResolution(
					GetResolution().X,
					GetResolution().Y
				);

				SetPosition(
					GetPosition().X,
					GetPosition().Y
				);

				SetTopMost(
					IsTopMost()
				);

				EnableClose(
					IsClosable()
				);

				EnableResize(
					IsResizable()
				);

				EnableMinimize(
					IsMinimizable()
				);

				EnableMaximize(
					IsMaximizable()
				);

				EnableFileDrop(
					IsFileDropEnabled()
				);

				Minimize(
					IsMinimized()
				);

				Maximize(
					IsMaximized()
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error applying state"
				);
			}

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			if (auto hCursor = wClass.hCursor)
			{
				::SetCursor(
					hCursor
				);
			}
#endif
		}

		virtual void OnClose()
		{
		}

		// @return false to cancel
		virtual bool OnClosing()
		{
			if (!IsClosable())
			{

				return false;
			}

			return true;
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
#if defined(AL_PLATFORM_WINDOWS)
			MSG msg;
			msg.message = WM_NULL;

			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					isOpen = false;

					if (!destroyOnClose)
					{

						Destroy();
					}

					break;
				}

				DispatchMessageA(&msg);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		virtual void OnFileDrop(const String& path)
		{
		}

		// @throw AL::Exceptions::Exception
		// @return false to prevent move
		virtual bool OnPositionChanging()
		{
			if (!IsMovable())
			{

				return false;
			}

			return true;
		}
		
		// @throw AL::Exceptions::Exception
		virtual void OnPositionChanged(const WindowPosition& position)
		{
		}

		// @throw AL::Exceptions::Exception
		// @return false to prevent resize
		virtual bool OnResolutionChanging()
		{
			if (!IsResizable())
			{

				return false;
			}

			return true;
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

				case WM_CLOSE:
				{
					if (OnClosing())
					{
						OnClose();

						DestroyWindow(
							GetHandle()
						);
					}
				}
				break;

				case WM_DESTROY:
					PostQuitMessage(0);
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

			isCreating = true;

			try
			{
				OnCreate();
			}
			catch (Exceptions::Exception&)
			{
				isCreating = false;

				throw;
			}

			isCreated = true;
			isCreating = false;
		}

		void Destroy()
		{
			if (IsCreated())
			{
				OnDestroy();

				isCreated = false;
			}
		}

#if defined(AL_PLATFORM_WINDOWS)
		static HICON LoadNativeIcon(WindowIcons icon)
		{
			switch (icon)
			{
				case WindowIcons::None:
					return nullptr;

				case WindowIcons::Custom:
					throw Exceptions::NotImplementedException();

				case WindowIcons::Default:
					return LoadIcon(NULL, IDI_APPLICATION);;

				case WindowIcons::Asterisk:
					return LoadIcon(NULL, IDI_ASTERISK);

				case WindowIcons::Error:
					return LoadIcon(NULL, IDI_ERROR);

				case WindowIcons::Question:
					return LoadIcon(NULL, IDI_QUESTION);

				case WindowIcons::Shield:
					return LoadIcon(NULL, IDI_SHIELD);

				case WindowIcons::Warning:
					return LoadIcon(NULL, IDI_WARNING);
			}

			return nullptr;
		}

		static HCURSOR LoadNativeCursor(WindowCursors cursor)
		{
			switch (cursor)
			{
				case WindowCursors::None:
					return nullptr;

				case WindowCursors::Arrow:
					return LoadCursor(NULL, IDC_ARROW);

				case WindowCursors::Beam:
					return LoadCursor(NULL, IDC_IBEAM);

				case WindowCursors::Wait:
					return LoadCursor(NULL, IDC_WAIT);

				case WindowCursors::Hand:
					return LoadCursor(NULL, IDC_HAND);

				case WindowCursors::Help:
					return LoadCursor(NULL, IDC_HELP);
			}

			return nullptr;
		}

		// @throw AL::Exceptions::Exception
		static void SetNativeWindowLongPtr(HWND hWnd, int index, LONG_PTR value, bool set = true)
		{
			BitMask<LONG_PTR> mask;

			if (!(mask.Mask = GetWindowLongPtr(hWnd, index)))
			{

				throw Exceptions::SystemException(
					"GetWindowLongPtr"
				);
			}

			mask.Set(
				value,
				set
			);

			if (!SetWindowLongPtr(hWnd, index, mask.Mask))
			{

				throw Exceptions::SystemException(
					"SetWindowLongPtr"
				);
			}
		}

		static LRESULT CALLBACK NativeWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
#if defined(AL_X86)
			static constexpr int USER_DATA_INDEX = GWL_USERDATA;
#elif defined(AL_X86_64)
			static constexpr int USER_DATA_INDEX = GWLP_USERDATA;
#endif

			if (message == WM_CREATE)
			{
				auto lpWindow = reinterpret_cast<Window*>(
					reinterpret_cast<CREATESTRUCT*>(
						lParam
					)->lpCreateParams
				);

				SetWindowLongPtr(
					hWnd,
					USER_DATA_INDEX,
					reinterpret_cast<LONG_PTR>(
						lpWindow
					)
				);
			}

			else if (auto lpWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, USER_DATA_INDEX)))
			{

				return lpWindow->HandleNativeMessage(
					message,
					wParam,
					lParam
				);
			}

			return DefWindowProc(
				hWnd,
				message,
				wParam,
				lParam
			);
		}
#endif
	};
}
