#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

#include "AL/Collections/ArrayList.hpp"

static void AL_Collections_ArrayList()
{
	using namespace AL;
	using namespace AL::Collections;

	ArrayList<uint32> array(100);

	for (uint32 i = 0; i < 100; ++i)
	{
		array.PushBack(
			i
		);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"+%lu",
			i
		);
#endif
	}

	for (auto it = array.rbegin(); it != array.rend(); ++it)
	{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"-%lu",
			*it
		);
#endif

		array.PopBack();
	}
}
