#pragma once
#include "AL/Common.hpp"

#include "Keys.hpp"
#include "KeyboardEvents.hpp"

namespace AL::Input
{
	struct KeyboardEvent
	{
		KeyboardEvents Event;

		union
		{
			Keys Key;
			char Char;
		};
	};
}
