#pragma once
#include "AL/Common.hpp"

#include "MouseEvents.hpp"
#include "MouseButtons.hpp"
#include "MousePosition.hpp"

namespace AL::Input
{
	struct MouseEvent
	{
		MouseEvents   Event;

		MouseButtons  Button;
		MousePosition Position;
	};
}
