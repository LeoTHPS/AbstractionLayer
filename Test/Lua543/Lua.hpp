#pragma once
#include "AL/Common.hpp"

#include "AL/Lua543/Lua.hpp"

#include "AL/OS/Console.hpp"

static void AL_Lua543()
{
	using namespace AL;
	using namespace AL::Lua543;

	State lua;
	lua.GetStack().Push(1);
	lua.GetStack().Pop();
}
