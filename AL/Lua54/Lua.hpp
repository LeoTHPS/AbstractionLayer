#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"

#if !defined(AL_PLATFORM_PICO)
	#include "AL/FileSystem/Path.hpp"
#endif

#if AL_HAS_INCLUDE(<lua.hpp>)
	#include <lua.hpp>
#elif AL_HAS_INCLUDE(<lua5.4/lua.hpp>)
	#include <lua5.4/lua.hpp>
#else
	#error Missing lua.hpp
#endif

#if defined(AL_PLATFORM_WINDOWS)
	#undef LoadLibrary
#endif

namespace AL::Lua54
{
	class Lua;

	template<typename F>
	class Function;
	template<typename T, typename ... TArgs>
	class Function<T(TArgs ...)>
	{
		friend Lua;

		::lua_State* lua              = nullptr;
		int          reference;
		size_t*      lpReferenceCount = nullptr;

		Function(::lua_State* lua, size_t index)
			: lua(
				lua
			),
			reference(
				[lua, index]()
				{
					::lua_pushvalue(
						lua,
						index
					);

					return ::luaL_ref(
						lua,
						LUA_REGISTRYINDEX
					);
				}()
			),
			lpReferenceCount(
				new size_t(1)
			)
		{
		}

	public:
		Function()
		{
		}

		Function(Function&& function)
			: lua(
				function.lua
			),
			reference(
				function.reference
			),
			lpReferenceCount(
				function.lpReferenceCount
			)
		{
			function.lua              = nullptr;
			function.lpReferenceCount = nullptr;
		}
		Function(const Function& function)
			: lua(
				function.lua
			),
			reference(
				function.reference
			),
			lpReferenceCount(
				function.lpReferenceCount
			)
		{
			if (lpReferenceCount != nullptr)
			{

				++(*lpReferenceCount);
			}
		}

		virtual ~Function()
		{
			Release();
		}

		Bool IsNull() const
		{
			return (lua != nullptr) && (reference == -1);
		}

		Void Release()
		{
			if (lpReferenceCount != nullptr)
			{
				if (--(*lpReferenceCount) == 0)
				{
					::luaL_unref(
						lua,
						LUA_REGISTRYINDEX,
						reference
					);
				}

				lua              = nullptr;
				lpReferenceCount = nullptr;
			}
		}

		operator Bool() const
		{
			if (lua == nullptr)
			{

				return False;
			}

			return True;
		}

		T operator () (TArgs ... args) const;

		Function& operator = (Function&& function)
		{
			Release();

			lua = function.lua;
			function.lua = nullptr;

			reference = function.reference;

			lpReferenceCount = function.lpReferenceCount;
			function.lpReferenceCount = nullptr;

			return *this;
		}
		Function& operator = (const Function& function)
		{
			Release();

			lua              = function.lua;
			reference        = function.reference;
			lpReferenceCount = function.lpReferenceCount;

			if (lpReferenceCount != nullptr)
			{

				++(*lpReferenceCount);
			}

			return *this;
		}

		Bool operator == (const Function& function) const
		{
			if (lua != function.lua)
			{

				return False;
			}

			if (reference != function.reference)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Function& function) const
		{
			if (operator==(function))
			{

				return False;
			}

			return True;
		}
	};
	template<typename T, typename ... TArgs>
	class Function<T(*)(TArgs ...)>
		: public Function<T(TArgs ...)>
	{
	public:
		using Function<T(TArgs ...)>::Function;
	};

