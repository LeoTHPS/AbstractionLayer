#pragma once
#include "AL/Common.hpp"

namespace AL::Drawing
{
	enum class Colors : uint32
	{
		//                 AARRGGBB
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
}
