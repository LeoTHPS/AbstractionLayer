#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"
#include "AL/OS/ThreadPool.hpp"

static void AL_OS_ThreadPool()
{
	using namespace AL;
	using namespace AL::OS;

	ThreadPool pool(
		10
	);

	try
	{
		pool.Start();
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error starting ThreadPool"
		);
	}

	for (uint32 i = 0; i < 10; ++i)
	{
		pool.Post(
			[](uint32 _i)
			{
				Sleep(
					TimeSpan::FromMilliseconds(1)
				);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
				Console::WriteLine(
					"[Thread.ID: %lu] i = %lu",
					OS::GetCurrentThreadId(),
					_i
				);
#endif
			},
			i
		);
	}

	pool.Stop();
}
