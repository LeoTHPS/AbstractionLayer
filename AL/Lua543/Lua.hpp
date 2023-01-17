#pragma once
#include "AL/Common.hpp"

#include "LuaException.hpp"
#include "LuaExtensions.hpp"

#include "AL/Collections/Tuple.hpp"

#if !defined(AL_PLATFORM_PICO)
	#include "AL/FileSystem/Path.hpp"
#endif

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
		struct Function_Utils
		{
			template<typename T>
			struct Is_Const_Pointer
			{
				static constexpr Bool Value = False;
			};
			template<typename T>
			struct Is_Const_Pointer<const T*>
			{
				static constexpr Bool Value = True;
			};

			template<typename T>
			struct Is_Const_Reference
			{
				static constexpr Bool Value = False;
			};
			template<typename T>
			struct Is_Const_Reference<const T&>
			{
				static constexpr Bool Value = True;
			};

			template<typename T>
			struct Remove_Const_Reference
			{
				typedef T Type;
			};
			template<typename T>
			struct Remove_Const_Reference<const T&>
			{
				typedef T Type;
			};

			template<typename T_ARG, size_t I>
			static constexpr auto Detour_Get(::lua_State* lua)
			{
				if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
				{
					return Extensions::Type_Functions<T_ARG>::Get(
						lua,
						I + 1
					);
				}
				else if constexpr (Is_Const_Pointer<T_ARG>::Value)
				{
					return reinterpret_cast<T_ARG>(
						Extensions::Type_Functions<Void*>::Get(
							lua,
							I + 1
						)
					);
				}
				else if constexpr (Is_Pointer<T_ARG>::Value)
				{
					return reinterpret_cast<T_ARG>(
						Extensions::Type_Functions<Void*>::Get(
							lua,
							I + 1
						)
					);
				}
				else if constexpr (Is_Const_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Const_Reference<T_ARG>::Type T_ARG_NO_CONST_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_CONST_REF>::IsDefined)
					{
						return Extensions::Type_Functions<T_ARG_NO_CONST_REF>::Get(
							lua,
							I + 1
						);
					}
				}
				else if constexpr (Is_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Reference<T_ARG>::Type T_ARG_NO_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_REF>::IsDefined)
					{
						return Extensions::Type_Functions<T_ARG_NO_REF>::Get(
							lua,
							I + 1
						);
					}
				}
			}

			template<typename T_ARG>
			static constexpr auto Detour_Pop(::lua_State* lua)
			{
				if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
				{
					return Extensions::Type_Functions<T_ARG>::Pop(
						lua,
						1
					);
				}
				else if constexpr (Is_Const_Pointer<T_ARG>::Value)
				{
					return reinterpret_cast<T_ARG>(
						Extensions::Type_Functions<Void*>::Pop(
							lua,
							1
						)
					);
				}
				else if constexpr (Is_Pointer<T_ARG>::Value)
				{
					return reinterpret_cast<T_ARG>(
						Extensions::Type_Functions<Void*>::Pop(
							lua,
							1
						)
					);
				}
				else if constexpr (Is_Const_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Const_Reference<T_ARG>::Type T_ARG_NO_CONST_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_CONST_REF>::IsDefined)
					{
						return Extensions::Type_Functions<T_ARG_NO_CONST_REF>::Pop(
							lua,
							1
						);
					}
				}
				else if constexpr (Is_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Reference<T_ARG>::Type T_ARG_NO_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_REF>::IsDefined)
					{
						return Extensions::Type_Functions<T_ARG_NO_REF>::Pop(
							lua,
							1
						);
					}
				}
			}

			template<typename T_ARG>
			static constexpr Void Detour_Push(::lua_State* lua, T_ARG value)
			{
				if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
				{
					Extensions::Type_Functions<T_ARG>::Push(
						lua,
						Forward<T_ARG>(value)
					);
				}
				else if constexpr (Is_Const_Pointer<T_ARG>::Value)
				{
					typedef typename Remove_Const<T_ARG>::Type T_NO_CONST;

					Extensions::Type_Functions<Void*>::Push(
						lua,
						const_cast<T_NO_CONST>(value)
					);
				}
				else if constexpr (Is_Pointer<T_ARG>::Value)
				{
					Extensions::Type_Functions<Void*>::Push(
						lua,
						value
					);
				}
				else if constexpr (Is_Const_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Const_Reference<T_ARG>::Type T_ARG_NO_CONST_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_CONST_REF>::IsDefined)
					{
						Extensions::Type_Functions<T_ARG_NO_CONST_REF>::Push(
							lua,
							value
						);
					}
				}
				else if constexpr (Is_Reference<T_ARG>::Value)
				{
					typedef typename Remove_Reference<T_ARG>::Type T_ARG_NO_REF;

					if constexpr (Extensions::Type_Functions<T_ARG_NO_REF>::IsDefined)
					{
						Extensions::Type_Functions<T_ARG_NO_REF>::Push(
							lua,
							value
						);
					}
				}
			}
		};

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

					Function_Utils::Detour_Push<T>(
						lua,
						Forward<T>(result)
					);

					return 1;
				}

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
				{
					typedef typename Get_Type_Sequence<INDEX, TArgs ...>::Type T_ARG;

					return Function_Utils::Detour_Get<T_ARG, INDEX>(
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

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
				{
					typedef typename Get_Type_Sequence<INDEX, TArgs ...>::Type T_ARG;

					return Function_Utils::Detour_Get<T_ARG, INDEX>(
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

				template<size_t INDEX>
				static constexpr auto GetArg(::lua_State* lua)
				{
					typedef typename Get_Type_Sequence<INDEX, TArgs ...>::Type T_ARG;

					return Function_Utils::Detour_Get<T_ARG, INDEX>(
						lua
					);
				}

				static constexpr Void PushReturnValues(::lua_State* lua)
				{
				}
				template<typename T_RETURN, typename ... T_RETURN_VALUES>
				static constexpr Void PushReturnValues(::lua_State* lua, T_RETURN value, T_RETURN_VALUES ... values)
				{
					Function_Utils::Detour_Push<T_RETURN>(
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
					Extensions::getGlobal(
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

					return Function_Utils::Detour_Pop<T_RETURN>(
						lua
					);
				}

			private:
				static constexpr Void PushArgs(::lua_State* lua)
				{
				}
				template<typename _T_ARG, typename ... _T_ARGS>
				static constexpr Void PushArgs(::lua_State* lua, _T_ARG arg, _T_ARGS ... args)
				{
					Function_Utils::Detour_Push<_T_ARG>(
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
					Extensions::getGlobal(
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
					Function_Utils::Detour_Push<_T_ARG>(
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
					Extensions::getGlobal(
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
					Function_Utils::Detour_Push<_T_ARG>(
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
					typedef typename Get_Type_Sequence<INDEX, T_RETURN ...>::Type _T_RETURN;

					return Function_Utils::Detour_Pop<_T_RETURN>(
						lua
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

#if !defined(AL_PLATFORM_PICO)
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
#endif

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

		Bool operator == (const State& state) const
		{
			if (lua != state.lua)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const State& state) const
		{
			if (operator==(state))
			{

				return False;
			}

			return True;
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
