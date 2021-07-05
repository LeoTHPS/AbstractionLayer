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
	struct AL::Lua::Extensions::Type_Functions<type> \
	{ \
		static constexpr auto Get  = get; \
		static constexpr auto Push = push; \
		static constexpr auto Pop  = pop; \
	}

#define AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(type, alias) \
	template<> \
	struct AL::Lua::Extensions::Type_Functions<type> \
		: public AL::Lua::Extensions::Type_Functions<alias> \
	{ \
	}

namespace AL::Lua
{
	namespace Extensions
	{
		template<typename T>
		struct Type_Functions;

		static nullptr_t     getnil(lua_State* lua, size_t index)
		{
			return nullptr;
		}
		static const char*   getstring(lua_State* lua, size_t index)
		{
			auto value = lua_tostring(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static String        getString(lua_State* lua, size_t index)
		{
			auto lpString = getstring(
				lua,
				index
			);

			return String(
				lpString ? lpString : ""
			);
		}
		static bool          getboolean(lua_State* lua, size_t index)
		{
			auto value = lua_toboolean(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value == 1;
		}
		static void*         getlightuserdata(lua_State* lua, size_t index)
		{
			auto value = lua_touserdata(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_Number    getnumber(lua_State* lua, size_t index)
		{
			auto value = lua_tonumber(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_Integer   getinteger(lua_State* lua, size_t index)
		{
			auto value = lua_tointeger(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}
		static lua_CFunction getcfunction(lua_State* lua, size_t index)
		{
			auto value = lua_tocfunction(
				lua,
				static_cast<int>(index & 0x7FFFFFFF)
			);

			return value;
		}

		static void pushnil(lua_State* lua, nullptr_t)
		{
			lua_pushnil(
				lua
			);
		}
		static void pushstring(lua_State* lua, const char* value)
		{
			lua_pushstring(
				lua,
				value
			);
		}
		static void pushString(lua_State* lua, const String& value)
		{
			pushstring(
				lua,
				value.GetCString()
			);
		}
		static void pushboolean(lua_State* lua, bool value)
		{
			lua_pushboolean(
				lua,
				value ? 1 : 0
			);
		}
		static void pushlightuserdata(lua_State* lua, void* value)
		{
			lua_pushlightuserdata(
				lua,
				value
			);
		}
		static void pushnumber(lua_State* lua, lua_Number value)
		{
			lua_pushnumber(
				lua,
				value
			);
		}
		static void pushinteger(lua_State* lua, lua_Integer value)
		{
			lua_pushinteger(
				lua,
				value
			);
		}
		static void pushcfunction(lua_State* lua, lua_CFunction value)
		{
			lua_pushcfunction(
				lua,
				value
			);
		}

		template<typename T>
		static T    pop(lua_State* lua, size_t count = 1)
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
	}

	class State;

	class Stack
	{
		friend State;

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
			return Extensions::Type_Functions<T>::Get(
				GetHandle(),
				index
			);
		}

		template<typename T>
		void Push(T value)
		{
			Extensions::Type_Functions<T>::Push(
				GetHandle(),
				Forward<T>(value)
			);
		}

		template<typename T>
		T    Pop()
		{
			return Extensions::Type_Functions<T>::Pop(
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
	};

	class State
	{
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

					Extensions::Type_Functions<T>::Push(
						lua,
						result
					);

					return 1;
				}

				template<typename T_ARG, size_t INDEX>
				static constexpr auto Get(lua_State* lua)
				{
					return static_cast<T_ARG>(
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
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
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
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
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
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
					Extensions::Type_Functions<T_RETURN>::Push(
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

					return Extensions::Type_Functions<T_RETURN>::Pop(
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
					Extensions::Type_Functions<_T_ARG>::Push(
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
					Extensions::Type_Functions<_T_ARG>::Push(
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
					Extensions::Type_Functions<_T_ARG>::Push(
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
					return Extensions::Type_Functions<typename Get_Type_Sequence<INDEX, T_RETURN ...>::Type>::Pop(
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

		lua_State* GetHandle() const
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
		auto GetGlobal(const String& name) const
		{
			lua_getglobal(
				GetHandle(),
				name.GetCString()
			);

			return Extensions::Type_Functions<T>::Pop(
				GetHandle()
			);
		}

		template<typename T>
		void SetGlobal(const String& name, T value)
		{
			Extensions::Type_Functions<T>::Push(
				GetHandle(),
				Forward<T>(value)
			);

			lua_setglobal(
				GetHandle(),
				name.GetCString()
			);
		}

		template<auto F>
		void SetGlobalFunction(const String& name)
		{
			SetGlobal(
				name,
				&Function<F>::Execute
			);
		}

		template<typename F, typename ... TArgs>
		auto CallGlobalFunction(const String& name, TArgs ... args)
		{
			return LuaFunction<F>::Execute(
				GetHandle(),
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
				GetHandle()
			);

			lua = nullptr;
		}
	};
}

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(nullptr_t,     &AL::Lua::Extensions::getnil,           &AL::Lua::Extensions::pushnil,           &AL::Lua::Extensions::pop<nullptr_t>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(bool,          &AL::Lua::Extensions::getboolean,       &AL::Lua::Extensions::pushboolean,       &AL::Lua::Extensions::pop<bool>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(void*,         &AL::Lua::Extensions::getlightuserdata, &AL::Lua::Extensions::pushlightuserdata, &AL::Lua::Extensions::pop<void*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_Number,    &AL::Lua::Extensions::getnumber,        &AL::Lua::Extensions::pushnumber,        &AL::Lua::Extensions::pop<lua_Number>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_Integer,   &AL::Lua::Extensions::getinteger,       &AL::Lua::Extensions::pushinteger,       &AL::Lua::Extensions::pop<lua_Integer>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(const char*,   &AL::Lua::Extensions::getstring,        &AL::Lua::Extensions::pushstring,        &AL::Lua::Extensions::pop<const char*>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(AL::String,    &AL::Lua::Extensions::getString,        &AL::Lua::Extensions::pushString,        &AL::Lua::Extensions::pop<AL::String>);
AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS(lua_CFunction, &AL::Lua::Extensions::getcfunction,     &AL::Lua::Extensions::pushcfunction,     &AL::Lua::Extensions::pop<lua_CFunction>);

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

AL_LUA_DEFINE_TYPE_STACK_FUNCTIONS_ALIAS(const AL::String&, AL::String);

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
