#pragma once
#include <AL/Common.hpp>

#include <AL/Collections/Tuple.hpp>

#include <AL/FileSystem/Path.hpp>

#if defined(AL_PLATFORM_WINDOWS)
	#define LUA_BUILD_AS_DLL
#endif

#include "Lua543/lua.hpp"

#define AL_LUA_GET_GLOBAL(state, type, name)                     state.GetGlobal<type>(name)

#define AL_LUA_SET_GLOBAL(state, value)                          AL_LUA_SET_GLOBAL_EX(state, #value, value)
#define AL_LUA_SET_GLOBAL_EX(state, name, value)                 state.SetGlobal(name, value)

#define AL_LUA_SET_GLOBAL_FUNCTION(state, value)                 AL_LUA_SET_GLOBAL_FUNCTION_EX(state, #value, value)
#define AL_LUA_SET_GLOBAL_FUNCTION_EX(state, name, value)        state.SetGlobalFunction<&value>(name)

#define AL_LUA_CALL_GLOBAL_FUNCTION(state, signature, name, ...) state.CallGlobalFunction<signature>(name, __VA_ARGS__)

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(type, get, push, pop) \
	template<> \
	struct AL::Lua::Stack::Type_Functions<type> \
	{ \
		static constexpr auto Get  = get; \
		static constexpr auto Push = push; \
		static constexpr auto Pop  = pop; \
	}

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(type, alias) \
	template<> \
	struct AL::Lua::Stack::Type_Functions<type> \
		: public AL::Lua::Stack::Type_Functions<alias> \
	{ \
	}

namespace AL::Lua
{
	class State;

	class Stack
	{
		friend State;

		template<typename T>
		struct Type_Functions;
		
		template<auto F>
		class Function
		{
			template<typename T>
			class Detour;
			template<typename T, typename ... TArgs>
			class Detour<T(*)(TArgs ...)>
			{
			public:
				static int Execute(lua_State* lua)
				{
					return Execute(
						lua,
						F,
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type{}
					);
				}

			private:
				template<size_t ... INDEXES>
				static constexpr int Execute(lua_State* lua, T(*lpFunction)(TArgs ...), Index_Sequence<INDEXES ...>)
				{
					auto result = lpFunction(
						GetArg<INDEXES>(lua) ...
					);

					Type_Functions<T>::Push(
						lua,
						result
					);

					return 1;
				}

