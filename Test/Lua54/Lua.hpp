#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Lua54/Lua.hpp>

static AL::Lua54::Lua              AL_Lua54_Lua;
static AL::Lua54::Function<void()> AL_Lua54_Callback;

static void AL_Lua54_do_the_thing()
{
	auto the_thing = AL_Lua54_Lua.GetGlobal<AL::uint32>(
		"the_thing"
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	AL::OS::Console::WriteLine(
		"[do_the_thing] the_thing = %lu",
		the_thing
	);
#endif

	AL_Lua54_Lua.SetGlobal(
		"the_thing",
		the_thing + 1
	);
}

static auto AL_Lua54_get_the_thing()
{
	return true;
}

static void AL_Lua54_say_the_thing(const char* message)
{
	AL::OS::Console::WriteLine(
		message
	);
}

static void AL_Lua54_say_the_thing2(const AL::String& message)
{
	AL::OS::Console::WriteLine(
		message
	);
}

static void AL_Lua54_call_the_thing(AL::Lua54::Function<void()> callback)
{
	callback();
}

static void AL_Lua54_do_the_call()
{
	AL_Lua54_Callback();
}

static void AL_Lua54_prepare_the_call(AL::Lua54::Function<void()> callback)
{
	AL_Lua54_Callback = AL::Move(
		callback
	);
}

static auto AL_Lua54_do_the_thing_via(AL::Lua54::Function<int(int, int)> func, int a, int b)
{
	return func(a, b);
}

static void AL_Lua54_test_args_c(int a, int b, int c)
{
	AL::OS::Console::WriteLine("test_args_c");
	AL::OS::Console::WriteLine("%i, %i, %i", a, b, c);
}

// @throw AL::Exception
static void AL_Lua54()
{
	using namespace AL;
	using namespace AL::Lua54;

	AL_Lua54_Lua.Create();

	AL_Lua54_Lua.LoadLibrary(
		Libraries::All
	);

	try
	{
		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_do_the_thing>(
			"do_the_thing"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_get_the_thing>(
			"get_the_thing"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_call_the_thing>(
			"call_the_thing"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_say_the_thing>(
			"say_the_thing"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_say_the_thing2>(
			"say_the_thing2"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_do_the_call>(
			"do_the_call"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_prepare_the_call>(
			"prepare_the_call"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_do_the_thing_via>(
			"do_the_thing_via"
		);

		AL_Lua54_Lua.SetGlobalFunction<AL_Lua54_test_args_c>(
			"test_args_c"
		);

		AL_Lua54_Lua.Run(
			"function test_args(a, b, c)"                       "\n"
			"	print('test_args');"                            "\n"
			"	print(a, b, c);"                                "\n"
			"	print(type(a), type(b), type(c));"              "\n"
			"end"                                               "\n"
			                                                    "\n"
			"test_args_c(1, 2, 3);"                             "\n"
			                                                    "\n"
			"the_thing = 0;"                                    "\n"
			                                                    "\n"
			"while (the_thing ~= 10) do"                        "\n"
			"	call_the_thing(do_the_thing);"                  "\n"
			"end"                                               "\n"
			                                                    "\n"
			"prepare_the_call(do_the_thing);"                   "\n"
			"do_the_call();"                                    "\n"
			                                                    "\n"
			"function add_the_thing(a, b)"                      "\n"
			"	print(a, b);"                                   "\n"
			"	return a + b;"                                  "\n"
			"end"                                               "\n"
			                                                    "\n"
			"the_thing = do_the_thing_via(add_the_thing, 1, 2)" "\n"
			"print(type(the_thing), the_thing);"                "\n"
		);

		AL_Lua54_Lua.CallGlobalFunction<void(int, int, int)>(
			"test_args",
			1,
			2,
			3
		);
	}
	catch (AL::Exception&)
	{
		AL_Lua54_Lua.Destroy();

		throw;
	}

	AL_Lua54_Callback.Release();
	AL_Lua54_Lua.Destroy();
}
