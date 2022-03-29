#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/SQLite3/Database.hpp>

// @throw AL::Exception
static void AL_SQLite3_Database()
{
	using namespace AL;
	using namespace AL::SQLite3;

	Database db(
		FileSystem::Path("sqlite3.db"),
		DatabaseFlags::Memory | DatabaseFlags::Create | DatabaseFlags::ReadWrite
	);

	db.Open();

	db.Query(
		"CREATE TABLE IF NOT EXISTS test ("
			"ID INTEGER UNIQUE PRIMARY KEY,"
			"Value TEXT NOT NULL"
		") WITHOUT ROWID;"
	);

	db.Query(
		"INSERT INTO test VALUES(1, 'hello');"
		"INSERT INTO test VALUES(2, 'world');"
	);

	auto result = db.Query(
		"SELECT * FROM test;"
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	for (auto& columnName : result.begin()->Columns)
	{
		OS::Console::Write(
			"%-10s",
			columnName.GetCString()
		);
	}

	OS::Console::WriteLine();

	for (auto& row : result)
	{
		for (auto& columnValue : row.Values)
		{
			OS::Console::Write(
				"%-10s",
				columnValue.GetCString()
			);
		}

		OS::Console::WriteLine();
	}

#endif

	db.Close();
}
