#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"
#include "AL/OS/Process.hpp"

static void AL_OS_Process()
{
	using namespace AL;
	using namespace AL::OS;

	ProcessEnumCallback enumProcessesCallback(
		[](ProcessId _processId, const String& _processName)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			Console::WriteLine(
				"Process [ID: %lu, Name: %s]",
				_processId,
				_processName.GetCString()
			);
#endif

			Process process;

			if (!Process::Open(process, _processId))
			{

				return True;
			}

			ProcessMemory processMemory;

			if (!ProcessMemory::Open(processMemory, process))
			{

				return True;
			}

			ProcessMemoryEnumRegionsCallback enumProcessMemoryRegionsCallback(
				[](const ProcessMemoryRegion& _processMemoryRegion)
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					Console::WriteLine(
						"Process Memory Region [Address: 0x%X, Size: %s]",
						_processMemoryRegion.Address,
						ToString(_processMemoryRegion.Size).GetCString()
					);
#endif

					return True;
				}
			);

			processMemory.EnumerateRegions(
				enumProcessMemoryRegionsCallback
			);

			return True;
		}
	);

	Process::Enumerate(
		enumProcessesCallback
	);
}
