#pragma once
#include "AL/Common.hpp"

#if AL_HAS_INCLUDE(<lua.hpp>)
	#define AL_DEPENDENCY_LUA

	#include <lua.hpp>
#else
	struct lua_State;
#endif

namespace AL::Lua543
{
	class LuaException
		: public Exception
	{
	public:
		template<size_t S>
		explicit LuaException(const char(&function)[S])
			: Exception(
				"Error calling '%s'",
				&function[0]
			)
		{
		}

		template<size_t S>
		LuaException(::lua_State* lua, const char(&function)[S])
			: Exception(
				"Error calling '%s': %s",
				&function[0],
#if defined(AL_DEPENDENCY_LUA)
				::lua_tostring(lua, -1)
#else
				"Not implemented"
#endif
			)
		{
		}
	};
}
