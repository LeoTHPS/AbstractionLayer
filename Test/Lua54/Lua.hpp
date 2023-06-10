#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Lua54/Lua.hpp>

static AL::Lua54::State                         AL_Lua54_State;
static AL::Lua54::Function::LuaCallback<void()> AL_Lua54_Callback;

static void AL_Lua54_do_the_thing()
{
	using namespace AL;
	using namespace AL::Lua54;

	auto the_thing = AL_Lua54_State.GetGlobal<uint32>(
		"the_thing"
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		"[do_the_thing] the_thing = %lu",
		the_thing
	);
#endif

	AL_Lua54_State.SetGlobal(
		"the_thing",
		the_thing + 1
	);
}

static bool AL_Lua54_get_the_thing()
{
	return true;
}

static void AL_Lua54_say_the_thing(const char* message)
{
	AL::OS::Console::WriteLine(
		message
	);
}

// TODO: fix param type
// static void AL_Lua54_say_the_thing2(const AL::String& message)
// {
// 	AL::OS::Console::WriteLine(
// 		message
// 	);
// }

static void AL_Lua54_call_the_thing(AL::Lua54::Function::LuaCallback<void()> callback)
{
	callback();
}

static void AL_Lua54_do_the_call()
{
	AL_Lua54_Callback();
}

static void AL_Lua54_prepare_the_call(AL::Lua54::Function::LuaCallback<void()> callback)
{
	AL_Lua54_Callback = AL::Move(
		callback
	);
}

// @throw AL::Exception
static void AL_Lua54()
{
	using namespace AL;
	using namespace AL::Lua54;

	AL_Lua54_State.Create();

	AL_Lua54_State.LoadLibrary(
		Libraries::All
	);

	try
	{
		AL_Lua54_State.SetGlobalFunction<AL_Lua54_do_the_thing>(
			"do_the_thing"
		);

		AL_Lua54_State.SetGlobalFunction<AL_Lua54_get_the_thing>(
			"get_the_thing"
		);

		AL_Lua54_State.SetGlobalFunction<AL_Lua54_call_the_thing>(
			"call_the_thing"
		);

		AL_Lua54_State.SetGlobalFunction<AL_Lua54_say_the_thing>(
			"say_the_thing"
		);

		// AL_Lua54_State.SetGlobalFunction<AL_Lua54_say_the_thing2>(
		// 	"say_the_thing2"
		// );

		AL_Lua54_State.SetGlobalFunction<AL_Lua54_do_the_call>(
			"do_the_call"
		);

		AL_Lua54_State.SetGlobalFunction<AL_Lua54_prepare_the_call>(
			"prepare_the_call"
		);

		AL_Lua54_State.Run(
			"the_thing = 0;"                    "\n"
			""                                  "\n"
			"while (the_thing ~= 10) do"        "\n"
			    "call_the_thing(do_the_thing);" "\n"
			"end"                               "\n"
			                                    "\n"
			"prepare_the_call(do_the_thing);"   "\n"
			"do_the_call();"                    "\n"
			"print(type(get_the_thing()), get_the_thing());"
		);
	}
	catch (Exception&)
	{
		AL_Lua54_State.Destroy();

		throw;
	}

	AL_Lua54_State.Destroy();
}
