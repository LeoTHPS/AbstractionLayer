#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

#include "AL/Collections/LinkedList.hpp"

static void AL_Collections_LinkedList()
{
	using namespace AL;
	using namespace AL::Collections;

	LinkedList<uint32> list;

	for (uint32 i = 0; i < 100; ++i)
	{
		list.PushBack(
			i
		);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"+%lu",
			i
		);
#endif
	}

	for (auto it = list.begin(); it != list.end(); )
	{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"-%lu",
			*it
		);
#endif

		list.Erase(
			it++
		);
	}
}
