#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "GDIException.hpp"
#include "SolidColorBrush.hpp"

#include "AL/Drawing/Rectangle.hpp"

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
		Void DrawString(const String& string, T x, T y)
		{
			::RECT rect;
			rect.left   = static_cast<::LONG>(x);
			rect.right  = static_cast<::LONG>(x);
			rect.top    = static_cast<::LONG>(y);
			rect.bottom = static_cast<::LONG>(y);

			if (::DrawTextA(GetHandle(), string.GetCString(), string.GetLength(), &rect, DT_LEFT | DT_NOCLIP) == 0)
			{

				throw GDIException(
					"DrawTextA"
				);
			}
		}
		// @throw AL::Exception
		template<typename T>
		Void DrawString(const String& string, const Drawing::Rectangle<T>& rectangle)
		{
			::RECT rect;
			rect.left   = static_cast<::LONG>(rectangle.Left);
			rect.right  = static_cast<::LONG>(rectangle.Right);
			rect.top    = static_cast<::LONG>(rectangle.Top);
			rect.bottom = static_cast<::LONG>(rectangle.Bottom);

			if (::DrawTextA(GetHandle(), string.GetCString(), string.GetLength(), &rect, DT_LEFT) == 0)
			{

				throw GDIException(
					"DrawTextA"
				);
			}
		}

		// @throw AL::Exception
		template<typename T>
		Void FillRectangle(const Drawing::Rectangle<T>& rectangle, const SolidColorBrush& brush)
		{
			::RECT rect;
			rect.left   = static_cast<::LONG>(rectangle.Left);
			rect.right  = static_cast<::LONG>(rectangle.Right);
			rect.top    = static_cast<::LONG>(rectangle.Top);
			rect.bottom = static_cast<::LONG>(rectangle.Bottom);

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
