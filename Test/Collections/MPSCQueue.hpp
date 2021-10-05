#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

#include "AL/Collections/MPSCQueue.hpp"

static void AL_Collections_MPSCQueue()
{
	using namespace AL;
	using namespace AL::Collections;

	MPSCQueue<uint32> queue;

	for (uint32 i = 0; i < 100; ++i)
	{
		queue.Enqueue(
			i
		);
	}

	uint32 value;

	while (queue.Dequeue(value))
	{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"value = %lu",
			value
		);
#endif
	}
}
