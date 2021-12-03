#pragma once
#include "AL/Common.hpp"

#include "LuaException.hpp"

#if __has_include(<lua.hpp>)
	#define AL_DEPENDENCY_LUA

	#include <lua.hpp>
#else
	#define LUA_INT_INT             1
	#define LUA_INT_LONG            2
	#define LUA_INT_LONGLONG        3
	#define LUA_INT_TYPE            LUA_INT_LONGLONG

	#if LUA_INT_TYPE == LUA_INT_INT
		typedef int                 lua_Integer;
	#elif LUA_INT_TYPE == LUA_INT_LONG
		typedef long                lua_Integer;
	#elif LUA_INT_TYPE == LUA_INT_LONGLONG
		typedef long long           lua_Integer;
	#endif

	#define LUA_FLOAT_FLOAT         1
	#define LUA_FLOAT_DOUBLE        2
	#define LUA_FLOAT_LONGDOUBLE    3
	#define LUA_FLOAT_TYPE          LUA_FLOAT_DOUBLE

	#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
		typedef float               lua_Number;
	#elif LUA_FLOAT_TYPE == LUA_FLOAT_DOUBLE
		typedef double              lua_Number;
	#elif LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
		typedef long double         lua_Number;
	#endif

	typedef int(*                   lua_CFunction)(lua_State*);

	#define AL_DEPENDENCY_LUA_MISSING_VERSION "Lua 5.4.3"
#endif

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(__type__, __get__, __push__, __pop__) \
	template<> \
	struct AL::Lua543::Extensions::Type_Functions<__type__> \
	{ \
		static constexpr Bool IsAlias   = False; \
		static constexpr Bool IsDefined = True; \
		\
		static constexpr auto Get  = __get__; \
		static constexpr auto Push = __push__; \
		static constexpr auto Pop  = __pop__; \
	}

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(__type__, __alias__, __cast__) \
	template<> \
	struct AL::Lua543::Extensions::Type_Functions<__type__> \
	{ \
		static constexpr Bool IsAlias   = True; \
		static constexpr Bool IsDefined = True; \
		\
		static __type__ Get(::lua_State* lua, size_t index) \
		{ \
			return __cast__<__type__>( \
				Type_Functions<__alias__>::Get( \
					lua, \
					index \
				) \
			); \
		}; \
		static Void Push(::lua_State* lua, __type__ value) \
		{ \
			Type_Functions<__alias__>::Push( \
				lua, \
				__cast__<__alias__>(value) \
			); \
		}; \
		static __type__ Pop(::lua_State* lua, size_t count) \
		{ \
			return __cast__<__type__>( \
				Type_Functions<__alias__>::Pop( \
					lua, \
					count \
				) \
			); \
		}; \
	}

namespace AL::Lua543::Extensions
{
	template<typename T>
	struct Type_Functions
	{
		static constexpr Bool IsDefined = False;
	};

