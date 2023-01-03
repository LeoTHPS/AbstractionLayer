#pragma once
#include "AL/Common.hpp"

#include "InputDeviceState.hpp"

#include "AL/Drawing/Vector.hpp"

namespace AL::Hardware
{
	enum class MouseEvents : uint8
	{
		Move,
		ButtonUp,
		ButtonDown,
		ScrollUp,
		ScrollDown
	};

	enum class MouseButtons : uint8
	{
		Left,
		Right,
		Middle,
		X1,
		X2,

		COUNT
	};

	typedef Drawing::Vector2U32 MousePosition;

	struct MouseEvent
	{
		MouseEvents   Type;

		MouseButtons  Button;
		MousePosition Position;
	};

	typedef InputDeviceState<MouseButtons, static_cast<size_t>(MouseButtons::COUNT)> MouseState;
}
