#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

static void AL_Collections_String()
{
	using namespace AL;

	String string;
	string.Append("Hello World!");

	if (string.Contains(' ', true))
	{
		auto chunks = string.Split(
			' '
		);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		for (auto& chunk : chunks)
		{
			OS::Console::WriteLine(
				chunk
			);
		}
#endif
	}

	string.SetCapacity(
		1000
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		"string.GetSize() = %llu, string.GetLength() = %llu, string.GetCapacity() = %llu",
		string.GetSize(),
		string.GetLength(),
		string.GetCapacity()
	);
#endif

	string.ShrinkToFit();

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		"string.GetSize() = %llu, string.GetLength() = %llu, string.GetCapacity() = %llu",
		string.GetSize(),
		string.GetLength(),
		string.GetCapacity()
	);
#endif
}
