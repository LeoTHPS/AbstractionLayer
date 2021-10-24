#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Thread.hpp>
#include <AL/OS/Process.hpp>

#include "Collections/Array.hpp"
#include "Collections/ArrayList.hpp"
#include "Collections/Dictionary.hpp"
#include "Collections/LinkedList.hpp"
#include "Collections/MPSCQueue.hpp"
#include "Collections/Queue.hpp"
#include "Collections/String.hpp"
#include "Collections/StringBuilder.hpp"

#include "Common/Function.hpp"

#include "FileSystem/File.hpp"

#include "HTML/Document.hpp"

#include "Lua543/Lua.hpp"

#include "OS/Process.hpp"
#include "OS/Thread.hpp"
#include "OS/ThreadPool.hpp"
#include "OS/Window.hpp"

#define AL_TEST_EXECUTE(__function__, ...) \
	AL::OS::Console::WriteLine( \
		"Executing " #__function__ \
	); \
	\
	try \
	{ \
		AL::OS::Timer timer; \
		\
		__function__( \
			__VA_ARGS__ \
		); \
		\
		AL::OS::Console::WriteLine( \
			"Completed in %lluus", \
			timer.GetElapsed().ToMicroseconds() \
		); \
		\
		AL::OS::Console::WriteLine(); \
	} \
	catch (const AL::Exception& exception) \
	{ \
		AL::OS::Console::WriteLine( \
			exception.GetMessage() \
		); \
		\
		if (auto lpInnerException = exception.GetInnerException()) \
		{ \
			do \
			{ \
				AL::OS::Console::WriteLine( \
					lpInnerException->GetMessage() \
				); \
			} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr); \
		} \
		\
		AL::OS::Console::WriteLine(); \
	}

void main_display_build_information()
{
	AL::OS::Console::WriteLine(
		"Compiler: %s",
#if defined(AL_COMPILER_GNU)
		"GNU"
#elif defined(AL_COMPILER_MSVC)
		"MSVC"
#elif defined(AL_COMPILER_CLANG)
		"CLANG"
#else
		"Undefined"
#endif
	);

	AL::OS::Console::WriteLine(
		"Architecture: %s",
#if defined(AL_ARCH_X86)
		"x86"
#elif defined(AL_ARCH_X86_64)
		"x86_64"
#elif defined(AL_ARCH_ARM)
		"ARM"
#elif defined(AL_ARCH_ARM64)
		"ARM64"
#else
		"Undefined"
#endif
	);
}

void main_display_system_information()
{
AL::OS::Console::WriteLine(
		"OS: %s",
#if defined(AL_PLATFORM_LINUX)
		"Linux"
#elif defined(AL_PLATFORM_WINDOWS)
		"Windows"
#else
		"Undefined"
#endif
	);

	AL::OS::Console::WriteLine(
		"Timezone: %i",
		AL::OS::System::GetTimezone()
	);

	AL::OS::Console::WriteLine(
		"Timestamp: %llu",
		AL::OS::System::GetTimestamp().ToSeconds()
	);

	AL::OS::Console::WriteLine(
		"CPU Count: %llu",
		AL::OS::System::GetProcessorCount()
	);

	AL::OS::Console::WriteLine(
		"Page Size: %llu",
		AL::OS::System::GetPageSize()
	);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif
}

void main_display_thread_information()
{
	AL::OS::Console::WriteLine(
		"Thread ID: %lu",
		AL::OS::GetCurrentThreadId()
	);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	auto lpTEB = AL::OS::GetThreadEnvironmentBlock();


#endif
}

void main_display_process_information()
{
	AL::OS::Console::WriteLine(
		"Process ID: %lu",
		AL::OS::GetCurrentProcessId()
	);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	auto lpPEB = AL::OS::GetProcessEnvironmentBlock();

	AL::OS::Console::WriteLine(
		"Session ID: %lu",
		lpPEB->SessionId
	);

	AL::OS::Console::WriteLine(
		"Image Path: %S",
		lpPEB->ProcessParameters->ImagePathName.Buffer
	);

	AL::OS::Console::WriteLine(
		"Parameters: %S",
		lpPEB->ProcessParameters->CommandLine.Buffer
	);

	AL::OS::Console::WriteLine(
		"Debugger Present: %s",
		AL::ToString(AL::OS::IsDebuggerPresent()).GetCString()
	);
#endif
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

	AL_TEST_EXECUTE(AL_Collections_Array);
	AL_TEST_EXECUTE(AL_Collections_ArrayList);
	AL_TEST_EXECUTE(AL_Collections_Dictionary);
	AL_TEST_EXECUTE(AL_Collections_LinkedList);
	AL_TEST_EXECUTE(AL_Collections_MPSCQueue);
	AL_TEST_EXECUTE(AL_Collections_Queue);
	AL_TEST_EXECUTE(AL_Collections_String);
	AL_TEST_EXECUTE(AL_Collections_StringBuilder);

	AL_TEST_EXECUTE(AL_Function);

	AL_TEST_EXECUTE(AL_FileSystem_File);

	AL_TEST_EXECUTE(AL_HTML_Document);

	AL_TEST_EXECUTE(AL_Lua543);

	AL_TEST_EXECUTE(AL_OS_Process);
	AL_TEST_EXECUTE(AL_OS_Thread);
	AL_TEST_EXECUTE(AL_OS_ThreadPool);
	AL_TEST_EXECUTE(AL_OS_Window);

	return 0;
}
