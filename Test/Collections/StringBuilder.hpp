#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/FileSystem/TextFile.hpp> // TextFile::LF

#include <AL/Collections/StringBuilder.hpp>

static void AL_Collections_StringBuilder()
{
	using namespace AL;
	using namespace AL::Collections;

	StringBuilder sb;
	sb << "Hello" << " world!" << AL::FileSystem::TextFile::LF;
	sb << 1 << ' ' << 2 << ',' << 3 << AL::FileSystem::TextFile::LF;

	OS::Console::WriteLine(
		sb.ToString()
	);
}
