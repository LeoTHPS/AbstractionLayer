#pragma once
#include "AL/Common.hpp"

#include "SQLiteException.hpp"

#include "AL/FileSystem/Path.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Tuple.hpp"
#include "AL/Collections/LinkedList.hpp"

#if __has_include(<sqlite3.h>)
	#define AL_DEPENDENCY_SQLITE3

	#include <sqlite3.h>
#endif

namespace AL::SQLite3
{
	enum class DatabaseFlags : uint16
	{
		None         = 0x00,

		// The filename can be interpreted as a URI if this flag is set
		URI          = 0x01,
		// The database is created if it does not already exist
		Create       = 0x02,
		// The database is opened in read-only mode
		ReadOnly     = 0x04,
		// The database is opened for reading and writing if possible, or reading only if the file is write protected by the operating system
		ReadWrite    = 0x08,
		// The database will be opened as an in-memory database
		// The database is named by the path argument for the purposes of cache-sharing
		Memory       = 0x10,

		// The new database connection will use the "multi-thread" threading mode
		// This means that separate threads are allowed to use SQLite at the same time, as long as each thread is using a different database connection
		NoMutex      = 0x20,
		// The new database connection will use the "serialized" threading mode
		// This means the multiple threads can safely attempt to use the same database connection at the same time
		FullMutex    = 0x40,

		// The database filename is not allowed to be a symbolic link
		NoFollow     = 0x80,

		// The database is opened with shared cache enabled
		SharedCache  = 0x100,
		// The database is opened with shared cache disabled
		PrivateCache = 0x200,
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(DatabaseFlags);

	struct DatabaseQueryResultRow
	{
		Collections::Array<String> Columns;
		Collections::Array<String> Values;
	};

	typedef Collections::LinkedList<DatabaseQueryResultRow> DatabaseQueryResult;

	class Database
	{
		Bool                   isOpen = False;

		FileSystem::Path       path;
		BitMask<DatabaseFlags> flags;

#if defined(AL_DEPENDENCY_SQLITE3)
		::sqlite3*             db = nullptr;
#endif

		Database(const Database&) = delete;

	public:
		Database(Database&& database)
			: isOpen(
				database.isOpen
			),
			path(
				Move(database.path)
			),
			flags(
				Move(database.flags)
			)
#if defined(AL_DEPENDENCY_SQLITE3)
			,
			db(
				database.db
			)
#endif
		{
			database.isOpen = False;
			database.db     = nullptr;
		}

		Database(FileSystem::Path&& path, DatabaseFlags flags = DatabaseFlags::Create | DatabaseFlags::ReadWrite)
			: path(
				Move(path)
			),
			flags(
				flags
			)
		{
		}
		Database(const FileSystem::Path& path, DatabaseFlags flags = DatabaseFlags::Create | DatabaseFlags::ReadWrite)
			: Database(
				FileSystem::Path(path),
				flags
			)
		{
		}

		virtual ~Database()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		auto& GetPath() const
		{
			return path;
		}

		auto& GetFlags() const
		{
			return flags;
		}

		auto GetHandle() const
		{
#if defined(AL_DEPENDENCY_SQLITE3)
			return db;
#endif
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"Database already open"
			);

#if defined(AL_DEPENDENCY_SQLITE3)
			int flags = 0;

			if (GetFlags().IsSet(DatabaseFlags::URI))          flags |= SQLITE_OPEN_URI;
			if (GetFlags().IsSet(DatabaseFlags::Create))       flags |= SQLITE_OPEN_CREATE;
			if (GetFlags().IsSet(DatabaseFlags::ReadOnly))     flags |= SQLITE_OPEN_READONLY;
			if (GetFlags().IsSet(DatabaseFlags::ReadWrite))    flags |= SQLITE_OPEN_READWRITE;
			if (GetFlags().IsSet(DatabaseFlags::Memory))       flags |= SQLITE_OPEN_MEMORY;
			if (GetFlags().IsSet(DatabaseFlags::NoMutex))      flags |= SQLITE_OPEN_NOMUTEX;
			if (GetFlags().IsSet(DatabaseFlags::FullMutex))    flags |= SQLITE_OPEN_FULLMUTEX;
#if defined(AL_PLATFORM_WINDOWS)
			if (GetFlags().IsSet(DatabaseFlags::NoFollow))     flags |= SQLITE_OPEN_NOFOLLOW;
#endif
			if (GetFlags().IsSet(DatabaseFlags::SharedCache))  flags |= SQLITE_OPEN_SHAREDCACHE;
			if (GetFlags().IsSet(DatabaseFlags::PrivateCache)) flags |= SQLITE_OPEN_PRIVATECACHE;

			if ((::sqlite3_open_v2(GetPath().GetString().GetCString(), &db, flags, nullptr)) != SQLITE_OK)
			{
				db = nullptr;

				throw SQLiteException(
					db,
					"sqlite3_open_v2"
				);
			}
#else
			throw DependencyMissingException(
				"SQLite3"
			);
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_DEPENDENCY_SQLITE3)
				::sqlite3_close(
					db
				);

				db = nullptr;
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		template<size_t S, typename ... TArgs>
		auto Query(const char(&format)[S], TArgs ... args)
		{
			AL_ASSERT(
				IsOpen(),
				"Database not open"
			);

			struct Context
			{
				String              Query;
				DatabaseQueryResult Result;
				::sqlite3_callback  Callback;
				Database*           lpDatabase;
			};

			Context context =
			{
				.Query      = String::Format(format, Forward<TArgs>(args) ...),
				.Callback   = [](void* _lpParam, int _column_count, char** _column_values, char** _column_names)->int
				{
					auto lpContext = reinterpret_cast<Context*>(
						_lpParam
					);

					DatabaseQueryResultRow row =
					{
						.Columns = decltype(DatabaseQueryResultRow::Columns)(_column_count),
						.Values  = decltype(DatabaseQueryResultRow::Values)(_column_count)
					};

					for (int i = 0; i < _column_count; ++i, ++_column_names, ++_column_values)
					{
						row.Columns[i].Assign(
							*_column_names
						);

						row.Values[i].Assign(
							*_column_values
						);
					}

					lpContext->Result.PushBack(
						Move(row)
					);

					return SQLITE_OK;
				},
				.lpDatabase = this
			};

			if (::sqlite3_exec(GetHandle(), context.Query.GetCString(), context.Callback, &context, nullptr) != SQLITE_OK)
			{

				throw SQLiteException(
					db,
					"sqlite3_exec"
				);
			}

			return context.Result;
		}

		Database& operator = (Database&& database)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = database.isOpen;
			database.isOpen = False;

			path = Move(
				database.path
			);

			flags = Move(
				database.flags
			);

#if defined(AL_DEPENDENCY_SQLITE3)
			db = database.db;
			database.db = nullptr;
#endif

			return *this;
		}

		Bool operator == (const Database& database) const
		{
			if (GetPath() != database.GetPath())
			{

				return False;
			}

#if defined(AL_DEPENDENCY_SQLITE3)
			if (GetHandle() != database.GetHandle())
			{

				return False;
			}
#endif

			return True;
		}
		Bool operator != (const Database& database) const
		{
			if (operator==(database))
			{

				return False;
			}

			return True;
		}
	};
}
