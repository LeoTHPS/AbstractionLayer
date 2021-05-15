#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	public ref class Timer
	{
		AL::OS::Timer* const lpTimer;

	public:
		Timer()
			: lpTimer(
				new AL::OS::Timer()
			)
		{
		}

		virtual ~Timer()
		{
			delete lpTimer;
		}

		auto GetElapsed()
		{
			auto elapsed = lpTimer->GetElapsed();
			auto milliseconds = elapsed.ToMilliseconds();
			auto millisecondFractionInUS = elapsed.ToMicroseconds() - (milliseconds * 1000);

			return System::TimeSpan::FromMilliseconds(
				(milliseconds * 1.0) + (millisecondFractionInUS / 1000.0)
			);
		}

		void Reset()
		{
			lpTimer->Reset();
		}
	};
}
