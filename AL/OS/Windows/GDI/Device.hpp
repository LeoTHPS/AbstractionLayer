#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "GDIException.hpp"
#include "SolidColorBrush.hpp"

#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include <wingdi.h>

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "Gdi32.lib")
#endif

namespace AL::OS::Windows::GDI
{
	class Device
	{
		::HDC         hDC;
		::HWND        hWND;
		::PAINTSTRUCT paintStruct;

		Device(const Device&) = delete;

		Device(::HDC hDC, ::HWND hWND, ::PAINTSTRUCT&& paintStruct)
			: hDC(
				hDC
			),
			hWND(
				hWND
			),
			paintStruct(
				Move(paintStruct)
			)
		{
		}

	public:
		// @throw AL::Exception
		static Device BeginPaint(::HWND hWND)
		{
			::HDC hDC;
			::PAINTSTRUCT paintStruct;

			if ((hDC = ::BeginPaint(hWND, &paintStruct)) == NULL)
			{

				throw GDIException(
					"BeginPaint"
				);
			}

			return Device(
				hDC,
				hWND,
				Move(paintStruct)
			);
		}

		Device(Device&& device)
			: hDC(
				device.hDC
			),
			hWND(
				device.hWND
			),
			paintStruct(
				Move(device.paintStruct)
			)
		{
			device.hDC = NULL;
			device.hWND = NULL;
		}

		virtual ~Device()
		{
			if (hDC != NULL)
			{
				::EndPaint(
					hWND,
					&paintStruct
				);
			}
		}

		auto GetHandle() const
		{
			return hDC;
		}

		// @throw AL::Exception
		template<typename T>
		Void DrawString(const String& string, T x, T y, Drawing::Color foreground, Drawing::Color background)
		{
			DrawString(
				string,
				Drawing::Rectangle<T>(x, x + (paintStruct.rcPaint.right - paintStruct.rcPaint.left), y, y + (paintStruct.rcPaint.bottom - paintStruct.rcPaint.top)),
				foreground,
				background
			);
		}
		// @throw AL::Exception
		template<typename T>
		Void DrawString(const String& string, T x, T y, T width, T height, Drawing::Color foreground, Drawing::Color background)
		{
			DrawString(
				string,
				Drawing::Rectangle<T>(x, x + width, y, y + height),
				foreground,
				background
			);
		}
		// @throw AL::Exception
		template<typename T>
		Void DrawString(const String& string, const Drawing::Rectangle<T>& rectangle, Drawing::Color foreground, Drawing::Color background)
		{
			if (string.GetLength() == 0)
			{

				return;
			}

			::RECT rect =
			{
				.left   = BitConverter::Cast<::LONG>(rectangle.Left),
				.top    = BitConverter::Cast<::LONG>(rectangle.Top),
				.right  = BitConverter::Cast<::LONG>(rectangle.Right),
				.bottom = BitConverter::Cast<::LONG>(rectangle.Bottom)
			};

			::COLORREF prevBackground;
			::COLORREF prevForeground;

			if ((prevBackground = ::SetBkColor(GetHandle(), (static_cast<::COLORREF>(background.B) << 16) | (static_cast<::COLORREF>(background.G) << 8) | static_cast<::COLORREF>(background.R))) == CLR_INVALID)
			{

				throw GDIException(
					"SetBkColor"
				);
			}

			if ((prevForeground = ::SetTextColor(GetHandle(), (static_cast<::COLORREF>(foreground.B) << 16) | (static_cast<::COLORREF>(foreground.G) << 8) | static_cast<::COLORREF>(foreground.R))) == CLR_INVALID)
			{

				throw GDIException(
					"SetTextColor"
				);
			}

			if (::DrawTextA(GetHandle(), string.GetCString(), string.GetLength(), &rect, DT_LEFT) == 0)
			{

				throw GDIException(
					"DrawTextA"
				);
			}

			if (::SetBkColor(GetHandle(), prevBackground) == CLR_INVALID)
			{

				throw GDIException(
					"SetBkColor"
				);
			}

			if (::SetTextColor(GetHandle(), prevForeground) == CLR_INVALID)
			{

				throw GDIException(
					"SetTextColor"
				);
			}
		}

		// @throw AL::Exception
		template<typename T>
		Void FillRectangle(const Drawing::Rectangle<T>& rectangle, const SolidColorBrush& brush)
		{
			::RECT rect =
			{
				.left   = BitConverter::Cast<::LONG>(rectangle.Left),
				.top    = BitConverter::Cast<::LONG>(rectangle.Top),
				.right  = BitConverter::Cast<::LONG>(rectangle.Right),
				.bottom = BitConverter::Cast<::LONG>(rectangle.Bottom)
			};

			if (::FillRect(GetHandle(), &rect, brush.GetHandle()) == 0)
			{

				throw GDIException(
					"FillRect"
				);
			}
		}

		Device& operator = (Device&& device)
		{
			if (hDC != NULL)
			{
				::EndPaint(
					hWND,
					&paintStruct
				);
			}

			hDC = device.hDC;
			device.hDC = NULL;

			hWND = device.hWND;
			device.hWND = NULL;

			paintStruct = Move(
				device.paintStruct
			);

			return *this;
		}

		Bool operator == (const Device& device) const
		{
			if (hDC != device.hDC)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Device& device) const
		{
			if (operator==(device))
			{

				return False;
			}

			return True;
		}
	};
}
