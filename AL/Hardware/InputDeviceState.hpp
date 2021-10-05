#pragma once
#include "AL/Common.hpp"

namespace AL::Hardware
{
	template<typename T_BUTTON, size_t COUNT>
	class InputDeviceState
	{
		struct Context
		{
			Bool Down = False;
			Bool DownPrevious = False;
		};

		Context contexts[COUNT];

		InputDeviceState(InputDeviceState&&) = delete;
		InputDeviceState(const InputDeviceState&) = delete;

	public:
		typedef T_BUTTON Button;

		InputDeviceState()
		{
		}

		virtual ~InputDeviceState()
		{
		}

		Bool IsUp(Button button) const
		{
			return !contexts[static_cast<size_t>(button)].Down;
		}

		Bool IsDown(Button button) const
		{
			return contexts[static_cast<size_t>(button)].Down;
		}

		Bool JustPressed(Button button) const
		{
			auto& context = contexts[static_cast<size_t>(button)];

			return context.Down && !context.DownPrevious;
		}

		Bool JustReleased(Button button) const
		{
			auto& context = contexts[static_cast<size_t>(button)];

			return !context.Down && context.DownPrevious;
		}

		Void SetPressed(Button button, Bool set = True)
		{
			auto& context = contexts[static_cast<size_t>(button)];

			context.DownPrevious = context.Down;
			context.Down = set;
		}

		Void Reset()
		{
			memset(
				&contexts[0],
				0,
				sizeof(Context) * COUNT
			);
		}
	};
}
