#include "AL/Common.hpp"

#include "AL/OS/Timer.hpp"
#include "AL/OS/Console.hpp"

#include "Collections/Array.hpp"
#include "Collections/ArrayList.hpp"
#include "Collections/Dictionary.hpp"
#include "Collections/LinkedList.hpp"
#include "Collections/MPSCQueue.hpp"
#include "Collections/Queue.hpp"
#include "Collections/String.hpp"

#include "Common/Function.hpp"

#include "FileSystem/File.hpp"

#include "Game/Network/Client.hpp"

#include "Game/Storage/DataFile.hpp"

#include "Lua543/Lua.hpp"

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
	} \
	catch (const AL::Exception& exception) \
	{ \
		AL::OS::Console::WriteLine( \
			exception.GetMessage().GetCString() \
		); \
		\
		if (auto lpInnerException = exception.GetInnerException()) \
		{ \
			do \
			{ \
				AL::OS::Console::WriteLine( \
					lpInnerException->GetMessage().GetCString() \
				); \
			} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr); \
		} \
	}

int main(int argc, char* argv[])
{
	AL::OS::Console::SetTitle(
		"AbstractionLayer Tests"
	);

	AL_TEST_EXECUTE(AL_Collections_Array);
	AL_TEST_EXECUTE(AL_Collections_ArrayList);
	AL_TEST_EXECUTE(AL_Collections_Dictionary);
	AL_TEST_EXECUTE(AL_Collections_LinkedList);
	AL_TEST_EXECUTE(AL_Collections_MPSCQueue);
	AL_TEST_EXECUTE(AL_Collections_Queue);
	AL_TEST_EXECUTE(AL_Collections_String);

	AL_TEST_EXECUTE(AL_Function);

	AL_TEST_EXECUTE(AL_FileSystem_File);

	AL_TEST_EXECUTE(AL_Game_Network_Client);

	AL_TEST_EXECUTE(AL_Game_Storage_DataFile);

	AL_TEST_EXECUTE(AL_Lua543);

	AL_TEST_EXECUTE(AL_OS_Thread);
	AL_TEST_EXECUTE(AL_OS_ThreadPool);
	AL_TEST_EXECUTE(AL_OS_Window);

	return 0;
}
