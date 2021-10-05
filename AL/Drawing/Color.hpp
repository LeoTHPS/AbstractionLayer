#pragma once
#include "AL/Common.hpp"

#include "Colors.hpp"

namespace AL::Drawing
{
	struct Color
	{
		union
		{
			struct
			{
				uint8 B;
				uint8 G;
				uint8 R;
				uint8 A;
			};

			uint32 ARGB;
		};

		Color()
			: Color(
				0
			)
		{
		}

		Color(uint32 argb)
			: ARGB(
				argb
			)
		{
		}

		Color(Colors color)
			: Color(
				static_cast<uint32>(color)
			)
		{
		}

		Color(Colors color, uint8 a)
			: Color(
				color
			)
		{
			A = a;
		}

		Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF)
			: B(
				b
			),
			G(
				g
			),
			R(
				r
			),
			A(
				a
			)
		{
		}

		virtual ~Color()
		{
		}

		operator uint32& ()
		{
			return ARGB;
		}
		operator uint32 () const
		{
			return ARGB;
		}

		Bool operator == (Color color) const
		{
			if (ARGB != color.ARGB)
			{

				return False;
			}

			return True;
		}
		Bool operator != (Color color) const
		{
			if (operator==(color))
			{

				return False;
			}

			return True;
		}
	};
}
