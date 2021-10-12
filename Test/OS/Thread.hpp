#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Thread.hpp"
#include "AL/OS/Console.hpp"

static void AL_OS_Thread()
{
	using namespace AL;
	using namespace AL::OS;

	Thread thread;

	try
	{
		thread.Start(
			[]()
			{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
				Console::WriteLine(
					"GetCurrentThreadId() = %lu",
					OS::GetCurrentThreadId()
				);
#endif
			}
		);
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error starting Thread"
		);
	}

	if (thread.IsRunning())
	{
		try
		{
			thread.Join();
		}
		catch (Exception& exception)
		{

			throw Exception(
				Move(exception),
				"Error joining Thread"
			);
		}
	}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	Console::WriteLine(
		"thread.IsRunning() = %s",
		ToString(thread.IsRunning()).GetCString()
	);
#endif
}
