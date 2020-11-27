#pragma once
#include "AL/Common.hpp"

namespace AL::Input
{
	template<typename BUTTON, size_t COUNT, typename EVENT>
	class InputState
	{
		struct Context
		{
			bool Down = false;
			bool DownPrevious = false;
		};

		Context contexts[COUNT];

		InputState(InputState&&) = delete;
		InputState(const InputState&) = delete;

	public:
		typedef EVENT Event;

		typedef BUTTON Button;

		InputState()
		{
		}

		bool IsUp(Button button) const
		{
			return !contexts[static_cast<size_t>(button)].Down;
		}

		bool IsDown(Button button) const
		{
			return contexts[static_cast<size_t>(button)].Down;
		}

		bool JustPressed(Button button) const
		{
			auto& context = contexts[static_cast<size_t>(button)];

			return context.Down && !context.DownPrevious;
		}

		bool JustReleased(Button button) const
		{
			auto& context = contexts[static_cast<size_t>(button)];

			return !context.Down && context.DownPrevious;
		}

		void SetPressed(Button button, bool set = true)
		{
			auto& context = contexts[static_cast<size_t>(button)];

			context.DownPrevious = context.Down;
			context.Down = set;
		}

		void Reset()
		{
			memset(
				&contexts[0],
				0,
				sizeof(Context) * COUNT
			);
		}
	};
}
