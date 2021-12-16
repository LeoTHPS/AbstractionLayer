#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

// @throw AL::Exception
static void AL_Function()
{
	using namespace AL;
	using namespace AL::OS;

	Function<void()> func(
		[]()
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			Console::WriteLine(
				__FUNCTION__
			);
#endif
		}
	);

	func();
}
