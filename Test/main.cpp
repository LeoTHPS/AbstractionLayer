#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Thread.hpp>
#include <AL/OS/Process.hpp>

// #include "Collections/Array.hpp"
// #include "Collections/ArrayList.hpp"
// #include "Collections/Dictionary.hpp"
// #include "Collections/LinkedList.hpp"
// #include "Collections/MPSCQueue.hpp"
// #include "Collections/Queue.hpp"
// #include "Collections/CircularQueue.hpp"
// #include "Collections/String.hpp"
// #include "Collections/StringBuilder.hpp"

// #include "Common/Function.hpp"

// #include "FileSystem/File.hpp"

// #include "Game/Engine/Window.hpp"

// #include "Game/FileSystem/DataFile.hpp"
// #include "Game/FileSystem/ConfigFile.hpp"

// #include "Game/Network/ClientServer.hpp"

// #include "Hardware/Drivers/ADS1115.hpp"
// #include "Hardware/Drivers/AT24C256.hpp"
// #include "Hardware/Drivers/MLX90640.hpp"

// #include "Lua543/Lua.hpp"

// #include "Network/Adapter.hpp"
// #include "Network/UdpSocket.hpp"

// #include "Network/HTTP/Request.hpp"

// #include "OS/Process.hpp"
// #include "OS/Thread.hpp"
// #include "OS/ThreadPool.hpp"
// #include "OS/Window.hpp"

// #include "Serialization/CSV.hpp"
// #include "Serialization/HTML.hpp"
// #include "Serialization/JSON.hpp"
// #include "Serialization/NMEA.hpp"
// #include "Serialization/XML.hpp"

// #include "SQLite3/Database.hpp"

void main_display_build_information()
{
	AL::OS::Console::WriteLine(
		"Build Information"
	);

	AL::OS::Console::WriteLine(
		"\tPlatform: %s",
		AL::ToString(AL::Platforms::Machine).GetCString()
	);
}

void main_display_system_information()
{
	AL::OS::Console::WriteLine(
		"System Information"
	);

	AL::OS::Console::WriteLine(
		"\tTimezone: %i",
		AL::OS::System::GetTimezone()
	);

	AL::OS::Console::WriteLine(
		"\tTimestamp: %llu",
		AL::OS::System::GetTimestamp().ToSeconds()
	);

	AL::OS::Console::WriteLine(
		"\tCPU Count: %s",
		AL::ToString(AL::OS::System::GetProcessorCount()).GetCString()
	);

	AL::OS::Console::WriteLine(
		"\tCPU Cache Count: %s",
		AL::ToString(AL::OS::System::GetProcessorCacheCount()).GetCString()
	);

#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
	AL::OS::Console::WriteLine(
		"\tPage Size: %s",
		AL::ToString(AL::OS::System::GetPageSize()).GetCString()
	);

	AL::OS::Console::WriteLine(
		"\tCurrent User: %s",
		AL::OS::System::GetCurrentUser().GetCString()
	);

	AL::OS::Console::WriteLine(
		"\tCurrent User ID: %lu",
		AL::OS::System::GetCurrentUserID()
	);
#elif defined(AL_PLATFORM_WINDOWS)
	AL::OS::Console::WriteLine(
		"\tPage Size: %s",
		AL::ToString(AL::OS::System::GetPageSize()).GetCString()
	);

	AL::OS::Console::WriteLine(
		"\tCurrent User: %s",
		AL::OS::System::GetCurrentUser().GetCString()
	);
#endif
}

void main_display_thread_information()
{
	AL::OS::Console::WriteLine(
		"Thread Information"
	);

#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
	AL::OS::Console::WriteLine(
		"\tThread ID: %lu",
		AL::OS::GetCurrentThreadId()
	);
#elif defined(AL_PLATFORM_WINDOWS)
	AL::OS::Console::WriteLine(
		"\tThread ID: %lu",
		AL::OS::GetCurrentThreadId()
	);

	auto lpTEB = AL::OS::GetThreadEnvironmentBlock();


#endif
}

void main_display_process_information()
{
	AL::OS::Console::WriteLine(
		"Process Information"
	);

#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)
	AL::OS::Console::WriteLine(
		"\tProcess ID: %lu",
		AL::OS::GetCurrentProcessId()
	);
#elif defined(AL_PLATFORM_WINDOWS)
	AL::OS::Console::WriteLine(
		"\tProcess ID: %lu",
		AL::OS::GetCurrentProcessId()
	);
	auto lpPEB = AL::OS::GetProcessEnvironmentBlock();

	AL::OS::Console::WriteLine(
		"\tSession ID: %lu",
		lpPEB->SessionId
	);

	AL::OS::Console::WriteLine(
		"\tImage Path: %S",
		lpPEB->ProcessParameters->ImagePathName.Buffer
	);

	AL::OS::Console::WriteLine(
		"\tParameters: %S",
		lpPEB->ProcessParameters->CommandLine.Buffer
	);

	AL::OS::Console::WriteLine(
		"\tDebugger Present: %s",
		AL::ToString(AL::OS::IsDebuggerPresent()).GetCString()
	);
#endif
}

