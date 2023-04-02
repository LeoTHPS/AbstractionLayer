#pragma once
#include "AL/Common.hpp"

#include "LuaException.hpp"
#include "LuaExtensions.hpp"

#include "AL/Collections/Tuple.hpp"

#if !defined(AL_PLATFORM_PICO)
	#include "AL/FileSystem/Path.hpp"
#endif

#if defined(AL_PLATFORM_WINDOWS)
	#undef LoadLibrary
#endif

namespace AL::Lua54
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

	class Function
	{
	public:
		template<typename F>
		class LuaCallback;

		template<typename T>
		struct Is_LuaCallback
		{
			static constexpr Bool Value = False;
		};
		template<typename F>
		struct Is_LuaCallback<LuaCallback<F>>
		{
			static constexpr Bool Value = True;
		};

		template<auto F>
		class C
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
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type {}
					);
				}
				template<size_t ... INDEXES>
				static int Execute(::lua_State* lua, Index_Sequence<INDEXES ...>)
				{
					auto result = F(
						Get<INDEXES>(lua) ...
					);

					Push<T>(
						lua,
						Forward<T>(result)
					);

					return 1;
				}

			private:
				template<size_t INDEX>
				static constexpr auto Get(::lua_State* lua)
				{
					return Function::Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type>(
						lua,
						1 + INDEX
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
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type {}
					);
				}
				template<size_t ... INDEXES>
				static int Execute(::lua_State* lua, Index_Sequence<INDEXES ...>)
				{
					F(
						Get<INDEXES>(lua) ...
					);

					return 0;
				}

			private:
				template<size_t INDEX>
				static constexpr auto Get(::lua_State* lua)
				{
					return Function::Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type>(
						lua,
						1 + INDEX
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
						typename Make_Index_Sequence<sizeof ...(T)>::Type {},
						typename Make_Index_Sequence<sizeof ...(TArgs)>::Type {}
					);
				}
				template<size_t ... I_RETURN, size_t ... I_ARGS>
				static int Execute(::lua_State* lua, Index_Sequence<I_RETURN ...>, Index_Sequence<I_ARGS ...>)
				{
					auto result = F(
						Get<I_ARGS>(lua) ...
					);

					(Push<I_RETURN>(lua, result), ...);

					return sizeof ...(T);
				}

			private:
				template<size_t INDEX>
				static constexpr auto Get(::lua_State* lua)
				{
					return Function::Get<typename Get_Type_Sequence<INDEX, TArgs ...>::Type>(
						lua,
						1 + INDEX
					);
				}

				template<size_t INDEX>
				static constexpr Void Push(::lua_State* lua, const Collections::Tuple<T ...>& tuple)
				{
					Function::Push<typename Get_Type_Sequence<INDEX, T ...>::Type>(
						lua,
						tuple.template Get<INDEX>()
					);
				}
			};

			C() = delete;

		public:
			static int Execute(::lua_State* lua)
			{
				return Detour<decltype(F)>::Execute(
					lua
				);
			}
		};

		template<typename F>
		class Lua;
		template<typename T, typename ... TArgs>
		class Lua<T(TArgs ...)>
		{
			template<typename F>
			class Detour;
			template<typename T_RETURN, typename ... T_ARGS>
			class Detour<T_RETURN(T_ARGS ...)>
			{
			public:
				static constexpr T_RETURN Execute(::lua_State* lua, T_ARGS ... args)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						1
					);

					return Pop<T_RETURN>(
						lua
					);
				}
			};
			template<typename ... T_ARGS>
			class Detour<Void(T_ARGS ...)>
			{
			public:
				static constexpr Void Execute(::lua_State* lua, T_ARGS ... args)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						0
					);
				}
			};
			template<typename ... T_RETURN, typename ... T_ARGS>
			class Detour<Collections::Tuple<T_RETURN ...>(T_ARGS ...)>
			{
			public:
				static constexpr Collections::Tuple<T_RETURN ...> Execute(::lua_State* lua, T_ARGS ... args)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						sizeof ...(T_RETURN)
					);

					return Collections::Tuple<T_RETURN ...>(
						Pop<T_RETURN>(lua) ...
					);
				}
			};

			Lua() = delete;

		public:
			static constexpr T Execute(::lua_State* lua, TArgs ... args)
			{
				return Detour<T(TArgs ...)>::Execute(
					lua,
					Forward<TArgs>(args) ...
				);
			}
		};

		template<typename T, typename ... TArgs>
		class LuaCallback<T(TArgs ...)>
		{
			friend Function;

			::lua_State* lua;
			mutable int  reference;

			LuaCallback(const LuaCallback&) = delete;

			explicit LuaCallback(::lua_State* lua)
				: lua(
					lua
				),
				reference(
					::luaL_ref(lua, LUA_REGISTRYINDEX)
				)
			{
			}

		public:
			LuaCallback()
				: lua(
					nullptr
				)
			{
			}

			LuaCallback(LuaCallback&& callback)
				: lua(
					callback.lua
				),
				reference(
					callback.reference
				)
			{
				callback.lua = nullptr;
			}

			virtual ~LuaCallback()
			{
				if (lua != nullptr)
				{
					::luaL_unref(
						lua,
						LUA_REGISTRYINDEX,
						reference
					);

					lua = nullptr;
				}
			}

			T operator () (TArgs ... args) const
			{
				::lua_rawgeti(
					lua,
					LUA_REGISTRYINDEX,
					reference
				);

				return Lua<T(TArgs ...)>::Execute(
					lua,
					Forward<TArgs>(args) ...
				);
			}

			LuaCallback& operator = (LuaCallback&& callback)
			{
				if (lua != nullptr)
				{
					luaL_unref(
						lua,
						LUA_REGISTRYINDEX,
						reference
					);
				}

				lua = callback.lua;
				callback.lua = nullptr;

				reference = callback.reference;

				return *this;
			}
		};

	private:
		template<typename T_ARG>
		static constexpr T_ARG Get(::lua_State* lua, size_t index)
		{
			if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
			{
				return Extensions::Type_Functions<T_ARG>::Get(
					lua,
					index
				);
			}
			else if constexpr (Is_LuaCallback<T_ARG>::Value)
			{
				return T_ARG(
					lua
				);
			}
		}

		template<typename T_ARG>
		static constexpr T_ARG Pop(::lua_State* lua)
		{
			if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
			{
				return Extensions::Type_Functions<T_ARG>::Pop(
					lua,
					1
				);
			}
			else if constexpr (Is_LuaCallback<T_ARG>::Value)
			{
				return T_ARG(
					lua
				);
			}
		}

		template<typename T_ARG>
		static constexpr Void  Push(::lua_State* lua, T_ARG value)
		{
			if constexpr (Extensions::Type_Functions<T_ARG>::IsDefined)
			{
				Extensions::Type_Functions<T_ARG>::Push(
					lua,
					Forward<T_ARG>(value)
				);
			}
		}
	};

	enum class Libraries : uint8
	{
		IO,
		OS,
		Base,
		Math,
		UTF8,
		Debug,
		Table,
		String,
		Package,
		CoRoutine,

		COUNT,

		All
	};

	class State
	{
		struct Library
		{
			String          Name;
			::lua_CFunction Function;

			Library(String&& name, ::lua_CFunction function)
				: Name(
					Move(name)
				),
				Function(
					function
				)
			{
			}
		};

		inline static const Library LUA_LIBRARIES[static_cast<size_t>(Libraries::COUNT)] =
		{
			Library(LUA_IOLIBNAME,   &luaopen_io),
			Library(LUA_OSLIBNAME,   &luaopen_os),
			Library(LUA_GNAME,       &luaopen_base),
			Library(LUA_MATHLIBNAME, &luaopen_math),
			Library(LUA_UTF8LIBNAME, &luaopen_utf8),
			Library(LUA_DBLIBNAME,   &luaopen_debug),
			Library(LUA_TABLIBNAME,  &luaopen_table),
			Library(LUA_STRLIBNAME,  &luaopen_string),
			Library(LUA_LOADLIBNAME, &luaopen_package),
			Library(LUA_COLIBNAME,   &luaopen_coroutine)
		};

		Bool         isCreated = False;

		::lua_State* lua;
		Stack        stack;

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
				&Function::C<F>::Execute
			);
		}

		template<typename F, typename ... TArgs>
		auto CallGlobalFunction(const String& name, TArgs ... args)
		{
			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return Function::Lua<F>::Execute(
				GetHandle(),
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
		// @return AL::False if not found
		Bool LoadLibrary(Libraries library)
		{
			if (library == Libraries::All)
			{
				for (auto& library : LUA_LIBRARIES)
				{
					try
					{
						LoadLibrary(
							library
						);
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error loading library '%s'",
							library.Name.GetCString()
						);
					}
				}

				return True;
			}

			if (library >= Libraries::COUNT)
			{

				return False;
			}

			auto lpLibrary = &LUA_LIBRARIES[static_cast<size_t>(library)];

			try
			{
				LoadLibrary(
					*lpLibrary
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error loading library '%s'",
					lpLibrary->Name.GetCString()
				);
			}

			return True;
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

	private:
		// @throw AL::Exception
		Void LoadLibrary(const Library& library)
		{
			::luaL_requiref(
				GetHandle(),
				library.Name.GetCString(),
				library.Function,
				1
			);

			::lua_pop(
				GetHandle(),
				1
			);
		}
	};
}

inline ::lua_State* AL::Lua54::Stack::GetHandle() const
{
	return lpState->GetHandle();
}
