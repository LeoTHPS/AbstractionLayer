#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

#include "AL/Collections/Dictionary.hpp"

static void AL_Collections_Dictionary()
{
	using namespace AL;
	using namespace AL::Collections;

	Dictionary<uint32, uint32> dictionary;

	for (uint32 i = 0, x = 0, y = 10; i < 10; ++i, ++x, --y)
	{
		dictionary[x] = y;

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"+%lu => %lu",
			x,
			y
		);
#endif
	}

	for (auto it = dictionary.begin(); it != dictionary.end(); )
	{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"-%lu => %lu",
			it->Key,
			it->Value
		);
#endif

		dictionary.Erase(
			it++
		);
	}
}
