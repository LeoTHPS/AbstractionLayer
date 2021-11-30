#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "GDI/Device.hpp"

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Hardware/Mouse.hpp"
#include "AL/Hardware/Keyboard.hpp"

#include <shellapi.h>

namespace AL::OS::Windows
{
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
		WindowIcons icon;
		::HICON     handle;

	public:
		WindowIcon(::HICON value)
			: icon(
				WindowIcons::UserDefined
			),
			handle(
				value
			)
		{
		}
		WindowIcon(WindowIcons value)
			: icon(
				value
			),
			handle(
				LoadNativeIcon(
					value
				)
			)
		{
		}

		virtual ~WindowIcon()
		{
		}

		auto GetHandle() const
		{
			return handle;
		}

		operator WindowIcons() const
		{
			return icon;
		}

	private:
		static ::HICON LoadNativeIcon(WindowIcons icon)
		{
			switch (icon)
			{
				case WindowIcons::None:
					return nullptr;

				case WindowIcons::UserDefined:
					throw NotImplementedException();

				case WindowIcons::Default:
					return ::LoadIconA(NULL, IDI_APPLICATION);;

				case WindowIcons::Asterisk:
					return ::LoadIconA(NULL, IDI_ASTERISK);

				case WindowIcons::Error:
					return ::LoadIconA(NULL, IDI_ERROR);

				case WindowIcons::Question:
					return ::LoadIconA(NULL, IDI_QUESTION);

				case WindowIcons::Shield:
					return ::LoadIconA(NULL, IDI_SHIELD);

				case WindowIcons::Warning:
					return ::LoadIconA(NULL, IDI_WARNING);
			}

			return nullptr;
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
		WindowCursors cursor;
		::HCURSOR     handle;

	public:
		WindowCursor(::HCURSOR value)
			: cursor(
				WindowCursors::UserDefined
			),
			handle(
				value
			)
		{
		}
		WindowCursor(WindowCursors value)
			: cursor(
				value
			),
			handle(
				LoadNativeCursor(
					value
				)
			)
		{
		}

		virtual ~WindowCursor()
		{
		}

		auto GetHandle() const
		{
			return handle;
		}

		operator WindowCursors() const
		{
			return cursor;
		}

	private:
		static ::HCURSOR LoadNativeCursor(WindowCursors cursor)
		{
			switch (cursor)
			{
				case WindowCursors::None:
					return nullptr;

				case WindowCursors::Arrow:
					return ::LoadCursorA(NULL, IDC_ARROW);

				case WindowCursors::Beam:
					return ::LoadCursorA(NULL, IDC_IBEAM);

				case WindowCursors::Wait:
					return ::LoadCursorA(NULL, IDC_WAIT);

				case WindowCursors::Hand:
					return ::LoadCursorA(NULL, IDC_HAND);

				case WindowCursors::Help:
					return ::LoadCursorA(NULL, IDC_HELP);

				case WindowCursors::UserDefined:
					throw OperationNotSupportedException();
			}

			return nullptr;
		}
	};

	typedef Drawing::Color        WindowColor;
	typedef Drawing::Colors       WindowColors;

	typedef Drawing::RectangleU32 WindowBounds;
	typedef Drawing::Vector2I32   WindowPosition;
	typedef Drawing::SizeU32      WindowResolution;

	enum class WindowClipboardDataFormats
	{
		String,
		WString,

		TIFF,
		Bitmap,

		RIFF,
		Wave,

		Drop,

		MetaFile,
		MetaFilePicture,
		
		NotSupported
	};

