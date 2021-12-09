#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Window.hpp>
#include <AL/OS/Console.hpp>

static void AL_OS_Window()
{
	using namespace AL;
	using namespace AL::OS;

	Window window(
		"MainWindow",
		"AL::OS::Window"
	);

	window.Open();

	Timer timer;

	while (window.IsOpen())
	{
		if (!window.Update(timer.GetElapsed()))
		{

			break;
		}

		timer.Reset();

		Sleep(
			TimeSpan::FromMilliseconds(10)
		);
	}

	window.UnloadContent();
}
