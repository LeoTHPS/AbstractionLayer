#pragma once
#include "AL/Common.hpp"

#include "LuaException.hpp"
#include "LuaExtensions.hpp"

#include "AL/Collections/Tuple.hpp"

#include "AL/FileSystem/Path.hpp"

namespace AL::Lua543
{
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

		::lua_State* GetHandle() const;

		template<typename T>
		T    Get(size_t index = 1) const
		{
			return Extensions::Type_Functions<T>::Get(
				GetHandle(),
				index
			);
		}

		template<typename T>
		Void Push(T value)
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
				GetHandle(),
				1
			);
		}
		Void Pop(size_t count = 1)
		{
			Extensions::pop(
				GetHandle(),
				count
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
				static int Execute(::lua_State* lua)
				{
					return Execute(
						lua,
						F,
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type{}
					);
				}

			private:
				template<size_t ... INDEXES>
				static constexpr int Execute(::lua_State* lua, T(*lpFunction)(TArgs ...), Index_Sequence<INDEXES ...>)
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
				static constexpr auto Get(::lua_State* lua)
				{
					return static_cast<T_ARG>(
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
				{
					return Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type, INDEX>(
						lua
					);
				}
			};
			template<typename ... TArgs>
			class Detour<Void(*)(TArgs ...)>
			{
			public:
				static int Execute(::lua_State* lua)
				{
					return Execute(
						lua,
						F,
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type{}
					);
				}

			private:
				template<size_t ... INDEXES>
				static constexpr int Execute(::lua_State* lua, Void(*lpFunction)(TArgs ...), Index_Sequence<INDEXES ...>)
				{
					lpFunction(
						GetArg<INDEXES>(lua) ...
					);

					return 0;
				}

				template<typename T_ARG, size_t INDEX>
				static constexpr auto Get(::lua_State* lua)
				{
					return static_cast<T_ARG>(
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
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
				static int Execute(::lua_State* lua)
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
				static constexpr int Execute(::lua_State* lua, Collections::Tuple<T ...>(*lpFunction)(TArgs ...), Index_Sequence<I_T ...>, Index_Sequence<I_TArgs ...>)
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
				static constexpr auto Get(::lua_State* lua)
				{
					return static_cast<T_ARG>(
						Extensions::Type_Functions<T_ARG>::Get(lua, INDEX + 1)
					);
				}

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
				{
					return Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type, INDEX>(
						lua
					);
				}

				static constexpr Void PushReturnValues(::lua_State* lua)
				{
				}
				template<typename T_RETURN, typename ... T_RETURN_VALUES>
				static constexpr Void PushReturnValues(::lua_State* lua, T_RETURN value, T_RETURN_VALUES ... values)
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
			static int Execute(::lua_State* lua)
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
				static constexpr T_RETURN Execute(::lua_State* lua, const String& name, T_ARGS ... args)
				{
					Extensions::setGlobal(
						lua,
						name
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						1
					);

					return Extensions::Type_Functions<T_RETURN>::Pop(
						lua,
						1
					);
				}

			private:
				static constexpr Void PushArgs(::lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr Void PushArgs(::lua_State* lua, _T_ARG arg, _T_ARGS ... args)
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
			class Detour<Void(T_ARGS ...)>
			{
			public:
				static constexpr Void Execute(::lua_State* lua, const String& name, T_ARGS ... args)
				{
					Extensions::setGlobal(
						lua,
						name
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						0
					);
				}

			private:
				static constexpr Void PushArgs(::lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr Void PushArgs(::lua_State* lua, _T_ARG arg, _T_ARGS ... args)
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
				static constexpr Collections::Tuple<T_RETURN ...> Execute(::lua_State* lua, const String& name, T_ARGS ... args)
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
				static constexpr Collections::Tuple<T_RETURN ...> Execute(::lua_State* lua, const String& name, T_ARGS ... args, Index_Sequence<INDEXES ...>)
				{
					Extensions::setGlobal(
						lua,
						name
					);

					PushArgs(
						lua,
						Forward<T_ARGS>(args) ...
					);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						sizeof ...(T_RETURN)
					);

					return Collections::Tuple<T_RETURN ...>(
						PopReturnValue<INDEXES>(lua) ...
					);
				}

				static constexpr Void PushArgs(::lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr Void PushArgs(::lua_State* lua, _T_ARG arg, _T_ARGS ... args)
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
				static constexpr auto PopReturnValue(::lua_State* lua)
				{
					return Extensions::Type_Functions<typename Get_Type_Sequence<INDEX, T_RETURN ...>::Type>::Pop(
						lua,
						1
					);
				}
			};

			LuaFunction() = delete;

		public:
			static constexpr T Execute(::lua_State* lua, const String& name, TArgs ... args)
			{
				return Detour<T(TArgs ...)>::Execute(
					lua,
					name,
					Forward<TArgs>(args) ...
				);
			}
		};

		Bool isCreated = False;

		::lua_State* lua;

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
			state.isCreated = False;
		}

		virtual ~State()
		{
			Destroy();
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		::lua_State* GetHandle() const
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

		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"State already created"
			);

			OnCreate();

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				OnDestroy();

				isCreated = False;
			}
		}

		template<typename T>
		auto GetGlobal(const String& name) const
		{
			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return Extensions::Type_Functions<T>::Pop(
				GetHandle(),
				1
			);
		}

		template<typename T>
		Void SetGlobal(const String& name, T value)
		{
			Extensions::Type_Functions<T>::Push(
				GetHandle(),
				Forward<T>(value)
			);

			Extensions::setGlobal(
				GetHandle(),
				name
			);
		}

		template<auto F>
		Void SetGlobalFunction(const String& name)
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

		// @throw AL::Exception
		Void Run(const String& lua)
		{
			AL_ASSERT(
				IsCreated(),
				"State not created"
			);
			
			Extensions::doString(
				GetHandle(),
				lua
			);
		}

		// @throw AL::Exception
		Void RunFile(const FileSystem::Path& path)
		{
			AL_ASSERT(
				IsCreated(),
				"State not created"
			);

			Extensions::doFile(
				GetHandle(),
				path.GetString()
			);
		}

		State& operator = (State&& state)
		{
			Destroy();

			isCreated = state.isCreated;
			state.isCreated = False;

			lua = state.lua;

			stack = Move(
				state.stack
			);

			return state;
		}

	protected:
		// @throw AL::Exception
		virtual Void OnCreate()
		{
			lua = Extensions::newstate();
		}

		virtual Void OnDestroy()
		{
			Extensions::close(
				lua
			);

			lua = nullptr;
		}
	};
}

inline ::lua_State* AL::Lua543::Stack::GetHandle() const
{
	return lpState->GetHandle();
}