	class Exception
		: public AL::Exception
	{
	public:
		template<size_t S>
		explicit Exception(const char(&function)[S])
			: AL::Exception(
				"Error calling '%s'",
				&function[0]
			)
		{
		}

		template<size_t S>
		Exception(::lua_State* lua, const char(&function)[S])
			: AL::Exception(
				"Error calling '%s': %s",
				&function[0],
				::lua_tostring(lua, -1)
			)
		{
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

		All
	};

	class Lua
	{
		template<typename F>
		friend class Function;

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

		template<auto F>
		class CFunction
		{
			template<typename T>
			class Detour;
			template<typename T, typename ... T_ARGS>
			class Detour<T(*)(T_ARGS ...)>
			{
			public:
				static int Execute(::lua_State* lua)
				{
					return Execute(
						lua,
						typename Make_Index_Sequence<sizeof ...(T_ARGS)>::Type {}
					);
				}

			private:
				template<size_t ... INDEXES>
				static int Execute(::lua_State* lua, Index_Sequence<INDEXES ...>)
				{
					auto value = F(
						Get<INDEXES, T_ARGS ...>(lua) ...
					);

					Push<T>(
						lua,
						Forward<T>(value)
					);

					return 1;
				}
			};
			template<typename ... T_ARGS>
			class Detour<Void(*)(T_ARGS ...)>
			{
			public:
				static int Execute(::lua_State* lua)
				{
					return Execute(
						lua,
						typename Make_Index_Sequence<sizeof ...(T_ARGS)>::Type {}
					);
				}

			private:
				template<size_t ... INDEXES>
				static int Execute(::lua_State* lua, Index_Sequence<INDEXES ...>)
				{
					F(
						Get<INDEXES, T_ARGS ...>(lua) ...
					);

					return 0;
				}
			};
			template<typename ... T_RETURN, typename ... T_ARGS>
			class Detour<Collections::Tuple<T_RETURN ...>(*)(T_ARGS ...)>
			{
			public:
				static int Execute(::lua_State* lua)
				{
					return Execute(
						lua,
						typename Make_Index_Sequence<sizeof ...(T_RETURN)>::Type {},
						typename Make_Index_Sequence<sizeof ...(T_ARGS)>::Type {}
					);
				}

			private:
				template<size_t ... I_RETURN, size_t ... I_ARGS>
				static int Execute(::lua_State* lua, Index_Sequence<I_RETURN ...>, Index_Sequence<I_ARGS ...>)
				{
					auto value = F(
						Get<I_ARGS, T_ARGS ...>(lua) ...
					);

					PushTuple<T_RETURN ...>(
						lua,
						value
					);

					return sizeof ...(T_RETURN);
				}
			};

			CFunction() = delete;

		public:
			static int Execute(::lua_State* lua)
			{
				return Detour<decltype(F)>::Execute(
					lua
				);
			}

		private:
			template<size_t INDEX, typename ... T>
			static constexpr auto Get(::lua_State* lua)
			{
				return Peek<typename Get_Type_Sequence<INDEX, T ...>::Type>(
					lua,
					1 + INDEX
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

					return Peek<T_RETURN>(
						lua,
						1
					);
				}
			};
			template<typename ... T_RETURN, typename ... T_ARGS>
			class Detour<Collections::Tuple<T_RETURN ...>(T_ARGS ...)>
			{
			public:
				static constexpr Collections::Tuple<T_RETURN ...> Execute(::lua_State* lua, T_ARGS ... args)
				{
					return Execute(
						lua,
						Forward<T_ARGS>(args) ...,
						typename Make_Index_Sequence<sizeof ...(T_RETURN)>::Type {}
					);
				}

			private:
				template<size_t INDEX>
				static constexpr auto Get(::lua_State* lua)
				{
					return Peek<typename Get_Type_Sequence<INDEX, T_RETURN ...>::Type>(
						lua,
						1 + INDEX
					);
				}

				template<size_t ... INDEXES>
				static constexpr Collections::Tuple<T_RETURN ...> Execute(::lua_State* lua, T_ARGS ... args, Index_Sequence<INDEXES ...>)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					Extensions::call(
						lua,
						sizeof ...(T_ARGS),
						sizeof ...(T_RETURN)
					);

					return Collections::Tuple<T_RETURN ...>(
						Get<INDEXES>(lua) ...
					);
				}
			};

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
		class LuaFunction<T(*)(TArgs ...)>
			: public LuaFunction<T(TArgs ...)>
		{
		};

		class Extensions
		{
			Extensions() = delete;

		public:
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

			// @throw AL::Exception
			static ::lua_State*     newstate()
			{
				::lua_State* lua;

				if ((lua = ::luaL_newstate()) == nullptr)
				{

					throw Exception(
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

					throw Exception(
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

					throw Exception(
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

#if defined(AL_COMPILER_GNU)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-function"
#endif
			static Bool             pcall(::lua_State* lua, size_t argCount, size_t returnCount)
			{
				switch (::lua_pcall(lua, static_cast<int>(argCount & Integer<int>::SignedCastMask), static_cast<int>(returnCount & Integer<int>::SignedCastMask), 0))
				{
					case 0:
						break;

					case LUA_ERRRUN:
						return False;

					default:
						throw Exception(lua, "lua_pcall");
				}

				return True;
			}
#if defined(AL_COMPILER_GNU)
	#pragma GCC diagnostic pop
#endif
		};

		template<typename T>
		struct Is_CFunction
		{
			static constexpr Bool Value = False;
		};
		template<typename T, typename ... TArgs>
		struct Is_CFunction<T(TArgs ...)>
		{
			static constexpr Bool Value = True;
		};
		template<typename T, typename ... TArgs>
		struct Is_CFunction<T(*)(TArgs ...)>
		{
			static constexpr Bool Value = True;
		};

		template<typename T>
		struct Is_LuaFunction
		{
			static constexpr Bool Value = False;
		};
		template<typename T, typename ... TArgs>
		struct Is_LuaFunction<Function<T(TArgs ...)>>
		{
			static constexpr Bool Value = True;
		};
		template<typename T, typename ... TArgs>
		struct Is_LuaFunction<Function<T(*)(TArgs ...)>>
		{
			static constexpr Bool Value = True;
		};

		template<typename T>
		struct Is_Type_Pushable
		{
			static constexpr Bool Value =
				Is_Type<T, Bool>::Value ||
				Is_Type<T, char>::Value ||
				Is_Type<T, char*>::Value ||
				Is_Type<T, const char*>::Value ||
				Is_Type<T, String>::Value ||
				Is_Type<T, String&>::Value ||
				Is_Type<T, const String&>::Value ||
				Is_Decimal<T>::Value ||
				Is_Enum_Or_Integer<T>::Value ||
				Is_Pointer<T>::Value ||
				Is_Reference<T>::Value;
		};

		template<typename T>
		struct Is_Type_Peekable
		{
			static constexpr Bool Value =
				Is_CFunction<T>::Value ||
				Is_LuaFunction<T>::Value ||
				Is_Type<T, ::std::nullptr_t>::Value ||
				Is_Type<T, Bool>::Value ||
				Is_Type<T, char>::Value ||
				Is_Type<T, char*>::Value ||
				Is_Type<T, const char*>::Value ||
				Is_Type<T, String>::Value ||
				Is_Type<T, String&>::Value ||
				Is_Type<T, const String&>::Value ||
				Is_Decimal<T>::Value ||
				Is_Enum_Or_Integer<T>::Value ||
				Is_Pointer<T>::Value ||
				Is_Reference<T>::Value;
		};

		template<typename F>
		struct CFunction_To_LuaFunction;
		template<typename T, typename ... TArgs>
		struct CFunction_To_LuaFunction<T(TArgs ...)>
		{
			typedef Function<T(TArgs ...)> Type;
		};
		template<typename T, typename ... TArgs>
		struct CFunction_To_LuaFunction<T(*)(TArgs ...)>
		{
			typedef Function<T(TArgs ...)> Type;
		};

		inline static const Library LUA_LIBRARIES[static_cast<size_t>(Libraries::All)] =
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

		Lua(Lua&&) = delete;
		Lua(const Lua&) = delete;

	public:
		Lua()
		{
		}

		virtual ~Lua()
		{
			if (IsCreated())
			{

				Destroy();
			}
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		::lua_State* GetHandle() const
		{
			return lua;
		}

		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"Lua already created"
			);

			lua = Extensions::newstate();

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				Extensions::close(
					GetHandle()
				);

				isCreated = False;
			}
		}

		template<typename ... T>
		Void Push(T ... value)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			(Push<T>(GetHandle(), Forward<T>(value)), ...);
		}

		template<typename ... T>
		auto Pop()
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			if constexpr (sizeof ...(T) == 1)
				return Pop<T ...>(GetHandle());
			else
			{
				Collections::Tuple<T ...> value;

				Pop(
					GetHandle(),
					value,
					typename Make_Index_Sequence<sizeof ...(T)>::Type {}
				);

				return value;
			}
		}
		Void Pop(size_t count = 1)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			Extensions::pop(
				GetHandle(),
				count
			);
		}

		template<typename ... T>
		auto Peek(size_t index = 1) const
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			if constexpr (sizeof ...(T) == 1)
				return Peek<T ...>(GetHandle(), index);
			else
			{
				Collections::Tuple<T ...> value;

				Peek(
					GetHandle(),
					value,
					index,
					typename Make_Index_Sequence<sizeof ...(T)>::Type {}
				);

				return value;
			}
		}

		template<typename T>
		auto GetGlobal(const String& name)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			static_assert(
				!Is_CFunction<T>::Value && !Is_LuaFunction<T>::Value,
				"Use GetGlobalFunction instead"
			);

			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return Pop<T>();
		}

