#pragma once
#include "AL/Common.hpp"

#include "Exception.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"

#if !AL_HAS_INCLUDE(<mysql.h>)
	#error Missing mysql.h
#endif

#include <mysql.h>
#include <errmsg.h>

namespace AL::MySQL
{
	struct DatabaseQueryResultRow
	{
		Collections::Array<String> Columns;
		Collections::Array<String> Values;
	};

	typedef Collections::LinkedList<DatabaseQueryResultRow> DatabaseQueryResult;
	typedef Collections::LinkedList<DatabaseQueryResult>    DatabaseQueryResults;

	class Database
	{
		bool     isConnected = False;

		::MYSQL* db    = nullptr;
		uint32   error = 0;

		Database(Database&&) = delete;
		Database(const Database&) = delete;

	public:
		Database()
		{
		}

		virtual ~Database()
		{
			if (IsConnected())
				Disconnect();
		}

		Bool IsConnected() const
		{
			return isConnected;
		}

		auto GetHandle() const
		{
			return db;
		}

		auto GetLastError() const
		{
			return error;
		}

		auto GetChangeCount() const
		{
			return static_cast<uint64>(IsConnected() ? ::mysql_affected_rows(GetHandle()) : 0);
		}

		auto GetLastInsertRowID() const
		{
			return static_cast<uint64>(IsConnected() ? ::mysql_insert_id(GetHandle()) : 0);
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Connect(const String& host, uint16 port, const String& username, const String& password)
		{
			AL_ASSERT(!IsConnected(), "Database already connected");

			if ((db = ::mysql_init(nullptr)) == nullptr)
				throw Exception("mysql_init", CR_OUT_OF_MEMORY, "");

			if (::mysql_real_connect(GetHandle(), host.GetCString(), username.GetCString(), password.GetCString(), nullptr, port, nullptr, CLIENT_COMPRESS | CLIENT_MULTI_STATEMENTS | CLIENT_REMEMBER_OPTIONS) == nullptr)
			{
				switch (error = ::mysql_errno(GetHandle()))
				{
					case CR_SERVER_LOST:
					case CR_UNKNOWN_HOST:
					case CR_CONN_HOST_ERROR:
					case CR_CONNECTION_ERROR:
						::mysql_close(GetHandle());
						db = nullptr;
						return False;
				}

				String errorString(::mysql_error(GetHandle()));

				::mysql_close(GetHandle());
				db = nullptr;

				throw Exception("mysql_real_connect", GetLastError(), errorString);
			}

			isConnected = True;

			return True;
		}
		Void Disconnect()
		{
			if (IsConnected())
			{
				::mysql_close(GetHandle());
				db = nullptr;

				isConnected = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Query(DatabaseQueryResults& results, const String& value)
		{
			AL_ASSERT(IsConnected(), "Database not connected");

			if ((error = ::mysql_real_query(GetHandle(), value.GetCString(), value.GetLength())) != 0)
			{
				String errorString(::mysql_error(GetHandle()));

				switch (GetLastError())
				{
					case CR_SERVER_LOST:
					case CR_SERVER_GONE_ERROR:
						Disconnect();
						return False;
				}

				throw Exception("mysql_real_query", GetLastError(), errorString);
			}

			while (auto mysqlResult = ::mysql_store_result(GetHandle()))
			{
				DatabaseQueryResult result;

				auto mysqlFieldCount = ::mysql_num_fields(mysqlResult);

				while (auto mysqlRow = ::mysql_fetch_row(mysqlResult))
				{
					auto mysqlLengths = ::mysql_fetch_lengths(mysqlResult);

					DatabaseQueryResultRow resultRow =
					{
						.Columns = decltype(DatabaseQueryResultRow::Columns)(mysqlFieldCount),
						.Values  = decltype(DatabaseQueryResultRow::Values)(mysqlFieldCount)
					};

					auto mysqlFields = ::mysql_fetch_fields(mysqlResult);

					for (size_t i = 0; i < mysqlFieldCount; ++i)
					{
						resultRow.Columns[i] = mysqlFields[i].name;
						resultRow.Values[i]  = AL::String(mysqlRow[i], mysqlLengths[i]);
					}

					result.PushBack(AL::Move(resultRow));
				}

				::mysql_free_result(mysqlResult);

				results.PushBack(AL::Move(result));

				if ((error = ::mysql_next_result(GetHandle())) > 0)
					throw Exception("mysql_next_result");
				else if (GetLastError() == -1)
				{
					error = 0;

					break;
				}
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename ... TArgs>
		Bool Query(DatabaseQueryResults& results, const String::Char* format, TArgs ... args)
		{
			return Query(results, String::Format(format, Forward<TArgs>(args) ...));
		}

		auto EscapeString(const String& value)
		{
			AL_ASSERT(IsConnected(), "Database not connected");

			String string(String::END, value.GetLength());
			size_t stringLength;

			if ((stringLength = ::mysql_real_escape_string(GetHandle(), &string[0], value.GetCString(), value.GetLength())) == -1)
			{

				throw Exception(
					"mysql_real_escape_string"
				);
			}

			string.SetCapacity(stringLength);

			return string;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SelectDatabase(const String& value)
		{
			AL_ASSERT(IsConnected(), "Database not connected");

			if ((error = ::mysql_select_db(GetHandle(), value.GetCString())) != 0)
			{
				String errorString(::mysql_error(GetHandle()));

				switch (GetLastError())
				{
					case CR_SERVER_LOST:
					case CR_SERVER_GONE_ERROR:
						Disconnect();
						return False;
				}

				throw Exception("mysql_select_db", GetLastError(), errorString);
			}

			return True;
		}
	};
}
