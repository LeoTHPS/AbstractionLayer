#pragma once
#include "AL/Common.hpp"

#if AL_HAS_INCLUDE(<lua.hpp>)
	#include <lua.hpp>
#elif AL_HAS_INCLUDE(<lua5.4/lua.hpp>)
	#include <lua5.4/lua.hpp>
#else
	#error Missing lua.hpp
#endif

namespace AL::Lua54
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
				::lua_tostring(lua, -1)
			)
		{
		}
	};
}
