#pragma once
#include "AL/Common.hpp"

#include "InputState.hpp"

#include "MouseEvent.hpp"
#include "MouseButtons.hpp"

namespace AL::Input
{
	typedef InputState<MouseButtons, static_cast<size_t>(MouseButtons::COUNT), MouseEvent> MouseState;
}
