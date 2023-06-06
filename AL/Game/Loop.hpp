#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Timer.hpp"

namespace AL::Game
{
	class Loop
	{
		Loop() = delete;

	public:
		// @throw AL::Exception
		template<typename F_ON_UPDATE>
		static Void Run(uint32 tickRate, F_ON_UPDATE&& onUpdate)
		{
			auto tickInterval = TimeSpan::FromSeconds(1) / tickRate;

			TimeSpan  time;
			TimeSpan  delta;
			OS::Timer timer;
			TimeSpan  nextTick;

			while (onUpdate(delta))
			{
				delta     = tickInterval;
				nextTick += tickInterval;

				if ((time = timer.GetElapsed()) < nextTick)
				{
					Sleep(
						nextTick - time
					);
				}
			}
		}
		// @throw AL::Exception
		template<typename F_ON_UPDATE, typename F_ON_DRAW>
		static Void Run(uint32 tickRate, F_ON_UPDATE&& onUpdate, F_ON_DRAW&& onDraw)
		{
			auto tickInterval = TimeSpan::FromSeconds(1) / tickRate;

			TimeSpan  time;
			TimeSpan  delta;
			OS::Timer timer;
			TimeSpan  nextTick;

			while (onUpdate(delta))
			{
				delta     = tickInterval;
				nextTick += tickInterval;

				while ((time = timer.GetElapsed()) < nextTick)
				{
					if (!onDraw(1.0f - ((nextTick - time).ToMicroseconds() / 1000000.0f)))
					{

						return;
					}
				}
			}
		}
	};
}