	static ::std::nullptr_t getnil(::lua_State* lua, size_t index)
	{
		return nullptr;
	}
	static char             getchar(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_tointeger(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return static_cast<char>(
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static const char*      getstring(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_tostring(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static String           getString(::lua_State* lua, size_t index)
	{
		auto lpString = getstring(
			lua,
			index
		);

		return String(
			lpString ? lpString : ""
		);
	}
	static Bool             getboolean(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_toboolean(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value == 1;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void*            getlightuserdata(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_touserdata(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static ::lua_Number     getnumber(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_tonumber(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static ::lua_Integer    getinteger(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_tointeger(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static lua_CFunction    getcfunction(::lua_State* lua, size_t index)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = ::lua_tocfunction(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}

	static Void             pushnil(::lua_State* lua, ::std::nullptr_t)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushnil(
			lua
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushchar(::lua_State* lua, char value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushinteger(
			lua,
			static_cast<::lua_Integer>(value)
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushstring(::lua_State* lua, const char* value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushstring(
			lua,
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushString(::lua_State* lua, const String& value)
	{
		pushstring(
			lua,
			value.GetCString()
		);
	}
	static Void             pushboolean(::lua_State* lua, Bool value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushboolean(
			lua,
			value ? 1 : 0
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushlightuserdata(::lua_State* lua, Void* value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushlightuserdata(
			lua,
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushnumber(::lua_State* lua, ::lua_Number value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushnumber(
			lua,
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushinteger(::lua_State* lua, ::lua_Integer value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushinteger(
			lua,
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             pushcfunction(::lua_State* lua, lua_CFunction value)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pushcfunction(
			lua,
			value
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}

	static Void             pop(::lua_State* lua, size_t count)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_pop(
			lua,
			static_cast<int>(count & Integer<int>::SignedCastMask)
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	template<typename T>
	static T                pop(::lua_State* lua, size_t count)
	{
#if defined(AL_DEPENDENCY_LUA)
		auto value = Type_Functions<T>::Get(
			lua,
			1
		);

		pop(
			lua,
			count
		);

		return value;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}

	// @throw AL::Exception
	static ::lua_State*     newstate()
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_State* lua;

		if ((lua = ::luaL_newstate()) == nullptr)
		{

			throw LuaException(
				"luaL_newstate"
			);
		}

		return lua;
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}

	static Void             close(::lua_State* lua)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_close(
			lua
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}

	// @throw AL::Exception
	static Void             dostring(::lua_State* lua, const char* string)
	{
#if defined(AL_DEPENDENCY_LUA)
		if (luaL_dostring(lua, string) != LUA_OK)
		{

			throw LuaException(
				lua,
				"luaL_dostring"
			);
		}
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	// @throw AL::Exception
	static Void             doString(::lua_State* lua, const String& string)
	{
		dostring(
			lua,
			string.GetCString()
		);
	}

	// @throw AL::Exception
	static Void             dofile(::lua_State* lua, const char* path)
	{
#if defined(AL_DEPENDENCY_LUA)
		if (luaL_dofile(lua, path) != LUA_OK)
		{

			throw LuaException(
				lua,
				"luaL_dofile"
			);
		}
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	// @throw AL::Exception
	static Void             doFile(::lua_State* lua, const String& path)
	{
		dofile(
			lua,
			path.GetCString()
		);
	}

	static Void             getglobal(::lua_State* lua, const char* name)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_getglobal(
			lua,
			name
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             getGlobal(::lua_State* lua, const String& name)
	{
		getglobal(
			lua,
			name.GetCString()
		);
	}

	static Void             setglobal(::lua_State* lua, const char* name)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_setglobal(
			lua,
			name
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
	static Void             setGlobal(::lua_State* lua, const String& name)
	{
		setglobal(
			lua,
			name.GetCString()
		);
	}

	static Void             call(::lua_State* lua, size_t argCount, size_t returnCount)
	{
#if defined(AL_DEPENDENCY_LUA)
		::lua_call(
			lua,
			static_cast<int>(argCount & Integer<int>::SignedCastMask),
			static_cast<int>(returnCount & Integer<int>::SignedCastMask)
		);
#else
		throw DependencyMissingException(
			AL_DEPENDENCY_LUA_MISSING_VERSION
		);
#endif
	}
}

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::std::nullptr_t, &AL::Lua543::Extensions::getnil,           &AL::Lua543::Extensions::pushnil,           &AL::Lua543::Extensions::pop<::std::nullptr_t>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::Bool,         &AL::Lua543::Extensions::getboolean,       &AL::Lua543::Extensions::pushboolean,       &AL::Lua543::Extensions::pop<AL::Bool>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::Void*,        &AL::Lua543::Extensions::getlightuserdata, &AL::Lua543::Extensions::pushlightuserdata, &AL::Lua543::Extensions::pop<AL::Void*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_Number,     &AL::Lua543::Extensions::getnumber,        &AL::Lua543::Extensions::pushnumber,        &AL::Lua543::Extensions::pop<::lua_Number>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_Integer,    &AL::Lua543::Extensions::getinteger,       &AL::Lua543::Extensions::pushinteger,       &AL::Lua543::Extensions::pop<::lua_Integer>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(char,             &AL::Lua543::Extensions::getchar,          &AL::Lua543::Extensions::pushchar,          &AL::Lua543::Extensions::pop<char>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(const char*,      &AL::Lua543::Extensions::getstring,        &AL::Lua543::Extensions::pushstring,        &AL::Lua543::Extensions::pop<const char*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::String,       &AL::Lua543::Extensions::getString,        &AL::Lua543::Extensions::pushString,        &AL::Lua543::Extensions::pop<AL::String>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_CFunction,  &AL::Lua543::Extensions::getcfunction,     &AL::Lua543::Extensions::pushcfunction,     &AL::Lua543::Extensions::pop<::lua_CFunction>);

#if LUA_INT_TYPE == LUA_INT_INT
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int8,   ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint8,  ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int16,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint16, ::lua_Integer, static_cast);

//	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int32,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint32, ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int64,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint64, ::lua_Integer, static_cast);
#elif LUA_INT_TYPE == LUA_INT_LONG
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int8,   ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint8,  ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int16,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint16, ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int32,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint32, ::lua_Integer, static_cast);

//	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int64,  ::lua_Integer, static_cast);
//	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint64, ::lua_Integer, static_cast);
#elif LUA_INT_TYPE == LUA_INT_LONGLONG
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int8,   ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint8,  ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int16,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint16, ::lua_Integer, static_cast);

	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int32,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint32, ::lua_Integer, static_cast);

//	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int64,  ::lua_Integer, static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint64, ::lua_Integer, static_cast);
#endif

#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
//	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::Double, ::lua_Number,  static_cast);
#elif LUA_FLOAT_TYPE == LUA_FLOAT_DOUBLE
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::Float,  ::lua_Number,  static_cast);
#elif LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::Float,  ::lua_Number,  static_cast);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::Double, ::lua_Number), static_cast;
#endif

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(char*,          const char*,   const_cast);