		template<typename T>
		Void SetGlobal(const String& name, T value)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			static_assert(
				!Is_CFunction<T>::Value && !Is_LuaFunction<T>::Value,
				"Use SetGlobalFunction instead"
			);

			Push(
				Forward<T>(value)
			);

			Extensions::setGlobal(
				GetHandle(),
				name
			);
		}

		template<auto F>
		auto GetGlobalFunction(const String& name)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			typedef decltype(F) T;

			static_assert(
				Is_CFunction<T>::Value || Is_LuaFunction<T>::Value,
				"Use GetGlobal instead"
			);

			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return Pop<T>();
		}
		template<typename F>
		auto GetGlobalFunction(const String& name)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			static_assert(
				Is_CFunction<F>::Value || Is_LuaFunction<F>::Value,
				"Use GetGlobal instead"
			);

			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return Pop<F>();
		}

		template<auto F>
		Void SetGlobalFunction(const String& name)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			typedef decltype(F) T;

			static_assert(
				Is_CFunction<T>::Value || Is_LuaFunction<T>::Value,
				"Use SetGlobal instead"
			);

			Extensions::pushcfunction(
				GetHandle(),
				&CFunction<F>::Execute
			);

			Extensions::setGlobal(
				GetHandle(),
				name
			);
		}

		template<auto F, typename ... TArgs>
		auto CallGlobalFunction(const String& name, TArgs ... args)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			typedef decltype(F) T;

			static_assert(
				Is_CFunction<T>::Value || Is_LuaFunction<T>::Value,
				"Type is not a function"
			);

			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return LuaFunction<T>::Execute(
				GetHandle(),
				Forward<TArgs>(args) ...
			);
		}
		template<typename F, typename ... TArgs>
		auto CallGlobalFunction(const String& name, TArgs ... args)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			static_assert(
				Is_CFunction<F>::Value || Is_LuaFunction<F>::Value,
				"Type is not a function"
			);

			Extensions::getGlobal(
				GetHandle(),
				name
			);

			return LuaFunction<F>::Execute(
				GetHandle(),
				Forward<TArgs>(args) ...
			);
		}

		Void RemoveGlobal(const String& name)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			Extensions::pushnil(
				GetHandle(),
				nullptr
			);

			Extensions::setGlobal(
				GetHandle(),
				name
			);
		}

		// @throw AL::Exception
		Void Run(const String& lua)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
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
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

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
					catch (AL::Exception& exception)
					{

						throw AL::Exception(
							Move(exception),
							"Error loading library '%s'",
							library.Name.GetCString()
						);
					}
				}

				return True;
			}

			if (library >= Libraries::All)
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
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					Move(exception),
					"Error loading library '%s'",
					lpLibrary->Name.GetCString()
				);
			}

			return True;
		}

