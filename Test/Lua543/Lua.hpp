#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Lua543/Lua.hpp>

static AL::Lua543::State AL_Lua543_State;

static void AL_Lua543_do_the_thing()
{
	using namespace AL;
	using namespace AL::Lua543;

	auto the_thing = AL_Lua543_State.GetGlobal<uint32>(
		"the_thing"
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		"[do_the_thing] the_thing = %lu",
		the_thing
	);
#endif

	AL_Lua543_State.SetGlobal(
		"the_thing",
		the_thing + 1
	);
}

// @throw AL::Exception
static void AL_Lua543()
{
	using namespace AL;
	using namespace AL::API::Lua543;

	AL_Lua543_State.Create();

	try
	{
		AL_Lua543_State.SetGlobalFunction<AL_Lua543_do_the_thing>(
			"do_the_thing"
		);

		AL_Lua543_State.Run(
			"the_thing = 0;"             "\n"
			""                           "\n"
			"while (the_thing ~= 10) do" "\n"
			    "do_the_thing();"        "\n"
			"end"
		);
	}
	catch (Exception&)
	{
		AL_Lua543_State.Destroy();

		throw;
	}

	AL_Lua543_State.Destroy();
}
