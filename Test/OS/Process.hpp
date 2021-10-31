#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>
#include <AL/OS/Process.hpp>

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

			try
			{
				if (!Process::Open(process, _processId))
				{

					return True;
				}

				ProcessMemory processMemory;

				if (!ProcessMemory::Open(processMemory, process))
				{

					return True;
				}

				uint64 processMemoryUsage           = 0;
				uint64 processMemoryAllocationCount = 0;

				ProcessMemoryEnumRegionsCallback enumProcessMemoryRegionsCallback(
					[&processMemoryUsage, &processMemoryAllocationCount](const ProcessMemoryRegion& _processMemoryRegion)
					{
						processMemoryUsage += _processMemoryRegion.Size;

						++processMemoryAllocationCount;

						return True;
					}
				);

				processMemory.EnumerateRegions(
					enumProcessMemoryRegionsCallback
				);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
				Console::WriteLine(
					"\tMemory Usage: %llu Bytes",
					processMemoryUsage
				);

				Console::WriteLine(
					"\tMemory Allocation Count: %llu",
					processMemoryAllocationCount
				);
#endif
			}
			catch (const Exception&)
			{
			}

			return True;
		}
	);

	Process::Enumerate(
		enumProcessesCallback
	);
}
