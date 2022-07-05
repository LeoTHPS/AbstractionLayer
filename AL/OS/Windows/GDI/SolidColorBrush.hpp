#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

#include "GDIException.hpp"

#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"

namespace AL::OS::Windows::GDI
{
	class SolidColorBrush
	{
		::HBRUSH hBrush;

		SolidColorBrush(const SolidColorBrush&) = delete;

		explicit SolidColorBrush(::HBRUSH hBrush)
			: hBrush(
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

		auto GetHandle() const
		{
			return hBrush;
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
