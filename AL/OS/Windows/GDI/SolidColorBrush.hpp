#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "GDIException.hpp"

#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"

namespace AL::OS::Windows::GDI
{
	class SolidColorBrush
	{
		Drawing::Color color;
		::HBRUSH       hBrush;

		SolidColorBrush(const SolidColorBrush&) = delete;

		SolidColorBrush(Drawing::Color color, ::HBRUSH hBrush)
			: color(
				color
			),
			hBrush(
				hBrush
			)
		{
		}

	public:
		// @throw AL::Exception
		static SolidColorBrush Create(Drawing::Color color)
		{
			::HBRUSH hBrush;

			if ((hBrush = ::CreateSolidBrush(RGB(color.R, color.G, color.B))) == NULL)
			{

				throw GDIException(
					"CreateSolidBrush"
				);
			}

			return SolidColorBrush(
				color,
				hBrush
			);
		}
		// @throw AL::Exception
		static SolidColorBrush Create(Drawing::Colors color)
		{
			auto solidColorBrush = Create(
				Drawing::Color(color)
			);

			return solidColorBrush;
		}

		SolidColorBrush()
			: hBrush(
				NULL
			)
		{
		}

		SolidColorBrush(SolidColorBrush&& solidColorBrush)
			: hBrush(
				solidColorBrush.hBrush
			)
		{
			solidColorBrush.hBrush = NULL;
		}

		virtual ~SolidColorBrush()
		{
			if (hBrush != NULL)
			{
				::DeleteObject(
					hBrush
				);
			}
		}

		auto GetColor() const
		{
			return color;
		}

		auto GetHandle() const
		{
			return hBrush;
		}

		Void Destroy()
		{
			if (hBrush != NULL)
			{
				::DeleteObject(
					hBrush
				);

				hBrush = NULL;
			}
		}

		SolidColorBrush& operator = (SolidColorBrush&& solidColorBrush)
		{
			if (hBrush != NULL)
			{
				::DeleteObject(
					hBrush
				);
			}

			hBrush = solidColorBrush.hBrush;
			solidColorBrush.hBrush = NULL;

			return *this;
		}

		Bool operator == (const SolidColorBrush& solidColorBrush) const
		{
			if (hBrush != solidColorBrush.hBrush)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const SolidColorBrush& solidColorBrush) const
		{
			if (operator==(solidColorBrush))
			{

				return False;
			}

			return True;
		}
	};
}