	template<typename T>
	struct Get_Window_Clipboard_Data_Format
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::NotSupported;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<String>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::String;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<typename String::Char*>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::String;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<const typename String::Char*>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::String;
	};
	template<size_t S>
	struct Get_Window_Clipboard_Data_Format<typename String::Char[S]>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::String;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<WString>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::WString;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<typename WString::Char*>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::WString;
	};
	template<>
	struct Get_Window_Clipboard_Data_Format<const typename WString::Char*>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::WString;
	};
	template<size_t S>
	struct Get_Window_Clipboard_Data_Format<typename WString::Char[S]>
	{
		static constexpr WindowClipboardDataFormats Value = WindowClipboardDataFormats::WString;
	};

	class Window
	{
		class Clipboard
		{
			Bool isCreated = False;

			Window* const lpWindow;

			Clipboard(Clipboard&&) = delete;
			Clipboard(const Clipboard&) = delete;

			explicit Clipboard(Window& window)
				: lpWindow(
					&window
				)
			{
			}

		public:
			virtual ~Clipboard()
			{
			}

			Bool IsCreated() const
			{
				return isCreated;
			}

			auto& GetWindow()
			{
				return *lpWindow;
			}
			auto& GetWindow() const
			{
				return *lpWindow;
			}

			auto GetFormat() const
			{
				if (IsCreated())
				{
					static ::UINT DATA_FORMATS[9] =
					{
						CF_TEXT,
						CF_UNICODETEXT,
						CF_TIFF,
						CF_BITMAP,
						CF_RIFF,
						CF_WAVE,
						CF_HDROP,
						CF_ENHMETAFILE,
						CF_METAFILEPICT
					};

					switch (::GetPriorityClipboardFormat(&DATA_FORMATS[0], 9))
					{
						case CF_TEXT:
							return WindowClipboardDataFormats::String;

						case CF_UNICODETEXT:
							return WindowClipboardDataFormats::WString;

						case CF_TIFF:
							return WindowClipboardDataFormats::TIFF;

						case CF_BITMAP:
							return WindowClipboardDataFormats::Bitmap;

						case CF_RIFF:
							return WindowClipboardDataFormats::RIFF;

						case CF_WAVE:
							return WindowClipboardDataFormats::Wave;

						case CF_HDROP:
							return WindowClipboardDataFormats::Drop;

						case CF_ENHMETAFILE:
							return WindowClipboardDataFormats::MetaFile;

						case CF_METAFILEPICT:
							return WindowClipboardDataFormats::MetaFilePicture;
					}
				}

				return WindowClipboardDataFormats::NotSupported;
			}

			auto GetSequenceNumber() const
			{
				::DWORD sequenceNumber;

				if ((sequenceNumber = ::GetClipboardSequenceNumber()) == 0)
				{
				}

				return static_cast<uint32>(
					sequenceNumber
				);
			}

			// @throw AL::Exception
			// @return False if empty
			template<typename T>
			Bool Get(T& value) const
			{
				static constexpr WindowClipboardDataFormats T_FORMAT = Get_Window_Clipboard_Data_Format<T>::Value;

				AL_ASSERT(
					IsCreated(),
					"Clipboard not created"
				);

				AL_ASSERT(
					GetDataFormat() == T_FORMAT,
					"Unexpected T_FORMAT"
				);

				if (!::OpenClipboard(GetWindow().GetHandle()))
				{

					throw SystemException(
						"OpenClipboard"
					);
				}

				if constexpr (T_FORMAT == WindowClipboardDataFormats::String)
				{
					::HANDLE hData;

					if ((hData = ::GetClipboardData(CF_TEXT)) == NULL)
					{
						::CloseClipboard();

						throw SystemException(
							"GetClipboardData"
						);
					}

					auto hValue = reinterpret_cast<const typename String::Char*>(
						GlobalLock(
							hData
						)
					);

					auto valueLength = String::GetLength(
						hValue
					);

					value = String(
						hValue,
						valueLength
					);

					::GlobalUnlock(
						hData
					);
				}
				else if constexpr (T_FORMAT == WindowClipboardDataFormats::WString)
				{
					::HANDLE hData;

					if ((hData = ::GetClipboardData(CF_UNICODETEXT)) == NULL)
					{
						::CloseClipboard();

						throw SystemException(
							"GetClipboardData"
						);
					}

					auto hValue = reinterpret_cast<const typename WString::Char*>(
						::GlobalLock(
							hData
						)
					);

					auto valueLength = WString::GetLength(
						hValue
					);

					value = WString(
						hValue,
						valueLength
					);

					::GlobalUnlock(
						hData
					);
				}

				::CloseClipboard();

				return False;
			}

			// @throw AL::Exception
			template<typename T>
			Void Set(const T& value)
			{
				static constexpr WindowClipboardDataFormats T_FORMAT = Get_Window_Clipboard_Data_Format<T>::Value;

				AL_ASSERT(
					IsCreated(),
					"Clipboard not created"
				);

				if (!::OpenClipboard(GetWindow().GetHandle()))
				{

					throw SystemException(
						"OpenClipboard"
					);
				}

				if (!::EmptyClipboard())
				{
					::CloseClipboard();

					throw SystemException(
						"EmptyClipboard"
					);
				}

				size_t      dataSize;
//				::UINT      dataFormat;
				const void* lpDataSource;

				if constexpr (T_FORMAT == WindowClipboardDataFormats::String)
				{
					dataSize = (String::GetLength(value) + 1) * sizeof(typename String::Char);
//					dataFormat = CF_TEXT;
					lpDataSource = &value[0];
				}
				else if constexpr (T_FORMAT == WindowClipboardDataFormats::WString)
				{
					dataSize = (WString::GetLength(value) + 1) * sizeof(typename WString::Char);
//					dataFormat = CF_UNICODETEXT;
					lpDataSource = &value[0];
				}

				::HANDLE hValue;

				if ((hValue = ::GlobalAlloc(GMEM_MOVEABLE, dataSize)) == NULL)
				{
					::CloseClipboard();

					throw SystemException(
						"GlobalAlloc"
					);
				}

				::HANDLE hValueLock;

				if ((hValueLock = ::GlobalLock(hValue)) == NULL)
				{
					::GlobalFree(
						hValue
					);

					::CloseClipboard();

					throw SystemException(
						"GlobalLock"
					);
				}

				memcpy(
					hValueLock,
					lpDataSource,
					dataSize
				);

				::GlobalUnlock(
					hValue
				);

				if (::SetClipboardData(CF_TEXT, hValue) == NULL)
				{
					::GlobalFree(
						hValue
					);

					::CloseClipboard();

					throw SystemException(
						"SetClipboardData"
					);
				}

				::CloseClipboard();
			}

			// @throw AL::Exception
			Void Clear()
			{
				AL_ASSERT(
					IsCreated(),
					"WindowClipboard not created"
				);

				if (!::OpenClipboard(GetWindow().GetHandle()))
				{

					throw SystemException(
						"OpenClipboard"
					);
				}

				if (!::EmptyClipboard())
				{
					::CloseClipboard();

					throw SystemException(
						"EmptyClipboard"
					);
				}

				::CloseClipboard();
			}

			// @throw AL::Exception
			Void Create()
			{
				AL_ASSERT(
					!IsCreated(),
					"Clipboard already created"
				);

				OnCreate();

				isCreated = True;
			}

			Void Destroy()
			{
				if (IsCreated())
				{
					OnDestroy();

					isCreated = False;
				}
			}

		protected:
			// @throw AL::Exception
			virtual Void OnCreate()
			{
				if (!::AddClipboardFormatListener(GetWindow().GetHandle()))
				{

					throw SystemException(
						"AddClipboardFormatListener"
					);
				}
			}

			virtual Void OnDestroy()
			{
				::RemoveClipboardFormatListener(
					GetWindow().GetHandle()
				);
			}

		private: // Window access
			friend Window;

			// @throw AL::Exception
			// @return True if handled
			Bool HandleNativeMessage(::LRESULT& result, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
			{
				if (message == WM_CLIPBOARDUPDATE)
				{
					result = 0;

					GetWindow().OnClipboardDataChanged(
						GetFormat()
					);

					return True;
				}

				return False;
			}
		};

		Bool isOpen            = False;
		Bool isCreated         = False;
		Bool isCreating        = False;
		Bool isClosing         = False;
		Bool isContentLoaded   = False;

		Bool isClosable        = True;
		Bool isResizable       = True;
		Bool isMinimizable     = True;
		Bool isMaximizable     = True;
		Bool isFileDropEnabled = False;

		Bool isMinimized       = False;
		Bool isMaximized       = False;
		Bool isDefaultPosition = True;

		::HWND                  hWindow;
		::WNDCLASSEXA           windowClass;

		WindowIcon              windowIcon;
		String                  windowName;
		String                  windowTitle;
		WindowCursor            windowCursor;
		WindowPosition          windowPosition;
		WindowResolution        windowResolution;
		WindowColor             windowBackgroundColor;
		size_t                  windowSetIconPendingCount = 0;

		Clipboard               clipboard;

		Hardware::MouseState    mouseState;
		Hardware::KeyboardState keyboardState;

		Hardware::MouseEvent    lastMouseEvent;
		Hardware::KeyboardEvent lastKeyboardEvent;

		Window(Window&&) = delete;
		Window(const Window&) = delete;

	public:
		explicit Window(String&& name)
			: Window(
				Move(name),
				::GetModuleHandleA(nullptr)
			)
		{
		}

		Window(String&& name, ::HINSTANCE hInstance)
			: windowClass(
				{ 0 }
			),
			windowIcon(
				WindowIcons::Default
			),
			windowName(
				Move(name)
			),
			windowTitle(
				windowName
			),
			windowCursor(
				WindowCursors::Arrow
			),
			windowResolution(
				1366,
				768
			),
			windowBackgroundColor(
				WindowColors::CornflowerBlue
			),
			clipboard(
				*this
			)
		{
			windowClass.cbSize = sizeof(::WNDCLASSEXA);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.hIcon = windowIcon.GetHandle();
			windowClass.hIconSm = windowIcon.GetHandle();
			windowClass.hCursor = windowCursor.GetHandle();
			windowClass.hInstance = hInstance;
			windowClass.lpfnWndProc = &Window::NativeWindowProc;
			windowClass.lpszClassName = windowName.GetCString();
		}

		virtual ~Window()
		{
			if (IsOpen())
			{
				Destroy();

				::DestroyWindow(
					hWindow
				);

				if (auto hBackground = windowClass.hbrBackground)
				{
					::DeleteObject(
						hBackground
					);
				}
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		Bool IsContentLoaded() const
		{
			return isContentLoaded;
		}

		Bool IsClosing() const
		{
			return isClosing;
		}

		Bool IsClosable() const
		{
			return isClosable;
		}

		Bool IsResizable() const
		{
			return isResizable;
		}

		Bool IsMinimized() const
		{
			return isMinimized;
		}

		Bool IsMaximized() const
		{
			return isMaximized;
		}

		Bool IsMinimizable() const
		{
			return isMinimizable;
		}

		Bool IsMaximizable() const
		{
			return isMaximizable;
		}

		Bool IsFileDropEnabled() const
		{
			return isFileDropEnabled;
		}

		auto& GetName() const
		{
			return windowName;
		}

		auto& GetTitle() const
		{
			return windowTitle;
		}

		auto GetIcon() const
		{
			return windowIcon;
		}

		auto GetCursor() const
		{
			return windowCursor;
		}

		::HWND GetHandle() const
		{
			return hWindow;
		}

		auto& GetPosition() const
		{
			return windowPosition;
		}

		auto& GetResolution() const
		{
			return windowResolution;
		}

		auto GetBackgroundColor() const
		{
			return windowBackgroundColor;
		}

		auto GetClipboardFormat() const
		{
			return clipboard.GetFormat();
		}

		auto GetClipboardSequenceNumber() const
		{
			return clipboard.GetSequenceNumber();
		}

		// @throw AL::Exception
		// @return False if empty
		template<typename T>
		Bool GetClipboard(T& value) const
		{
			if (!clipboard.Get(value))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		template<typename T>
		Void SetClipboard(const T& value)
		{
			clipboard.Set(
				value
			);
		}

		// @throw AL::Exception
		Bool SetIcon(const WindowIcon& value)
		{
			if (!OnIconChanging(value))
			{

				return False;
			}

			if (IsCreated() || isCreating)
			{
				PostNativeMessage(
					WM_SETICON,
					ICON_BIG,
					reinterpret_cast<LPARAM>(
						value.GetHandle()
					)
				);

				++windowSetIconPendingCount;

				try
				{
					PostNativeMessage(
						WM_SETICON,
						ICON_SMALL,
						reinterpret_cast<LPARAM>(
							value.GetHandle()
						)
					);
				}
				catch (Exception&)
				{
					--windowSetIconPendingCount;

					throw;
				}
				
				++windowSetIconPendingCount;
			}
			else
			{
				windowClass.hIcon = value.GetHandle();
				windowClass.hIconSm = value.GetHandle();

				OnIconChanged(
					value
				);
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetTitle(String&& value)
		{
			if (IsCreated() || isCreated)
			{
				if (!OnTitleChanging(value))
				{

					return False;
				}

				windowTitle = Move(
					value
				);

				if (!::SetWindowTextA(GetHandle(), GetTitle().GetCString()))
				{

					throw SystemException(
						"SetWindowTextA"
					);
				}
			}
			else
			{
				if (!OnTitleChanging(value))
				{

					return False;
				}

				windowTitle = Move(
					value
				);

				OnTitleChanged(
					GetTitle()
				);
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetTitle(const String& value)
		{
			if (!SetTitle(String(value)))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool SetCursor(const WindowCursor& value)
		{
			if (!OnCursorChanging(value))
			{

				return False;
			}

			windowClass.hCursor = value.GetHandle();
			windowCursor = value;

			if (IsCreated() || isCreating)
			{

				::SetCursor(
					windowClass.hCursor
				);
			}

			OnCursorChanged(
				value
			);

			return True;
		}

		// @throw AL::Exception
		Bool SetPosition(const WindowPosition& value)
		{
			if (!SetPosition(value.X, value.Y))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool SetPosition(WindowPosition::Type x, WindowPosition::Type y)
		{
			isDefaultPosition = False;

			if (IsCreated() || isCreating)
			{
				::RECT rect;

				if (!::GetWindowRect(GetHandle(), &rect))
				{

					throw SystemException(
						"GetWindowRect"
					);
				}

				if (!::MoveWindow(GetHandle(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), TRUE))
				{

					throw SystemException(
						"MoveWindow"
					);
				}
			}
			else
			{
				if (!OnPositionChanging(x, y))
				{

					return False;
				}

				windowPosition.X = x;
				windowPosition.Y = y;

				OnPositionChanged(
					x,
					y
				);
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
			if (!IsResizable())
			{

				return False;
			}

			if (IsCreated() || isCreating)
			{
				::RECT clientRect;

				if (!::GetClientRect(GetHandle(), &clientRect))
				{

					throw SystemException(
						"GetClientRect"
					);
				}

				::RECT windowRect;

				if (!::GetWindowRect(GetHandle(), &windowRect))
				{

					throw SystemException(
						"GetWindowRect"
					);
				}

				auto windowRectPadding_X = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
				auto windowRectPadding_Y = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

				::RECT rect =
				{
					.left   = windowRect.left,
					.top    = windowRect.top,
					.right  = windowRect.left + static_cast<::LONG>(width) + windowRectPadding_X,
					.bottom = windowRect.top + static_cast<::LONG>(height) + windowRectPadding_Y
				};

				if (!::MoveWindow(GetHandle(), static_cast<int>(rect.left), static_cast<int>(rect.top), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), TRUE))
				{

					throw SystemException(
						"MoveWindow"
					);
				}
			}
			else
			{
				if (!OnResolutionChanging(width, height))
				{

					return False;
				}

				windowResolution.Width = width;
				windowResolution.Height = height;

				OnResolutionChanged(
					width,
					height
				);
			}

			return True;
		}

		// @throw AL::Exception
		Void SetBackgroundColor(WindowColor value)
		{
			windowBackgroundColor = value;

			if (auto hBackground = windowClass.hbrBackground)
			{
				::DeleteObject(
					hBackground
				);
			}

			if ((windowClass.hbrBackground = ::CreateSolidBrush(RGB(value.R, value.G, value.B))) == NULL)
			{

				throw GDI::GDIException(
					"CreateSolidBrush"
				);
			}

			if (IsCreated() || isCreating)
			{
				if (::InvalidateRect(GetHandle(), nullptr, TRUE) == 0)
				{

					throw GDI::GDIException(
						"InvalidateRect"
					);
				}
			}
		}
		// @throw AL::Exception
		Void SetBackgroundColor(WindowColors value)
		{
			SetBackgroundColor(
				WindowColor(value)
			);
		}

		// @throw AL::Exception
		Void EnableClose(Bool set = True)
		{
			if (IsCreated() || isCreating)
			{
				if (::EnableMenuItem(::GetSystemMenu(GetHandle(), FALSE), SC_CLOSE, set ? MF_ENABLED : MF_GRAYED) == -1)
				{

					throw SystemException(
						"EnableMenuItem"
					);
				}
			}

			isClosable = set;
		}

		// @throw AL::Exception
		Void EnableResize(Bool set = True)
		{
			if (IsCreated() || isCreating)
			{
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_THICKFRAME,
					set
				);
			}

			isResizable = set;
		}

		// @throw AL::Exception
		Void EnableMinimize(Bool set = True)
		{
			if (IsCreated() || isCreating)
			{
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_MINIMIZEBOX,
					set
				);
			}

			isMinimizable = set;
		}

		// @throw AL::Exception
		Void EnableMaximize(Bool set = True)
		{
			if (IsCreated() || isCreating)
			{
				SetNativeWindowLongPtr(
					GetHandle(),
					GWL_STYLE,
					WS_MAXIMIZEBOX,
					set
				);
			}

			isMaximizable = set;
		}

		// @throw AL::Exception
		Void EnableFileDrop(Bool set = True)
		{
			if (IsCreated() || isCreating)
			{
				::DragAcceptFiles(
					GetHandle(),
					set ? TRUE : FALSE
				);
			}

			isFileDropEnabled = set;
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsCreated(),
				"Window already created"
			);

			Create();

			try
			{
				OnOpen();
			}
			catch (Exception&)
			{
				Destroy();

				throw;
			}

			isOpen = True;
		}

		// @throw AL::Exception
		Void Close()
		{
			if (IsOpen() && !IsClosing())
			{
				isClosing = True;

				try
				{
					PostNativeMessage(
						WM_CLOSE,
						0,
						0
					);
				}
				catch (Exception&)
				{
					isClosing = False;

					throw;
				}
			}
		}

		// @throw AL::Exception
		Void Paint()
		{
			auto& resolution = GetResolution();

			WindowBounds bounds(
				resolution.Width,
				resolution.Height
			);

			Paint(
				bounds
			);
		}
		// @throw AL::Exception
		Void Paint(const WindowBounds& bounds)
		{
			Paint(
				bounds.Left,
				bounds.Top,
				bounds.GetWidth(),
				bounds.GetHeight()
			);
		}
		// @throw AL::Exception
		Void Paint(WindowBounds::Type x, WindowBounds::Type y, WindowBounds::Type width, WindowBounds::Type height)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			::RECT rect;
			rect.left = x;
			rect.right = x + width;
			rect.top = y;
			rect.bottom = y + height;

			if (!::RedrawWindow(GetHandle(), &rect, nullptr, RDW_INVALIDATE | RDW_NOCHILDREN))
			{
				
				throw GDI::GDIException(
					"RedrawWindow"
				);
			}
		}

		// @throw AL::Exception
		Void Focus(Bool set = True)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			::ShowWindow(
				GetHandle(),
				set ? SW_SHOW : SW_HIDE
			);
		}

		// @throw AL::Exception
		Bool Minimize(Bool set = True)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			if (!IsMinimizable())
			{

				return False;
			}

			::ShowWindow(
				GetHandle(),
				set ? SW_MINIMIZE : SW_RESTORE
			);

			return True;
		}

		// @throw AL::Exception
		Bool Maximize(Bool set = True)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			if (!IsMaximizable())
			{

				return False;
			}

			::ShowWindow(
				GetHandle(),
				set ? SW_MAXIMIZE : SW_RESTORE
			);

			return True;
		}

		// @throw AL::Exception
		Void PostNativeMessage(::UINT message, ::WPARAM wParam, ::LPARAM lParam)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			if (!::PostMessageA(GetHandle(), message, wParam, lParam))
			{

				throw SystemException(
					"PostMessage"
				);
			}
		}

		// @throw AL::Exception
		// @return False on closed
		Bool Update(TimeSpan delta)
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			AL_ASSERT(
				IsContentLoaded(),
				"Window content not loaded"
			);

			::MSG msg;
			msg.message = WM_NULL;

			while (::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(
					&msg
				);

				if (msg.message == WM_QUIT)
				{
					isOpen = False;

					return False;
				}

				::DispatchMessageA(
					&msg
				);
			}

			OnUpdate(
				delta
			);

			return True;
		}

		// @throw AL::Exception
		Void LoadContent()
		{
			AL_ASSERT(
				IsCreated(),
				"Window not created"
			);

			AL_ASSERT(
				!IsContentLoaded(),
				"Window content already loaded"
			);

			OnLoadContent();

			isContentLoaded = True;
		}

		Void UnloadContent()
		{
			if (IsCreated() && IsContentLoaded())
			{
				OnUnloadContent();

				isContentLoaded = False;
			}
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnCreate()
		{
			if (!::RegisterClassExA(&windowClass))
			{

				throw SystemException(
					"RegisterClassExA"
				);
			}

			BitMask<::DWORD> style(
				WS_BORDER | WS_CAPTION | WS_SYSMENU
			);

			if (IsResizable())   style.Add(WS_THICKFRAME);
			if (IsMinimized())   style.Add(WS_MINIMIZE);
			if (IsMinimizable()) style.Add(WS_MINIMIZEBOX);
			if (IsMaximized())   style.Add(WS_MAXIMIZE);
			if (IsMaximizable()) style.Add(WS_MAXIMIZEBOX);

			auto& position = GetPosition();
			auto& resolution = GetResolution();

			::RECT rect;
			rect.left = static_cast<::LONG>(position.X);
			rect.right = rect.left + static_cast<::LONG>(resolution.Width);
			rect.top = static_cast<::LONG>(position.Y);
			rect.bottom = rect.top + static_cast<::LONG>(resolution.Height);

			if (!::AdjustWindowRectEx(&rect, style.Value, FALSE, WS_EX_OVERLAPPEDWINDOW))
			{

				throw SystemException(
					"AdjustWindowRectEx"
				);
			}

			if ((hWindow = ::CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, windowClass.lpszClassName, GetTitle().GetCString(), style.Value, static_cast<int>(isDefaultPosition ? CW_USEDEFAULT : rect.left), static_cast<int>(isDefaultPosition ? CW_USEDEFAULT : rect.top), static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top), NULL, NULL, windowClass.hInstance, this)) == NULL)
			{
				::UnregisterClassA(
					windowClass.lpszClassName,
					windowClass.hInstance
				);

				throw SystemException(
					"CreateWindowExA"
				);
			}

			try
			{
				clipboard.Create();
			}
			catch (Exception& exception)
			{
				::UnregisterClassA(
					windowClass.lpszClassName,
					windowClass.hInstance
				);

				throw Exception(
					Move(exception),
					"Error creating clipboard"
				);
			}
		}

		virtual Void OnDestroy()
		{
			clipboard.Destroy();

			::UnregisterClassA(
				windowClass.lpszClassName,
				windowClass.hInstance
			);
		}

		// @throw AL::Exception
		virtual Void OnLoadContent()
		{
		}

		virtual Void OnUnloadContent()
		{
		}

		// @throw AL::Exception
		virtual Void OnOpen()
		{
			::ShowWindow(
				GetHandle(),
				SW_SHOW
			);

			try
			{
				SetIcon(
					windowClass.hIcon
				);

				SetCursor(
					GetCursor()
				);

				SetTitle(
					GetTitle()
				);

				SetResolution(
					GetResolution().Width,
					GetResolution().Height
				);

				SetPosition(
					GetPosition().X,
					GetPosition().Y
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
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error applying state"
				);
			}
		}

		// @return False to cancel
		virtual Bool OnClosing()
		{

			return True;
		}

		virtual Void OnClose()
		{
		}

		// @throw AL::Exception
		virtual Void OnPaint(GDI::Device& device)
		{
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta)
		{
		}

		// @throw AL::Exception
		virtual Void OnFileDrop(const String& path)
		{
		}

		// @throw AL::Exception
		virtual Void OnIconChanged(const WindowIcon& icon)
		{
		}

		// @throw AL::Exception
		// @return False to cancel
		virtual Bool OnIconChanging(const WindowIcon& icon)
		{

			return True;
		}

		// @throw AL::Exception
		virtual Void OnTitleChanged(const String& title)
		{
		}

		// @throw AL::Exception
		// @return False to cancel
		virtual Bool OnTitleChanging(const String& title)
		{

			return True;
		}

		// @throw AL::Exception
		virtual Void OnCursorChanged(const WindowCursor& cursor)
		{
		}

		// @throw AL::Exception
		// @return False to cancel
		virtual Bool OnCursorChanging(const WindowCursor& cursor)
		{

			return True;
		}

		// @throw AL::Exception
		virtual Void OnPositionChanged(WindowPosition::Type x, WindowPosition::Type y)
		{
		}

		// @throw AL::Exception
		// @return False to cancel
		virtual Bool OnPositionChanging(WindowPosition::Type x, WindowPosition::Type y)
		{

			return True;
		}

		// @throw AL::Exception
		virtual Void OnResolutionChanged(WindowResolution::Type width, WindowResolution::Type height)
		{
		}

		// @throw AL::Exception
		// @return False to cancel
		virtual Bool OnResolutionChanging(WindowResolution::Type width, WindowResolution::Type height)
		{

			return True;
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
		virtual Void OnClipboardDataChanged(WindowClipboardDataFormats format)
		{
		}

	private:
		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"Window already created"
			);

			isCreating = True;

			try
			{
				OnCreate();
			}
			catch (Exception&)
			{
				isCreating = False;

				OnDestroy();

				throw;
			}

			isCreated = True;
			isCreating = False;

			try
			{
				LoadContent();
			}
			catch (Exception& exception)
			{
				Destroy();

				throw Exception(
					Move(exception),
					"Error loading Window content"
				);
			}
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				if (IsContentLoaded())
				{

					UnloadContent();
				}

				OnDestroy();

				isCreated = False;
			}
		}

		// @throw AL::Exception
		static Void SetNativeWindowLongPtr(::HWND hWND, int index, ::LONG_PTR value, Bool set = True)
		{
			BitMask<::LONG_PTR> mask;

			if (!(mask.Value = ::GetWindowLongPtrA(hWND, index)))
			{

				throw SystemException(
					"GetWindowLongPtrA"
				);
			}

			mask.Set(
				value,
				set
			);

			if (!::SetWindowLongPtrA(hWND, index, mask.Value))
			{

				throw SystemException(
					"SetWindowLongPtrA"
				);
			}
		}

		static ::LRESULT CALLBACK NativeWindowProc(::HWND hWND, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
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

				::SetWindowLongPtrA(
					hWND,
					USER_DATA_INDEX,
					reinterpret_cast<LONG_PTR>(
						lpWindow
					)
				);
			}
			else if (auto lpWindow = reinterpret_cast<Window*>(::GetWindowLongPtrA(hWND, USER_DATA_INDEX)))
			{
				::LRESULT result;

				if (lpWindow->clipboard.HandleNativeMessage(result, message, wParam, lParam))
				{

					return result;
				}

				auto UpdateMousePosition = [lpWindow](::LPARAM _lParam)
				{
					auto points = MAKEPOINTS(_lParam);

					lpWindow->lastMouseEvent.Position.X = points.x;
					lpWindow->lastMouseEvent.Position.Y = points.y;
				};

				switch (message)
				{
					case WM_CHAR:
					{
						if (::isprint(static_cast<int>(wParam)))
						{
							lpWindow->lastKeyboardEvent.Char = static_cast<char>(wParam);
							lpWindow->lastKeyboardEvent.Type = Hardware::KeyboardEvents::Char;

							lpWindow->OnKeyboardEvent(
								lpWindow->lastKeyboardEvent
							);
						}
					}
					break;

					case WM_KEYUP:
					case WM_KEYDOWN:
					{
						auto VirtualKeyToInputKey = [](Hardware::Keys& _key, WPARAM _wParam)->Bool
						{
							switch (_wParam)
							{
								case 0x41:
									_key = Hardware::Keys::A;
									return True;
								case 0x42:
									_key = Hardware::Keys::B;
									return True;
								case 0x43:
									_key = Hardware::Keys::C;
									return True;
								case 0x44:
									_key = Hardware::Keys::D;
									return True;
								case 0x45:
									_key = Hardware::Keys::E;
									return True;
								case 0x46:
									_key = Hardware::Keys::F;
									return True;
								case 0x47:
									_key = Hardware::Keys::G;
									return True;
								case 0x48:
									_key = Hardware::Keys::H;
									return True;
								case 0x49:
									_key = Hardware::Keys::I;
									return True;
								case 0x4A:
									_key = Hardware::Keys::J;
									return True;
								case 0x4B:
									_key = Hardware::Keys::K;
									return True;
								case 0x4C:
									_key = Hardware::Keys::L;
									return True;
								case 0x4D:
									_key = Hardware::Keys::M;
									return True;
								case 0x4E:
									_key = Hardware::Keys::N;
									return True;
								case 0x4F:
									_key = Hardware::Keys::O;
									return True;
								case 0x50:
									_key = Hardware::Keys::P;
									return True;
								case 0x51:
									_key = Hardware::Keys::Q;
									return True;
								case 0x52:
									_key = Hardware::Keys::R;
									return True;
								case 0x53:
									_key = Hardware::Keys::S;
									return True;
								case 0x54:
									_key = Hardware::Keys::T;
									return True;
								case 0x55:
									_key = Hardware::Keys::U;
									return True;
								case 0x56:
									_key = Hardware::Keys::V;
									return True;
								case 0x57:
									_key = Hardware::Keys::W;
									return True;
								case 0x58:
									_key = Hardware::Keys::X;
									return True;
								case 0x59:
									_key = Hardware::Keys::Y;
									return True;
								case 0x5A:
									_key = Hardware::Keys::Z;
									return True;

								case 0x30:
									_key = Hardware::Keys::Num0;
									return True;
								case 0x31:
									_key = Hardware::Keys::Num1;
									return True;
								case 0x32:
									_key = Hardware::Keys::Num2;
									return True;
								case 0x33:
									_key = Hardware::Keys::Num3;
									return True;
								case 0x34:
									_key = Hardware::Keys::Num4;
									return True;
								case 0x35:
									_key = Hardware::Keys::Num5;
									return True;
								case 0x36:
									_key = Hardware::Keys::Num6;
									return True;
								case 0x37:
									_key = Hardware::Keys::Num7;
									return True;
								case 0x38:
									_key = Hardware::Keys::Num8;
									return True;
								case 0x39:
									_key = Hardware::Keys::Num9;
									return True;

								case VK_NUMPAD0:
									_key = Hardware::Keys::NumPad0;
									return True;
								case VK_NUMPAD1:
									_key = Hardware::Keys::NumPad1;
									return True;
								case VK_NUMPAD2:
									_key = Hardware::Keys::NumPad2;
									return True;
								case VK_NUMPAD3:
									_key = Hardware::Keys::NumPad3;
									return True;
								case VK_NUMPAD4:
									_key = Hardware::Keys::NumPad4;
									return True;
								case VK_NUMPAD5:
									_key = Hardware::Keys::NumPad5;
									return True;
								case VK_NUMPAD6:
									_key = Hardware::Keys::NumPad6;
									return True;
								case VK_NUMPAD7:
									_key = Hardware::Keys::NumPad7;
									return True;
								case VK_NUMPAD8:
									_key = Hardware::Keys::NumPad8;
									return True;
								case VK_NUMPAD9:
									_key = Hardware::Keys::NumPad9;
									return True;
									
								case VK_OEM_3:
									_key = Hardware::Keys::Tilde;
									return True;
								case VK_MENU:
									_key = Hardware::Keys::Alt;
									return True;
								case VK_LMENU:
									_key = Hardware::Keys::LAlt;
									return True;
								case VK_RMENU:
									_key = Hardware::Keys::RAlt;
									return True;
								case VK_CLEAR:
									_key = Hardware::Keys::Clear;
									return True;
								case VK_DELETE:
									_key = Hardware::Keys::Delete;
									return True;
								case VK_RETURN:
									_key = Hardware::Keys::Enter;
									return True;
								case VK_ESCAPE:
									_key = Hardware::Keys::Escape;
									return True;
								case VK_BACK:
									_key = Hardware::Keys::Backspace;
									return True;
								case VK_CONTROL:
									_key = Hardware::Keys::Control;
									return True;
								case VK_LCONTROL:
									_key = Hardware::Keys::LControl;
									return True;
								case VK_RCONTROL:
									_key = Hardware::Keys::RControl;
									return True;
								case VK_SHIFT:
									_key = Hardware::Keys::Shift;
									return True;
								case VK_LSHIFT:
									_key = Hardware::Keys::LShift;
									return True;
								case VK_RSHIFT:
									_key = Hardware::Keys::RShift;
									return True;
								case VK_SPACE:
									_key = Hardware::Keys::Spacebar;
									return True;
								case VK_TAB:
									_key = Hardware::Keys::Tab;
									return True;

								case VK_UP:
									_key = Hardware::Keys::Up;
									return True;
								case VK_DOWN:
									_key = Hardware::Keys::Down;
									return True;
								case VK_LEFT:
									_key = Hardware::Keys::Left;
									return True;
								case VK_RIGHT:
									_key = Hardware::Keys::Right;
									return True;
							}

							return False;
						};

						Bool isKeyDown = (message == WM_KEYDOWN);

						lpWindow->lastKeyboardEvent.Type = isKeyDown ? Hardware::KeyboardEvents::KeyDown : Hardware::KeyboardEvents::KeyUp;

						if (VirtualKeyToInputKey(lpWindow->lastKeyboardEvent.Key, wParam))
						{
							lpWindow->keyboardState.SetPressed(
								lpWindow->lastKeyboardEvent.Key,
								isKeyDown
							);

							lpWindow->OnKeyboardEvent(
								lpWindow->lastKeyboardEvent
							);
						}

						switch (wParam)
						{
							case VK_MENU:
							case VK_SHIFT:
							case VK_CONTROL:
							{
								wParam = MapVirtualKey(
									static_cast<UINT>((lParam & 0x00FF0000) >> 16),
									MAPVK_VSC_TO_VK_EX
								);

								if (VirtualKeyToInputKey(lpWindow->lastKeyboardEvent.Key, wParam))
								{
									lpWindow->keyboardState.SetPressed(
										lpWindow->lastKeyboardEvent.Key,
										isKeyDown
									);

									lpWindow->OnKeyboardEvent(
										lpWindow->lastKeyboardEvent
									);
								}
							}
							break;
						}
					}
					break;

					case WM_MOUSEMOVE:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::Move;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					case WM_MOUSEWHEEL:
					{
						auto delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

						// scroll up
						if (delta > 0)
						{
							lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ScrollUp;
						}

						// scroll down
						else if (delta < 0)
						{
							lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ScrollDown;
						}

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					// left

					case WM_LBUTTONDOWN:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Left,
							True
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonDown;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Left;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					case WM_LBUTTONUP:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Left,
							False
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonUp;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Left;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					// right

					case WM_RBUTTONDOWN:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Right,
							True
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonDown;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Right;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					case WM_RBUTTONUP:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Right,
							False
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonUp;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Right;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					// middle

					case WM_MBUTTONDOWN:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Middle,
							True
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonDown;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Middle;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					case WM_MBUTTONUP:
					{
						UpdateMousePosition(
							lParam
						);

						lpWindow->mouseState.SetPressed(
							Hardware::MouseButtons::Middle,
							False
						);

						lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonUp;
						lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::Middle;

						lpWindow->OnMouseEvent(
							lpWindow->lastMouseEvent
						);
					}
					break;

					// x1/x2

					case WM_XBUTTONDOWN:
					{
						UpdateMousePosition(
							lParam
						);

						switch (GET_XBUTTON_WPARAM(wParam))
						{
							case XBUTTON1:
							{
								lpWindow->mouseState.SetPressed(
									Hardware::MouseButtons::X1,
									True
								);

								lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonDown;
								lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::X1;

								lpWindow->OnMouseEvent(
									lpWindow->lastMouseEvent
								);
							}
							break;

							case XBUTTON2:
							{
								lpWindow->mouseState.SetPressed(
									Hardware::MouseButtons::X2,
									True
								);

								lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonDown;
								lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::X2;

								lpWindow->OnMouseEvent(
									lpWindow->lastMouseEvent
								);
							}
							break;
						}
					}
					break;

					case WM_XBUTTONUP:
					{
						UpdateMousePosition(
							lParam
						);

						switch (GET_XBUTTON_WPARAM(wParam))
						{
							case XBUTTON1:
							{
								lpWindow->mouseState.SetPressed(
									Hardware::MouseButtons::X1,
									False
								);

								lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonUp;
								lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::X1;

								lpWindow->OnMouseEvent(
									lpWindow->lastMouseEvent
								);
							}
							break;

							case XBUTTON2:
							{
								lpWindow->mouseState.SetPressed(
									Hardware::MouseButtons::X2,
									False
								);

								lpWindow->lastMouseEvent.Type = Hardware::MouseEvents::ButtonUp;
								lpWindow->lastMouseEvent.Button = Hardware::MouseButtons::X2;

								lpWindow->OnMouseEvent(
									lpWindow->lastMouseEvent
								);
							}
							break;
						}
					}
					break;

					case WM_PAINT:
					{
						auto device = GDI::Device::BeginPaint(
							hWND
						);

						lpWindow->OnPaint(
							device
						);
					}
					return 0;

					case WM_ERASEBKGND:
					{
						auto hDC = reinterpret_cast<::HDC>(
							wParam
						);

						if (::SetMapMode(hDC, MM_ANISOTROPIC) == 0)
						{

							throw GDI::GDIException(
								"SetMapMode"
							);
						}

						auto& resolution = lpWindow->GetResolution();

						if (::SetWindowExtEx(hDC, static_cast<int>(resolution.Width), static_cast<int>(resolution.Height), nullptr) == 0)
						{

							throw GDI::GDIException(
								"SetWindowExtEx"
							);
						}

						if (::SetViewportExtEx(hDC, static_cast<int>(resolution.Width), static_cast<int>(resolution.Height), nullptr) == 0)
						{

							throw GDI::GDIException(
								"SetViewportExtEx"
							);
						}

						::RECT rect;

						if (::GetClientRect(lpWindow->GetHandle(), &rect) == 0)
						{

							throw SystemException(
								"GetClientRect"
							);
						}

						if (!::FillRect(hDC, &rect, lpWindow->windowClass.hbrBackground))
						{

							throw GDI::GDIException(
								"FillRect"
							);
						}
					}
					return 1;

					// TODO: handle like WM_SETTEXT
					case WM_MOVE:
					{
						auto point = MAKEPOINTS(
							lParam
						);

						if (!lpWindow->OnPositionChanging(point.x, point.y))
						{

							return 0;
						}

						lpWindow->windowPosition.X = point.x;
						lpWindow->windowPosition.Y = point.y;

						lpWindow->OnPositionChanged(
							point.x,
							point.y
						);
					}
					break;

					case WM_SIZE:
					{
						switch (wParam)
						{
							case SIZE_MINIMIZED:
								lpWindow->isMinimized = True;
								break;

							case SIZE_MAXIMIZED:
								lpWindow->isMaximized = True;
								break;

							case SIZE_RESTORED:
							{
								lpWindow->isMinimized = False;
								lpWindow->isMaximized = False;

								auto width = static_cast<typename WindowResolution::Type>(
									LOWORD(lParam)
								);
								auto height = static_cast<typename WindowResolution::Type>(
									HIWORD(lParam)
								);

								if (!lpWindow->OnResolutionChanging(width, height))
								{

									return 0;
								}

								lpWindow->windowResolution.Width  = width;
								lpWindow->windowResolution.Height = height;

								lpWindow->OnResolutionChanged(
									lpWindow->windowResolution.Width,
									lpWindow->windowResolution.Height
								);
							}
							break;
						}
					}
					break;

					// TODO: handle like WM_SETTEXT
					case WM_SETICON:
					{
						if (lpWindow->windowSetIconPendingCount == 0)
						{
							auto hIcon = reinterpret_cast<::HICON>(
								lParam
							);

							lpWindow->windowIcon          = hIcon;
							lpWindow->windowClass.hIcon   = hIcon;
							lpWindow->windowClass.hIconSm = hIcon;

							lpWindow->OnIconChanged(
								lpWindow->windowIcon
							);
						}
						else if (--lpWindow->windowSetIconPendingCount == 0)
						{
							lpWindow->OnIconChanged(
								lpWindow->windowIcon
							);
						}
					}
					break;

					// TODO: handle like WM_SETTEXT
					case WM_SETCURSOR:
					{
						::SetCursor(
							lpWindow->windowClass.hCursor
						);
					}
					return TRUE;

					case WM_SETTEXT:
					{
						auto lpTitle = reinterpret_cast<const String::Char*>(
							lParam
						);

						if (lpTitle != lpWindow->windowTitle.GetCString())
						{
							String title(
								reinterpret_cast<const String::Char*>(lParam)
							);

							if (!lpWindow->OnTitleChanging(title))
							{

								return TRUE;
							}

							lpWindow->windowTitle = Move(
								title
							);
						}

						lpWindow->OnTitleChanged(
							lpWindow->GetTitle()
						);
					}
					break;

					case WM_CLOSE:
					{
						lpWindow->isClosing = False;

						if (lpWindow->OnClosing())
						{
							lpWindow->OnClose();

							lpWindow->Destroy();

							::DestroyWindow(
								lpWindow->GetHandle()
							);
						}
					}
					return 0;

					case WM_DESTROY:
						PostQuitMessage(0);
						break;
				}

				return ::DefWindowProcA(
					lpWindow->GetHandle(),
					message,
					wParam,
					lParam
				);
			}

			return ::DefWindowProcA(
				hWND,
				message,
				wParam,
				lParam
			);
		}
	};
}
