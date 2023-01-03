#pragma once
#include "AL/Common.hpp"

#if !AL_HAS_INCLUDE(<lua.hpp>)
	#error Missing lua.hpp
#endif

#include <lua.hpp>

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
				::lua_tostring(lua, -1)
			)
		{
		}
	};
}