// @throw AL::Exception
void main_execute_tests(AL::uint32& testCount, AL::uint32& testFailCount)
{
	#define main_execute_test(__function__) \
		_main_execute_test(#__function__, __function__)

	auto _main_execute_test = [&testCount, &testFailCount](const char* _name, void(*_lpEntryPoint)())
	{
		++testCount;

		AL::OS::Console::WriteLine(
			"Executing %s",
			_name
		);

		try
		{
			AL::OS::Timer timer;

			_lpEntryPoint();

			auto elapsed = timer.GetElapsed();

			AL::OS::Console::Write(
				"Completed in "
			);

			if (elapsed.ToMinutes() > 0)
			{
				AL::OS::Console::WriteLine(
					"%llu minute(s)",
					elapsed.ToMinutes()
				);
			}
			else if (elapsed.ToSeconds() > 0)
			{
				AL::OS::Console::WriteLine(
					"%llu second(s)",
					elapsed.ToSeconds()
				);
			}
			else if (elapsed.ToMilliseconds() > 0)
			{
				AL::OS::Console::WriteLine(
					"%llums",
					elapsed.ToMilliseconds()
				);
			}
			else if (elapsed.ToMicroseconds() > 0)
			{
				AL::OS::Console::WriteLine(
					"%lluus",
					elapsed.ToMicroseconds()
				);
			}
			else
			{
				AL::OS::Console::WriteLine(
					"%lluns",
					elapsed.ToNanoseconds()
				);
			}
		}
		catch (const AL::Exception& exception)
		{
			++testFailCount;

			AL::OS::Console::WriteLine(
				exception.GetMessage()
			);

			if (auto lpInnerException = exception.GetInnerException())
			{
				do
				{
					AL::OS::Console::WriteLine(
						lpInnerException->GetMessage()
					);
				} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
			}
		}

		AL::OS::Console::WriteLine();
	};

	// main_execute_test(AL_Collections_Array);
	// main_execute_test(AL_Collections_ArrayList);
	// main_execute_test(AL_Collections_Dictionary);
	// main_execute_test(AL_Collections_LinkedList);
	// main_execute_test(AL_Collections_MPSCQueue);
	// main_execute_test(AL_Collections_Queue);
	// main_execute_test(AL_Collections_CircularQueue);
	// main_execute_test(AL_Collections_String);
	// main_execute_test(AL_Collections_StringBuilder);

	// main_execute_test(AL_Function);

	// main_execute_test(AL_FileSystem_File);

	// main_execute_test(AL_Game_Engine_Window);

	// main_execute_test(AL_Game_FileSystem_DataFile);
	// main_execute_test(AL_Game_FileSystem_ConfigFile);

	// main_execute_test(AL_Game_Network_ClientServer);

	// main_execute_test(AL_Hardware_Drivers_ADS1115);
	// main_execute_test(AL_Hardware_Drivers_AT24C256);
	// main_execute_test(AL_Hardware_Drivers_MLX90640);

	// main_execute_test(AL_Lua543);

	// main_execute_test(AL_Network_Adapter);
	// main_execute_test(AL_Network_UdpSocket);

	// main_execute_test(AL_Network_HTTP_Request);

	// main_execute_test(AL_OS_Process);
	// main_execute_test(AL_OS_Thread);
	// main_execute_test(AL_OS_ThreadPool);
	// main_execute_test(AL_OS_Window);

	// main_execute_test(AL_Serialization_CSV);
	// main_execute_test(AL_Serialization_HTML);
	// main_execute_test(AL_Serialization_JSON);
	// main_execute_test(AL_Serialization_NMEA);
	// main_execute_test(AL_Serialization_XML);

	// main_execute_test(AL_SQLite3_Database);
}

int main(int argc, char* argv[])
{
	AL::OS::Console::SetTitle(
		"AbstractionLayer Tests"
	);

	main_display_build_information();
	main_display_system_information();
	main_display_thread_information();
	main_display_process_information();

	AL::OS::Console::WriteLine();

	AL::OS::Timer timer;
	AL::uint32    testCount     = 0;
	AL::uint32    testFailCount = 0;

	main_execute_tests(
		testCount,
		testFailCount
	);

	{
		auto elapsed = timer.GetElapsed();

		AL::OS::Console::Write(
			"%lu/%lu tests completed in ",
			testCount - testFailCount,
			testCount
		);

		if (elapsed.ToMinutes() > 0)
		{
			AL::OS::Console::WriteLine(
				"%llu minute(s)",
				elapsed.ToMinutes()
			);
		}
		else if (elapsed.ToSeconds() > 0)
		{
			AL::OS::Console::WriteLine(
				"%llu second(s)",
				elapsed.ToSeconds()
			);
		}
		else if (elapsed.ToMilliseconds() > 0)
		{
			AL::OS::Console::WriteLine(
				"%llums",
				elapsed.ToMilliseconds()
			);
		}
		else if (elapsed.ToMicroseconds() > 0)
		{
			AL::OS::Console::WriteLine(
				"%lluus",
				elapsed.ToMicroseconds()
			);
		}
		else
		{
			AL::OS::Console::WriteLine(
				"%lluns",
				elapsed.ToNanoseconds()
			);
		}
	}

// Windows debugging is often done through a spawned shell so this will keep results open
#if defined(AL_PLATFORM_WINDOWS)
	AL::OS::Console::WriteLine();
	AL::OS::Console::Write("Press any key to exit");
	AL::String::Char c; AL::OS::Console::Read(c);
#endif

	return 0;
}
