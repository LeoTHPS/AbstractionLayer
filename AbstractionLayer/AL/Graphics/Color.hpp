#pragma once
#include "AL/Common.hpp"

namespace AL::Graphics
{
	enum class Colors : uint32
	{
		White          = 0xFFFFFFFF,
		Black          = 0xFF000000,
		Gray           = 0xFF808080,
		Red            = 0xFFFF0000,
		Green          = 0xFF00FF00,
		Blue           = 0xFF0000FF,
		Orange         = 0xFFFF7F00,
		Yellow         = 0xFFFFFF00,
		Chartreuse     = 0xFF7FFF00,
		SpringGreen    = 0xFF00FF7F,
		Cyan           = 0xFF00FFFF,
		Azure          = 0xFF007FFF,
		Violet         = 0xFF7F00FF,
		Magenta        = 0xFFFF00FF,
		Rose           = 0xFFFF007F,
		CornflowerBlue = 0xFF6495ED,

		Transparent    = 0x00000000
	};

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
			: Color(0)
		{
		}

		Color(uint32 argb)
			: ARGB(argb)
		{
		}

		Color(Colors color)
			: Color(
				static_cast<uint32>(color)
			)
		{
		}

		Color(Colors color, uint8 a)
			: Color(color)
		{
			A = a;
		}

		Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF)
			: R(r),
			G(g),
			B(b),
			A(a)
		{
		}

		operator uint32&()
		{
			return ARGB;
		}
		operator uint32() const
		{
			return ARGB;
		}

		bool operator == (Color color) const
		{
			return ARGB == color.ARGB;
		}
		bool operator != (Color color) const
		{
			return !operator==(color);
		}
	};
}
