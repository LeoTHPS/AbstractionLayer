#pragma once
#include "AL/Common.hpp"

#include "InputState.hpp"

#include "Keys.hpp"
#include "KeyboardEvent.hpp"

namespace AL::Input
{
	typedef InputState<Keys, static_cast<size_t>(Keys::COUNT), KeyboardEvent> KeyboardState;
}