#if !defined(AL_PLATFORM_PICO)
		// @throw AL::Exception
		// @return AL::False if file does not exist
		Bool RunFile(const String& path)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			if (!FileSystem::Path::Exists(path))
			{

				return False;
			}

			if (!FileSystem::Path::IsFile(path))
			{

				return False;
			}

			Extensions::doFile(
				GetHandle(),
				path
			);

			return True;
		}
#endif

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

			Extensions::pop(
				GetHandle(),
				1
			);
		}

	private:
		template<typename T>
		static Void Push(::lua_State* lua, T value)
		{
			static_assert(
				Is_Type_Pushable<T>::Value,
				"Type not supported"
			);

			if constexpr (Is_Type<T, Bool>::Value)
			{
				Extensions::pushboolean(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, char>::Value)
			{
				Extensions::pushchar(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, char*>::Value)
			{
				Extensions::pushstring(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, const char*>::Value)
			{
				Extensions::pushstring(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, String>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, String&>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);
			}
			else if constexpr (Is_Type<T, const String&>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);
			}
			else if constexpr (Is_Decimal<T>::Value)
			{
				Extensions::pushnumber(
					lua,
					static_cast<::lua_Number>(value)
				);
			}
			else if constexpr (Is_Enum_Or_Integer<T>::Value)
			{
				Extensions::pushinteger(
					lua,
					static_cast<::lua_Integer>(value)
				);
			}
			else if constexpr (Is_Pointer<T>::Value)
			{
				if (value == nullptr)
				{
					Extensions::pushnil(
						lua,
						nullptr
					);
				}
				else
				{
					Extensions::pushlightuserdata(
						lua,
						const_cast<Void*>(static_cast<const Void*>(value))
					);
				}
			}
			else if constexpr (Is_Reference<T>::Value)
			{
				Extensions::pushlightuserdata(
					lua,
					const_cast<Void*>(&value)
				);
			}
		}

		template<typename T>
		static auto Pop(::lua_State* lua)
		{
			auto value = Peek<T>(
				lua,
				1
			);

			Extensions::pop(
				lua,
				1
			);

			return value;
		}

		template<typename T>
		static auto Peek(::lua_State* lua, size_t index)
		{
			static_assert(
				Is_Type_Peekable<T>::Value,
				"Type not supported"
			);

			if constexpr (Is_CFunction<T>::Value)
			{
				return typename CFunction_To_LuaFunction<T>::Type(
					lua,
					index
				);
			}
			else if constexpr (Is_LuaFunction<T>::Value)
			{
				return T(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, ::std::nullptr_t>::Value)
			{
				return Extensions::getnil(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, Bool>::Value)
			{
				return Extensions::getboolean(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, char>::Value)
			{
				return Extensions::getchar(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, char*>::Value)
			{
				return const_cast<char*>(
					Extensions::getstring(lua, index)
				);
			}
			else if constexpr (Is_Type<T, const char*>::Value)
			{
				return Extensions::getstring(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, String>::Value)
			{
				return Extensions::getString(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, String&>::Value)
			{
				return Extensions::getString(
					lua,
					index
				);
			}
			else if constexpr (Is_Type<T, const String&>::Value)
			{
				return Extensions::getString(
					lua,
					index
				);
			}
			else if constexpr (Is_Decimal<T>::Value)
			{
				return static_cast<T>(
					Extensions::getnumber(lua, index)
				);
			}
			else if constexpr (Is_Enum_Or_Integer<T>::Value)
			{
				return static_cast<T>(
					Extensions::getinteger(lua, index)
				);
			}
			else if constexpr (Is_Pointer<T>::Value)
			{
				return reinterpret_cast<T>(
					Extensions::getlightuserdata(lua, index)
				);
			}
			else if constexpr (Is_Reference<T>::Value)
			{
				return *reinterpret_cast<typename Remove_Reference<T>::Type*>(
					Extensions::getlightuserdata(lua, index)
				);
			}
		}

		template<typename ... T>
		static constexpr Void PushTuple(::lua_State* lua, const Collections::Tuple<T ...>& value)
		{
			PushTuple<T ...>(
				lua,
				value,
				typename Make_Index_Sequence<sizeof ...(T)>::Type {}
			);
		}
		template<typename ... T, size_t ... INDEXES>
		static constexpr Void PushTuple(::lua_State* lua, const Collections::Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			(Push<T>(lua, value.template Get<INDEXES>()), ...);
		}

		template<size_t INDEX, typename ... T>
		static constexpr Void PopTuple(::lua_State* lua, Collections::Tuple<T ...>& value)
		{
			value.template Set<INDEX>(
				Pop<typename Get_Type_Sequence<INDEX, T ...>::Type>(lua)
			);
		}
		template<size_t ... INDEXES, typename ... T>
		static constexpr Void PopTuple(::lua_State* lua, Collections::Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			(PopTuple<INDEXES, T>(lua, value), ...);
		}

		template<size_t INDEX, typename ... T>
		static constexpr Void PeekTuple(::lua_State* lua, size_t index, Collections::Tuple<T ...>& value)
		{
			value.template Set<INDEX>(
				Peek<typename Get_Type_Sequence<INDEX, T ...>::Type>(lua, index + INDEX)
			);
		}
		template<size_t ... INDEXES, typename ... T>
		static constexpr Void PeekTuple(::lua_State* lua, size_t index, Collections::Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			(PeekTuple<INDEXES, T>(lua, index, value), ...);
		}
	};
}

template<typename T, typename ... TArgs>
inline T AL::Lua54::Function<T(TArgs ...)>::operator () (TArgs ... args) const
{
	::lua_rawgeti(
		lua,
		LUA_REGISTRYINDEX,
		reference
	);

	return Lua::LuaFunction<T(TArgs ...)>::Execute(
		lua,
		Forward<TArgs>(args) ...
	);
}
