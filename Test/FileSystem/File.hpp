#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/FileSystem/File.hpp>

// @throw AL::Exception
static void AL_FileSystem_File()
{
	using namespace AL;
	using namespace AL::FileSystem;

	File file(
		"./test.tmp"
	);

	try
	{
		file.Open(
			FileOpenModes::Read
		);
	}
	catch (AL::Exception& exception)
	{

		throw AL::Exception(
			AL::Move(exception),
			"Error opening File"
		);
	}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		file.IsOpen() ? "File exists" : "File not found"
	);
#endif

	file.Close();
}
