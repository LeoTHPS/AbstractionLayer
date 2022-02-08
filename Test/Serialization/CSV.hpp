#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Serialization/CSV.hpp>

// @throw AL::Exception
static void AL_Serialization_CSV()
{
	using namespace AL;
	using namespace AL::Serialization;

	CSV<String, String, String> csv;
	csv.Add("hello", "world", "1,2,3,4");

	auto temp = csv.ToString();

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		temp
	);
#endif

	decltype(csv)::FromString(
		csv,
		temp
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		csv.ToString()
	);
#endif
}
