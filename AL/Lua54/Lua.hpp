#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"
#include "AL/Collections/Dictionary.hpp"

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

		T Execute(TArgs ... args) const;
		T ExecuteProtected(Bool& success, TArgs ... args) const;

		operator Bool() const
		{
			if (lua == nullptr)
			{

				return False;
			}

			return True;
		}

		T operator () (TArgs ... args) const
		{
			return Execute(
				AL::Forward<TArgs>(args) ...
			);
		}
		T operator () (Bool& success, TArgs ... args) const
		{
			return ExecuteProtected(
				success,
				AL::Forward<TArgs>(args) ...
			);
		}

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

	template<typename T_KEY, typename T_VALUE>
	using Table = Collections::Dictionary<T_KEY, T_VALUE>;

	template<typename ... T>
	using Tuple = Collections::Tuple<T ...>;

	template<typename ... T>
	class Object
		: public Tuple<T ...>
	{
	public:
		using Tuple<T ...>::Tuple;
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
					return Push<T>(
						lua,
						F(Get<INDEXES, T_ARGS ...>(lua) ...)
					);
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
				static constexpr Void ExecuteProtected(::lua_State* lua, Bool& success, T_ARGS ... args)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					success = Extensions::pcall(
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
						Get_Stack_Size<T_RETURN>::Value
					);

					return Pop<T_RETURN>(
						lua
					);
				}
				static constexpr T_RETURN ExecuteProtected(::lua_State* lua, Bool& success, T_ARGS ... args)
				{
					(Push<T_ARGS>(lua, Forward<T_ARGS>(args)), ...);

					Extensions::pcall(
						lua,
						sizeof ...(T_ARGS),
						Get_Stack_Size<T_RETURN>::Value
					);

					return Pop<T_RETURN>(
						lua
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
			static constexpr T ExecuteProtected(::lua_State* lua, Bool& success, TArgs ... args)
			{
				return Detour<T(TArgs ...)>::ExecuteProtected(
					lua,
					success,
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
			enum class Types
			{
				Null          = LUA_TNIL,
				Number        = LUA_TNUMBER,
				Boolean       = LUA_TBOOLEAN,
				String        = LUA_TSTRING,
				Table         = LUA_TTABLE,
				Function      = LUA_TFUNCTION,
				UserData      = LUA_TUSERDATA,
				Thread        = LUA_TTHREAD,
				LightUserData = LUA_TLIGHTUSERDATA
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

			static int              gettop(::lua_State* lua)
			{
				return ::lua_gettop(lua);
			}

			static Void             createtable(::lua_State* lua, size_t size, size_t other)
			{
				::lua_createtable(
					lua,
					static_cast<int>(size & Integer<int>::SignedCastMask),
					static_cast<int>(other & Integer<int>::SignedCastMask)
				);
			}

			static size_t           rawlen(::lua_State* lua, int index)
			{
				return ::lua_rawlen(
					lua,
					index
				);
			}

			// @throw AL::Exception
			static Types            rawgeti(::lua_State* lua, int index, lua_Integer key)
			{
				switch (::lua_rawgeti(lua, index, key))
				{
					case LUA_TNIL:           return Types::Null;
					case LUA_TNUMBER:        return Types::Number;
					case LUA_TBOOLEAN:       return Types::Boolean;
					case LUA_TSTRING:        return Types::String;
					case LUA_TTABLE:         return Types::Table;
					case LUA_TFUNCTION:      return Types::Function;
					case LUA_TUSERDATA:      return Types::UserData;
					case LUA_TTHREAD:        return Types::Thread;
					case LUA_TLIGHTUSERDATA: return Types::LightUserData;
				}

				throw OperationNotSupportedException();
			}

			static Void             rawseti(::lua_State* lua, int index, lua_Integer key)
			{
				::lua_rawseti(lua, index, key);
			}

			static Bool             rawequal(::lua_State* lua, int index1, int index2)
			{
				return ::lua_rawequal(
					lua,
					index1,
					index2
				) != 0;
			}

			static Void             call(::lua_State* lua, size_t argCount, size_t returnCount)
			{
				::lua_call(
					lua,
					static_cast<int>(argCount & Integer<int>::SignedCastMask),
					static_cast<int>(returnCount & Integer<int>::SignedCastMask)
				);
			}

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
		};

		template<typename T>
		struct Is_Tuple
		{
			static constexpr Bool Value = False;
		};
		template<typename ... T>
		struct Is_Tuple<Tuple<T ...>>
		{
			static constexpr Bool Value = True;
		};

		template<typename T>
		struct Is_Table
		{
			static constexpr Bool Value = False;
		};
		template<typename T_KEY, typename T_VALUE>
		struct Is_Table<Table<T_KEY, T_VALUE>>
		{
			static constexpr Bool Value = True;
		};

		template<typename T>
		struct Is_Object
		{
			static constexpr Bool Value = False;
		};
		template<typename ... T>
		struct Is_Object<Object<T ...>>
		{
			static constexpr Bool Value = True;
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
				Is_Table<T>::Value ||
				Is_Tuple<T>::Value ||
				Is_Object<T>::Value ||
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
				Is_Table<T>::Value ||
				Is_Tuple<T>::Value ||
				Is_Object<T>::Value ||
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

		template<typename T>
		struct Get_Tuple_Size;
		template<typename ... T>
		struct Get_Tuple_Size<Tuple<T ...>>
		{
			static constexpr size_t Value = sizeof ...(T);
		};

		template<typename T>
		struct Get_Stack_Size
		{
			static constexpr size_t Value = Conditional_Value<Is_Type<T, Void>::Value, size_t, 0, 1>::Value;
		};
		template<typename ... T>
		struct Get_Stack_Size<Tuple<T ...>>
		{
			static constexpr size_t Value = sizeof ...(T);
		};
		template<typename T_KEY, typename T_VALUE>
		struct Get_Stack_Size<Table<T_KEY, T_VALUE>>
		{
			static constexpr size_t Value = 1;
		};

		template<typename T>
		struct Get_Object_Size;
		template<typename ... T>
		struct Get_Object_Size<Object<T ...>>
		{
			static constexpr size_t Value = sizeof ...(T);
		};

		template<typename T>
		struct Get_Table_Key;
		template<typename T_KEY, typename T_VALUE>
		struct Get_Table_Key<Table<T_KEY, T_VALUE>>
		{
			typedef T_KEY Type;
		};

		template<typename T>
		struct Get_Table_Value;
		template<typename T_KEY, typename T_VALUE>
		struct Get_Table_Value<Table<T_KEY, T_VALUE>>
		{
			typedef T_VALUE Type;
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

		auto GetStackSize() const
		{
			return IsCreated() ? Extensions::gettop(GetHandle()) : 0;
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
				Tuple<T ...> value;

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
				Tuple<T ...> value;

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
		Void SetGlobalFunction(const String& name, ::lua_CFunction value)
		{
			AL_ASSERT(
				IsCreated(),
				"Lua not created"
			);

			Extensions::pushcfunction(
				GetHandle(),
				value
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

		template<auto F, typename ... TArgs>
		auto CallGlobalFunctionProtected(const String& name, Bool& success, TArgs ... args)
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

			return LuaFunction<T>::ExecuteProtected(
				GetHandle(),
				success,
				Forward<TArgs>(args) ...
			);
		}
		template<typename F, typename ... TArgs>
		auto CallGlobalFunctionProtected(const String& name, Bool& success, TArgs ... args)
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

			return LuaFunction<F>::ExecuteProtected(
				GetHandle(),
				success,
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
		static int Push(::lua_State* lua, T value)
		{
			static_assert(
				Is_Type_Pushable<T>::Value,
				"Type not supported"
			);

			if constexpr (Is_Tuple<T>::Value)
			{
				return PushTuple(
					lua,
					value
				);
			}
			else if constexpr (Is_Table<T>::Value)
			{
				typedef typename Get_Table_Key<T>::Type   T_KEY;
				typedef typename Get_Table_Value<T>::Type T_VALUE;

				Extensions::createtable(
					lua,
					value.GetSize(),
					0
				);

				auto top = Extensions::gettop(
					lua
				);

				for (auto& pair : value)
				{
					Push<T_KEY>(
						lua,
						Forward<T_KEY>(pair.Key)
					);

					Push<T_VALUE>(
						lua,
						Forward<T_VALUE>(pair.Value)
					);

					::lua_settable(
						lua,
						top
					);
				}

				return 1;
			}
			else if constexpr (Is_Object<T>::Value)
			{
				Extensions::createtable(
					lua,
					Get_Object_Size<T>::Value,
					0
				);

				PushObject(
					lua,
					Extensions::gettop(lua),
					value,
					typename Make_Index_Sequence<Get_Object_Size<T>::Value>::Type {}
				);

				return 1;
			}
			else if constexpr (Is_Type<T, Bool>::Value)
			{
				Extensions::pushboolean(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, char>::Value)
			{
				Extensions::pushchar(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, char*>::Value)
			{
				Extensions::pushstring(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, const char*>::Value)
			{
				Extensions::pushstring(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, String>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, String&>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Type<T, const String&>::Value)
			{
				Extensions::pushString(
					lua,
					value
				);

				return 1;
			}
			else if constexpr (Is_Decimal<T>::Value)
			{
				Extensions::pushnumber(
					lua,
					static_cast<::lua_Number>(value)
				);

				return 1;
			}
			else if constexpr (Is_Enum_Or_Integer<T>::Value)
			{
				Extensions::pushinteger(
					lua,
					static_cast<::lua_Integer>(value)
				);

				return 1;
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

				return 1;
			}
			else if constexpr (Is_Reference<T>::Value)
			{
				Extensions::pushlightuserdata(
					lua,
					const_cast<Void*>(&value)
				);

				return 1;
			}

			return 0;
		}

		template<typename T>
		static auto Pop(::lua_State* lua)
		{
			if constexpr (Is_Tuple<T>::Value)
			{
				T value;

				PopTuple(
					lua,
					value,
					typename Make_Index_Sequence<Get_Tuple_Size<T>::Value>::Type {}
				);

				return value;
			}
			else if constexpr (Is_Table<T>::Value)
			{
				// TODO: implement
			}
			else if constexpr (Is_Object<T>::Value)
			{
				// TODO: implement
			}
			else
			{
				auto value = Peek<T>(
					lua,
					1
				);

				Extensions::pop(
					lua,
					1
				);
			}
		}

		template<typename T>
		static auto Peek(::lua_State* lua, size_t index)
		{
			static_assert(
				Is_Type_Peekable<T>::Value,
				"Type not supported"
			);

			if constexpr (Is_Tuple<T>::Value)
			{
				T value;

				PeekTuple(
					lua,
					index,
					value
				);

				return value;
			}
			else if constexpr (Is_Table<T>::Value)
			{
				// TODO: implement
			}
			else if constexpr (Is_Object<T>::Value)
			{
				// TODO: implement
			}
			else if constexpr (Is_CFunction<T>::Value)
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
		static constexpr int PushTuple(::lua_State* lua, const Tuple<T ...>& value)
		{
			return PushTuple<T ...>(
				lua,
				value,
				typename Make_Index_Sequence<sizeof ...(T)>::Type {}
			);
		}
		template<typename ... T, size_t ... INDEXES>
		static constexpr int PushTuple(::lua_State* lua, const Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			return (Push<T>(lua, value.template Get<INDEXES>()) + ...);
		}

		template<size_t INDEX, typename ... T>
		static constexpr Void PopTuple(::lua_State* lua, Tuple<T ...>& value)
		{
			value.template Set<INDEX>(
				Pop<typename Get_Type_Sequence<INDEX, T ...>::Type>(lua)
			);
		}
		template<size_t ... INDEXES, typename ... T>
		static constexpr Void PopTuple(::lua_State* lua, Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			(PopTuple<INDEXES, T>(lua, value), ...);
		}

		template<typename ... T, size_t INDEX>
		static constexpr Void PeekTuple(::lua_State* lua, size_t index, Tuple<T ...>& value)
		{
			value.template Set<INDEX>(
				Peek<typename Get_Type_Sequence<INDEX, T ...>::Type>(lua, index + INDEX)
			);
		}
		template<typename ... T, size_t ... INDEXES>
		static constexpr Void PeekTuple(::lua_State* lua, size_t index, Tuple<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			(PeekTuple<INDEXES, T>(lua, index, value), ...);
		}

		template<size_t INDEX, typename ... T>
		static constexpr int PushObject(::lua_State* lua, int stack, const Object<T ...>& value)
		{
			Push(lua, value.template Get<INDEX>());
			Extensions::rawseti(lua, stack, INDEX + 1);

			return 1;
		}
		template<size_t ... INDEXES, typename ... T>
		static constexpr int PushObject(::lua_State* lua, int stack, const Object<T ...>& value, Index_Sequence<INDEXES ...>)
		{
			return (PushObject<INDEXES>(lua, stack, value) + ...);
		}
	};
}

template<typename T, typename ... TArgs>
inline T AL::Lua54::Function<T(TArgs ...)>::Execute(TArgs ... args) const
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
template<typename T, typename ... TArgs>
inline T AL::Lua54::Function<T(TArgs ...)>::ExecuteProtected(Bool& success, TArgs ... args) const
{
	::lua_rawgeti(
		lua,
		LUA_REGISTRYINDEX,
		reference
	);

	return Lua::LuaFunction<T(TArgs ...)>::ExecuteProtected(
		lua,
		success,
		Forward<TArgs>(args) ...
	);
}
