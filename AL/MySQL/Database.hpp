#pragma once
#include "AL/Common.hpp"

#include "Exception.hpp"

#include "AL/Collections/Array.hpp"

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

			if (::mysql_real_connect(GetHandle(), host.GetCString(), username.GetCString(), password.GetCString(), nullptr, port, nullptr, 0) == nullptr)
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
		auto Query(const String& value)
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
						break;
				}

				throw Exception("mysql_real_query", GetLastError(), errorString);
			}

			DatabaseQueryResult queryResult;

			if (auto result = ::mysql_store_result(GetHandle()))
			{
				auto fieldCount = ::mysql_num_fields(result);

				while (auto row = ::mysql_fetch_row(result))
				{
					auto lengths = ::mysql_fetch_lengths(result);

					DatabaseQueryResultRow queryResultRow =
					{
						.Columns = decltype(DatabaseQueryResultRow::Columns)(fieldCount),
						.Values  = decltype(DatabaseQueryResultRow::Values)(fieldCount)
					};

					auto fields = ::mysql_fetch_fields(result);

					for (size_t i = 0; i < fieldCount; ++i)
					{
						queryResultRow.Columns[i] = fields[i].name;
						queryResultRow.Values[i]  = AL::String(row[i], lengths[i]);
					}

					queryResult.PushBack(AL::Move(queryResultRow));
				}

				::mysql_free_result(result);
			}

			return queryResult;
		}
		// @throw AL::Exception
		template<typename ... TArgs>
		auto Query(const String::Char* format, TArgs ... args)
		{
			return Query(String::Format(format, Forward<TArgs>(args) ...));
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
		Void SelectDatabase(const String& value)
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
						break;
				}

				throw Exception("mysql_select_db", GetLastError(), errorString);
			}
		}
	};
}