				template<typename T_ARG, size_t INDEX>
				static constexpr auto Get(lua_State* lua)
				{
					return static_cast<T_ARG>(
						Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(lua_State* lua)
				{
					return Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type, INDEX>(
						lua
					);
				}
			};
			template<typename ... TArgs>
			class Detour<void(*)(TArgs ...)>
			{
			public:
				static int Execute(lua_State* lua)
				{
					return Execute(
						lua,
						F,
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type{}
					);
				}

			private:
				template<size_t ... INDEXES>
				static constexpr int Execute(lua_State* lua, void(*lpFunction)(TArgs ...), Index_Sequence<INDEXES ...>)
				{
					lpFunction(
						GetArg<INDEXES>(lua) ...
					);

					return 0;
				}

				template<typename T_ARG, size_t INDEX>
				static constexpr auto Get(lua_State* lua)
				{
					return static_cast<T_ARG>(
						Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(lua_State* lua)
				{
					return Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type, INDEX>(
						lua
					);
				}
			};
			template<typename ... T, typename ... TArgs>
			class Detour<Collections::Tuple<T ...>(*)(TArgs ...)>
			{
			public:
				static int Execute(lua_State* lua)
				{
					return Execute(
						lua,
						F,
						typename Make_Index_Sequence<sizeof ...(T)>::Type{},
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type{}
					);
				}

			private:
				template<size_t ... I_T, size_t ... I_TArgs>
				static constexpr int Execute(lua_State* lua, Collections::Tuple<T ...>(*lpFunction)(TArgs ...), Index_Sequence<I_T ...>, Index_Sequence<I_TArgs ...>)
				{
					auto result = lpFunction(
						GetArg<I_TArgs>(lua) ...
					);

					PushReturnValues(
						lua,
						result.template Get<I_T>() ...
					);

					return sizeof ...(T);
				}

				template<typename T_ARG, size_t INDEX>
				static constexpr auto Get(lua_State* lua)
				{
					return static_cast<T_ARG>(
						Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(lua_State* lua)
				{
					return Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type, INDEX>(
						lua
					);
				}

				static constexpr void PushReturnValues(lua_State* lua)
				{
				}
				template<typename T_RETURN, typename ... T_RETURN_VALUES>
				static constexpr void PushReturnValues(lua_State* lua, T_RETURN value, T_RETURN_VALUES ... values)
				{
					Type_Functions<T_RETURN>::Push(
						lua,
						Forward<T_RETURN>(value)
					);

					PushReturnValues(
						lua,
						Forward<T_RETURN_VALUES>(values) ...
					);
				}
			};

			Function() = delete;

		public:
			static int Execute(lua_State* lua)
			{
				return Detour<decltype(F)>::Execute(
					lua
				);
			}
		};

		template<typename F>
		class LuaFunction;
		template<typename T, typename ... TArgs>
		class LuaFunction<T(TArgs ...)>
		{
			template<typename F>
			class Detour;
			template<typename T_RETURN, typename ... T_ARGS>
			class Detour<T_RETURN(T_ARGS ...)>
			{
			public:
				static constexpr T_RETURN Execute(lua_State* lua, const String& name, T_ARGS ... args)
				{
					lua_getglobal(
						lua,
						name.GetCString()
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					lua_call(
						lua,
						sizeof ...(T_ARGS),
						1
					);

					return Type_Functions<T_RETURN>::Pop(
						lua
					);
				}

			private:
				static constexpr void PushArgs(lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr void PushArgs(lua_State* lua, _T_ARG arg, _T_ARGS ... args)
				{
					Type_Functions<_T_ARG>::Push(
						lua,
						Forward<_T_ARG>(arg)
					);

					PushArgs(
						lua,
						Forward<_T_ARGS>(args) ...
					);
				}
			};
			template<typename ... T_ARGS>
			class Detour<void(T_ARGS ...)>
			{
			public:
				static constexpr void Execute(lua_State* lua, const String& name, T_ARGS ... args)
				{
					lua_getglobal(
						lua,
						name.GetCString()
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					lua_call(
						lua,
						sizeof ...(T_ARGS),
						0
					);
				}

			private:
				static constexpr void PushArgs(lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr void PushArgs(lua_State* lua, _T_ARG arg, _T_ARGS ... args)
				{
					Type_Functions<_T_ARG>::Push(
						lua,
						Forward<_T_ARG>(arg)
					);

					PushArgs(
						lua,
						Forward<_T_ARGS>(args) ...
					);
				}
			};
			template<typename ... T_RETURN, typename ... T_ARGS>
			class Detour<Collections::Tuple<T_RETURN ...>(T_ARGS ...)>
			{
			public:
				static constexpr Collections::Tuple<T_RETURN ...> Execute(lua_State* lua, const String& name, T_ARGS ... args)
				{
					return Execute(
						lua,
						name,
						Forward<T_ARGS>(args) ...,
						typename Make_Index_Sequence<sizeof ...(T_RETURN)>::Type{}
					);
				}

			private:
				template<size_t ... INDEXES>
				static constexpr Collections::Tuple<T_RETURN ...> Execute(lua_State* lua, const String& name, T_ARGS ... args, Index_Sequence<INDEXES ...>)
				{
					lua_getglobal(
						lua,
						name.GetCString()
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					lua_call(
						lua,
						sizeof ...(T_ARGS),
						sizeof ...(T_RETURN)
					);

					return Collections::Tuple<T_RETURN ...>(
						PopReturnValue<INDEXES>(lua) ...
					);
				}

				static constexpr void PushArgs(lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr void PushArgs(lua_State* lua, _T_ARG arg, _T_ARGS ... args)
				{
					Type_Functions<_T_ARG>::Push(
						lua,
						Forward<_T_ARG>(arg)
					);

					PushArgs(
						lua,
						Forward<_T_ARGS>(args) ...
					);
				}

				template<size_t INDEX>
				static constexpr auto PopReturnValue(lua_State* lua)
				{
					return Type_Functions<typename Get_Type_Sequence<INDEX, T_RETURN ...>::Type>::Pop(
						lua
					);
				}
			};

			LuaFunction() = delete;

		public:
			static constexpr T Execute(lua_State* lua, const String& name, TArgs ... args)
			{
				return Detour<T(TArgs ...)>::Execute(
					lua,
					name,
					Forward<TArgs>(args) ...
				);
			}
		};

		State* lpState;

		Stack(const Stack&) = delete;

		explicit Stack(State& state)
			: lpState(
				&state
			)
		{
		}

	public:
		Stack(Stack&& stack)
			: lpState(
				stack.lpState
			)
		{
			stack.lpState = nullptr;
		}

		virtual ~Stack()
		{
		}

		lua_State* GetHandle() const;

		template<typename T>
		T    Get(size_t index = 1) const
		{
			return Type_Functions<T>::Get(
				GetHandle(),
				index
			);
		}

		template<typename T>
		void Push(T value)
		{
			Type_Functions<T>::Push(
				GetHandle(),
				Forward<T>(value)
			);
		}

		template<typename T>
		T    Pop()
		{
			return Type_Functions<T>::Pop(
				GetHandle()
			);
		}
		void Pop(size_t count = 1)
		{
			lua_pop(
				GetHandle(),
				static_cast<int>(count & 0x7FFFFFFF)
			);
		}

		auto& operator = (Stack&& stack)
		{
			lpState = stack.lpState;
			stack.lpState = nullptr;

			return *this;
		}

		static nullptr_t     _lua_getnil(lua_State* lua, size_t index)
		{
			return nullptr;
		}
		static const char*   _lua_getstring(lua_State* lua, size_t index)
		{
			auto value = lua_tostring(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static String        _lua_getString(lua_State* lua, size_t index)
		{
			auto lpString = _lua_getstring(
				lua,
				index
			);

			return String(
				lpString
			);
		}
		static bool          _lua_getboolean(lua_State* lua, size_t index)
		{
			auto value = lua_toboolean(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value == 1;
		}
		static void*         _lua_getlightuserdata(lua_State* lua, size_t index)
		{
			auto value = lua_touserdata(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_Number    _lua_getnumber(lua_State* lua, size_t index)
		{
			auto value = lua_tonumber(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_Integer   _lua_getinteger(lua_State* lua, size_t index)
		{
			auto value = lua_tointeger(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_CFunction _lua_getcfunction(lua_State* lua, size_t index)
		{
			auto value = lua_tocfunction(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}

		static void _lua_pushnil(lua_State* lua, nullptr_t)
		{
			lua_pushnil(
				lua
			);
		}
		static void _lua_pushstring(lua_State* lua, const char* value)
		{
			lua_pushstring(
				lua,
				value
			);
		}
		static void _lua_pushString(lua_State* lua, const String& value)
		{
			_lua_pushstring(
				lua,
				value.GetCString()
			);
		}
		static void _lua_pushboolean(lua_State* lua, bool value)
		{
			lua_pushboolean(
				lua,
				value ? 1 : 0
			);
		}
		static void _lua_pushlightuserdata(lua_State* lua, void* value)
		{
			lua_pushlightuserdata(
				lua,
				value
			);
		}
		static void _lua_pushnumber(lua_State* lua, lua_Number value)
		{
			lua_pushnumber(
				lua,
				value
			);
		}
		static void _lua_pushinteger(lua_State* lua, lua_Integer value)
		{
			lua_pushinteger(
				lua,
				value
			);
		}
		static void _lua_pushcfunction(lua_State* lua, lua_CFunction value)
		{
			lua_pushcfunction(
				lua,
				value
			);
		}

		template<typename T>
		static T    _lua_pop(lua_State* lua, size_t count = 1)
		{
			auto value = Type_Functions<T>::Get(
				lua,
				1
			);

			lua_pop(
				lua,
				static_cast<int>(count & 0x7FFFFFFF)
			);

			return value;
		}
	};

	class State
	{
		bool isCreated = false;

		lua_State* lua;

		Stack stack;

		State(const State&) = delete;

	public:
		State()
			: stack(
				*this
			)
		{
		}

		State(State&& state)
			: isCreated(
				state.isCreated
			),
			lua(
				state.lua
			),
			stack(
				Move(state.stack)
			)
		{
			state.isCreated = false;
		}

		virtual ~State()
		{
			Destroy();
		}

		bool IsCreated() const
		{
			return isCreated;
		}

		auto GetHandle() const
		{
			return lua;
		}

		Stack& GetStack()
		{
			return stack;
		}
		const Stack& GetStack() const
		{
			return stack;
		}

		// @throw AL::Exceptions::Exception
		void Create()
		{
			AL_ASSERT(!IsCreated(), "State already created");

			OnCreate();

			isCreated = true;
		}

		void Destroy()
		{
			if (IsCreated())
			{
				OnDestroy();

				isCreated = false;
			}
		}

		template<typename T>
		auto GetGlobal(const String& name)
		{
			lua_getglobal(
				lua,
				name.GetCString()
			);

			return GetStack().Pop<T>();
		}

		template<typename T>
		void SetGlobal(const String& name, T value)
		{
			GetStack().Push<T>(
				Forward<T>(value)
			);

			lua_setglobal(
				lua,
				name.GetCString()
			);
		}

		template<auto F>
		void SetGlobalFunction(const String& name)
		{
			SetGlobal(
				name,
				&Stack::Function<F>::Execute
			);
		}

		template<typename F, typename ... TArgs>
		auto CallGlobalFunction(const String& name, TArgs ... args)
		{
			return Stack::LuaFunction<F>::Execute(
				lua,
				name,
				Forward<TArgs>(args) ...
			);
		}

		// @throw AL::Exceptions::Exception
		void Run(const String& lua);

		// @throw AL::Exceptions::Exception
		void RunFile(const AL::FileSystem::Path& path);

		auto& operator = (State&& state)
		{
			Destroy();

			isCreated = state.isCreated;
			state.isCreated = false;

			lua = state.lua;

			stack = Move(
				state.stack
			);

			return state;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnCreate();

		virtual void OnDestroy()
		{
			lua_close(
				lua
			);
		}
	};
}

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(nullptr_t,     &AL::Lua::Stack::_lua_getnil,           &AL::Lua::Stack::_lua_pushnil,           &AL::Lua::Stack::_lua_pop<nullptr_t>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(bool,          &AL::Lua::Stack::_lua_getboolean,       &AL::Lua::Stack::_lua_pushboolean,       &AL::Lua::Stack::_lua_pop<bool>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(void*,         &AL::Lua::Stack::_lua_getlightuserdata, &AL::Lua::Stack::_lua_pushlightuserdata, &AL::Lua::Stack::_lua_pop<void*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_Number,    &AL::Lua::Stack::_lua_getnumber,        &AL::Lua::Stack::_lua_pushnumber,        &AL::Lua::Stack::_lua_pop<lua_Number>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_Integer,   &AL::Lua::Stack::_lua_getinteger,       &AL::Lua::Stack::_lua_pushinteger,       &AL::Lua::Stack::_lua_pop<lua_Integer>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(const char*,   &AL::Lua::Stack::_lua_getstring,        &AL::Lua::Stack::_lua_pushstring,        &AL::Lua::Stack::_lua_pop<const char*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::String,    &AL::Lua::Stack::_lua_getString,        &AL::Lua::Stack::_lua_pushString,        &AL::Lua::Stack::_lua_pop<AL::String>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_CFunction, &AL::Lua::Stack::_lua_getcfunction,     &AL::Lua::Stack::_lua_pushcfunction,     &AL::Lua::Stack::_lua_pop<lua_CFunction>);

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(const AL::String&, AL::String);

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int8,  lua_Integer);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint8, lua_Integer);

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int16,  lua_Integer);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint16, lua_Integer);

#if !LUA_32BITS
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int32, lua_Integer);
#else
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::int64, lua_Integer);
#endif

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint32, lua_Integer);

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(AL::uint64, lua_Integer);

#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(double, lua_Number);
#elif LUA_FLOAT_TYPE == LUA_FLOAT_DOUBLE
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(float,  lua_Number);
#elif LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(float,  lua_Number);
	AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(double, lua_Number);
#endif

#include "AL/Exceptions/LuaException.hpp"

inline lua_State* AL::Lua::Stack::GetHandle() const
{
	return lpState->GetHandle();
}

// @throw AL::Exceptions::Exception
inline void AL::Lua::State::Run(const String& lua)
{
	AL_ASSERT(IsCreated(), "State not created");

	if (luaL_dostring(GetHandle(), lua.GetCString()) != LUA_OK)
	{

		throw Exceptions::LuaException(
			*this,
			"luaL_dostring"
		);
	}
}

// @throw AL::Exceptions::Exception
inline void AL::Lua::State::RunFile(const AL::FileSystem::Path& path)
{
	AL_ASSERT(IsCreated(), "State not created");

	if (luaL_dofile(GetHandle(), path.GetString().GetCString()) != LUA_OK)
	{

		throw Exceptions::LuaException(
			*this,
			"luaL_dofile"
		);
	}
}

// @throw AL::Exceptions::Exception
inline void AL::Lua::State::OnCreate()
{
	if ((lua = luaL_newstate()) == nullptr)
	{

		throw Exceptions::LuaException(
			*this,
			"luaL_newstate"
		);
	}
}
