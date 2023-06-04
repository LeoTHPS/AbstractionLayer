#pragma once
#include "AL/Common.hpp"

#include "LuaException.hpp"

#if !AL_HAS_INCLUDE(<lua.hpp>)
	#error Missing lua.hpp
#endif

#include <lua.hpp>

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(__type__, __get__, __push__, __pop__) \
	template<> \
	struct AL::Lua54::Extensions::Type_Functions<__type__> \
	{ \
		static constexpr Bool IsAlias    = False; \
		static constexpr Bool IsDefined  = True; \
		static constexpr Bool IsExplicit = True; \
		\
		static constexpr auto Get  = __get__; \
		static constexpr auto Push = __push__; \
		static constexpr auto Pop  = __pop__; \
	}

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(__type__, __alias__, __cast__) \
	template<> \
	struct AL::Lua54::Extensions::Type_Functions<__type__> \
	{ \
		static constexpr Bool IsAlias    = True; \
		static constexpr Bool IsDefined  = True; \
		static constexpr Bool IsExplicit = True; \
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

namespace AL::Lua54::Extensions
{
	template<typename T>
	class Type_Functions
	{
		static constexpr Bool IsConst          = Is_Const<T>::Value;
		static constexpr Bool IsPointer        = Is_Pointer<T>::Value;
		static constexpr Bool IsReference      = Is_Reference<T>::Value;
		static constexpr Bool IsEnumOrInteger  = Is_Enum_Or_Integer<T>::Value;
		static constexpr Bool IsConstPointer   = IsConst && IsPointer;
		static constexpr Bool IsConstReference = IsConst && IsReference;

	public:
		static constexpr Bool IsAlias    = False;
		static constexpr Bool IsDefined  = IsPointer || IsReference || IsEnumOrInteger;
		static constexpr Bool IsExplicit = False;

		static T Get(::lua_State* lua, size_t index);

		static Void Push(::lua_State* lua, T value);

		static T Pop(::lua_State* lua, size_t count);
	};

	static ::std::nullptr_t getnil(::lua_State* lua, size_t index)
	{
		return nullptr;
	}
	static char             getchar(::lua_State* lua, size_t index)
	{
		auto value = ::lua_tointeger(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return static_cast<char>(
			value
		);
	}
	static const char*      getstring(::lua_State* lua, size_t index)
	{
		auto value = ::lua_tostring(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
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
		auto value = ::lua_toboolean(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value == 1;
	}
	static Void*            getlightuserdata(::lua_State* lua, size_t index)
	{
		auto value = ::lua_touserdata(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
	}
	static ::lua_Number     getnumber(::lua_State* lua, size_t index)
	{
		auto value = ::lua_tonumber(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
	}
	static ::lua_Integer    getinteger(::lua_State* lua, size_t index)
	{
		auto value = ::lua_tointeger(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
	}
	static lua_CFunction    getcfunction(::lua_State* lua, size_t index)
	{
		auto value = ::lua_tocfunction(
			lua,
			static_cast<int>(index & Integer<int>::SignedCastMask)
		);

		return value;
	}

	static Void             pushnil(::lua_State* lua, ::std::nullptr_t)
	{
		::lua_pushnil(
			lua
		);
	}
	static Void             pushchar(::lua_State* lua, char value)
	{
		::lua_pushinteger(
			lua,
			static_cast<::lua_Integer>(value)
		);
	}
	static Void             pushstring(::lua_State* lua, const char* value)
	{
		::lua_pushstring(
			lua,
			value
		);
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
		::lua_pushboolean(
			lua,
			value ? 1 : 0
		);
	}
	static Void             pushlightuserdata(::lua_State* lua, Void* value)
	{
		::lua_pushlightuserdata(
			lua,
			value
		);
	}
	static Void             pushnumber(::lua_State* lua, ::lua_Number value)
	{
		::lua_pushnumber(
			lua,
			value
		);
	}
	static Void             pushinteger(::lua_State* lua, ::lua_Integer value)
	{
		::lua_pushinteger(
			lua,
			value
		);
	}
	static Void             pushcfunction(::lua_State* lua, lua_CFunction value)
	{
		::lua_pushcfunction(
			lua,
			value
		);
	}

	static Void             pop(::lua_State* lua, size_t count)
	{
		::lua_pop(
			lua,
			static_cast<int>(count & Integer<int>::SignedCastMask)
		);
	}
	template<typename T>
	static T                pop(::lua_State* lua, size_t count)
	{
		auto value = Type_Functions<T>::Get(
			lua,
			1
		);

		pop(
			lua,
			count
		);

		return value;
	}

	// @throw AL::Exception
	static ::lua_State*     newstate()
	{
		::lua_State* lua;

		if ((lua = ::luaL_newstate()) == nullptr)
		{

			throw LuaException(
				"luaL_newstate"
			);
		}

		return lua;
	}

	static Void             close(::lua_State* lua)
	{
		::lua_close(
			lua
		);
	}

	// @throw AL::Exception
	static Void             dostring(::lua_State* lua, const char* string)
	{
		if (luaL_dostring(lua, string) != LUA_OK)
		{

			throw LuaException(
				lua,
				"luaL_dostring"
			);
		}
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
		if (luaL_dofile(lua, path) != LUA_OK)
		{

			throw LuaException(
				lua,
				"luaL_dofile"
			);
		}
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
		::lua_getglobal(
			lua,
			name
		);
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
		::lua_setglobal(
			lua,
			name
		);
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
		::lua_call(
			lua,
			static_cast<int>(argCount & Integer<int>::SignedCastMask),
			static_cast<int>(returnCount & Integer<int>::SignedCastMask)
		);
	}
	static Void             pcall(::lua_State* lua, size_t argCount, size_t returnCount)
	{
		::lua_pcall(
			lua,
			static_cast<int>(argCount & Integer<int>::SignedCastMask),
			static_cast<int>(returnCount & Integer<int>::SignedCastMask),
			0
		);
	}
}

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::std::nullptr_t, &AL::Lua54::Extensions::getnil,           &AL::Lua54::Extensions::pushnil,           &AL::Lua54::Extensions::pop<::std::nullptr_t>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::Bool,         &AL::Lua54::Extensions::getboolean,       &AL::Lua54::Extensions::pushboolean,       &AL::Lua54::Extensions::pop<AL::Bool>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::Void*,        &AL::Lua54::Extensions::getlightuserdata, &AL::Lua54::Extensions::pushlightuserdata, &AL::Lua54::Extensions::pop<AL::Void*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_Number,     &AL::Lua54::Extensions::getnumber,        &AL::Lua54::Extensions::pushnumber,        &AL::Lua54::Extensions::pop<::lua_Number>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_Integer,    &AL::Lua54::Extensions::getinteger,       &AL::Lua54::Extensions::pushinteger,       &AL::Lua54::Extensions::pop<::lua_Integer>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(char,             &AL::Lua54::Extensions::getchar,          &AL::Lua54::Extensions::pushchar,          &AL::Lua54::Extensions::pop<char>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(const char*,      &AL::Lua54::Extensions::getstring,        &AL::Lua54::Extensions::pushstring,        &AL::Lua54::Extensions::pop<const char*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::String,       &AL::Lua54::Extensions::getString,        &AL::Lua54::Extensions::pushString,        &AL::Lua54::Extensions::pop<AL::String>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(::lua_CFunction,  &AL::Lua54::Extensions::getcfunction,     &AL::Lua54::Extensions::pushcfunction,     &AL::Lua54::Extensions::pop<::lua_CFunction>);

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

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(char*, const char*, const_cast);

template<typename T>
inline T AL::Lua54::Extensions::Type_Functions<T>::Get(::lua_State* lua, size_t index)
{
	if constexpr (IsPointer || IsReference)
	{
		return reinterpret_cast<T>(
			Type_Functions<Void*>::Get(lua, index)
		);
	}
	else if constexpr (IsEnumOrInteger)
	{
		typedef typename Get_Enum_Or_Integer_Base<T>::Type TBase;

		return static_cast<T>(
			Type_Functions<TBase>::Get(lua, index)
		);
	}
}

template<typename T>
inline AL::Void AL::Lua54::Extensions::Type_Functions<T>::Push(::lua_State* lua, T value)
{
	if constexpr (IsPointer || IsConstPointer)
	{
		if (value == nullptr)
		{
			Type_Functions<::std::nullptr_t>::Push(
				lua,
				nullptr
			);
		}
		else if constexpr (IsPointer)
		{
			Type_Functions<Void*>::Push(
				lua,
				reinterpret_cast<Void*>(value)
			);
		}
		else if constexpr (IsConstPointer)
		{
			Type_Functions<Void*>::Push(
				lua,
				const_cast<Void*>(reinterpret_cast<const Void*>(value))
			);
		}
	}
	else if constexpr (IsReference)
	{
		Type_Functions<Void*>::Push(
			lua,
			reinterpret_cast<Void*>(&value)
		);
	}
	else if constexpr (IsConstReference)
	{
		Type_Functions<Void*>::Push(
			lua,
			const_cast<Void*>(reinterpret_cast<const Void*>(&value))
		);
	}
	else if constexpr (IsEnumOrInteger)
	{
		typedef typename Get_Enum_Or_Integer_Base<T>::Type TBase;

		Type_Functions<TBase>::Push(
			lua,
			static_cast<TBase>(value)
		);
	}
}

template<typename T>
inline T AL::Lua54::Extensions::Type_Functions<T>::Pop(::lua_State* lua, size_t count)
{
	if constexpr (IsPointer || IsReference)
	{
		return reinterpret_cast<T>(
			Type_Functions<Void*>::Pop(lua, count)
		);
	}
	else if constexpr (IsEnumOrInteger)
	{
		typedef typename Get_Enum_Or_Integer_Base<T>::Type TBase;

		return static_cast<T>(
			Type_Functions<TBase>::Pop(lua, count)
		);
	}
}
