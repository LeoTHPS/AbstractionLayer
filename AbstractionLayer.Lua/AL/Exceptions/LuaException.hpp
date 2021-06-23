#pragma once
#include "AL/Lua/Common.hpp"

namespace AL::Exceptions
{
	class LuaException
		: public Exception
	{
	public:
		template<size_t S>
		LuaException(Lua::State& state, const char(&function)[S])
			: Exception(
				String::Format(
					"Error calling '%s': %s",
					&function[0],
					lua_tostring(state.GetHandle(), -1)
				)
			)
		{
		}
	};
}
