#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/List.hpp"
#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <signal.h>
	#include <unistd.h>
#elif defined(AL_PLATFORM_WINDOWS)

#endif

namespace AL::OS
{
	typedef uint32 ProcessId;
	typedef uint32 ProcessExitCode;

#if defined(AL_X86) || defined(AL_ARM)
	typedef uint32 ProcessAddress;
#elif defined(AL_X86_64) || defined(AL_ARM64)
	typedef uint64 ProcessAddress;
#endif

	struct ProcessMemoryPatternEntry
	{
		uint8 Value    = 0x00;
		bool  Required = true;
	};

	class ProcessMemoryPattern
	{
		Collections::Array<ProcessMemoryPatternEntry> pattern;

	public:
		template<size_t SIZE>
		ProcessMemoryPattern(const uint8(&pattern)[SIZE])
			: pattern(
				SIZE
			)
		{
			ProcessMemoryPatternEntry* lpEntry;

			for (size_t i = 0, lpEntry = &pattern[i]; i < SIZE; ++i, ++lpEntry)
			{
				lpEntry->Value = pattern[i];
				lpEntry->Required = true;
			}
		}

		template<size_t SIZE>
		ProcessMemoryPattern(const ProcessMemoryPatternEntry(&pattern)[SIZE])
			: pattern(
				SIZE
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				this->pattern[i] = pattern[i];
			}
		}

		auto GetLength() const
		{
			return pattern.GetSize();
		}

		auto& operator [] (size_t index)
		{
			return pattern[index];
		}
		auto& operator [] (size_t index) const
		{
			return pattern[index];
		}
	};

	enum class ProcessInteropTypes
	{
		SysAPI,
		Injection
	};

#if defined(AL_PLATFORM_WINDOWS)
	enum class ProcessStartupFlags
	{
		None = 0,

		// Inherit parent input/output/error handles
		InheritHandles = STARTF_USESTDHANDLES,

		// The command line came from an untrusted source.
		UntrustedSource = STARTF_UNTRUSTEDSOURCE
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessStartupFlags);

	enum class ProcessMemoryAllocationTypes
	{
		// Allocates memory charges (from the overall size of memory and the paging files on disk)
		// for the specified reserved memory pages. The function also guarantees that when the
		// caller later initially accesses the memory, the contents will be zero. Actual physical
		// pages are not allocated unless/until the virtual addresses are actually accessed.
		// To reserve and commit pages in one step, use Commit | Reserve
		Commit = MEM_COMMIT,

		// Reserves a range of the process's virtual address space without allocating any actual
		// physical storage in memory or in the paging file on disk.
		// To reserve and commit pages in one step, use Commit | Reserve
		Reserve = MEM_RESERVE,

		// Indicates that data in the memory range is no longer of interest.
		// The pages should not be read from or written to the paging file.
		// However, the memory block will be used again later, so it should not be decommitted.
		// This value cannot be used with any other value.
		// Using this value does not guarantee that the range operated on with Reset will contain zeros.
		// If you want the range to contain zeros, decommit the memory and then recommit it.
		Reset = MEM_RESET,

		// MEM_RESET_UNDO should only be called on an address range to which Reset was successfully applied earlier.
		// It indicates that the data in the specified memory range specified by lpAddress and dwSize is of interest
		// to the caller and attempts to reverse the effects of Reset. If the function succeeds, that means all data
		// in the specified address range is intact. If the function fails, at least some of the data in the address
		// range has been replaced with zeroes.
		ResetUndo = MEM_RESET_UNDO,

		// Allocates memory using large page support.
		// - https://msdn.microsoft.com/en-us/library/windows/desktop/aa366720(v=vs.85).aspx
		// The size and alignment must be a multiple of the large - page minimum.
		// If you specify this value, you must also specify Reserve and Commit.
		LargePage = MEM_LARGE_PAGES,

		// Reserves an address range that can be used to map Address Windowing Extensions (AWE) pages.
		// - https://msdn.microsoft.com/en-us/library/windows/desktop/aa366527(v=vs.85).aspx
		// This value must be used with Reserve and no other values.
		Physical = MEM_PHYSICAL,

		// Allocates memory at the highest possible address.
		// This can be slower than regular allocations, especially when there are many allocations.
		TopDown = MEM_TOP_DOWN
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryAllocationTypes);

	enum class ProcessMemoryProtectionTypes
	{
		// Enables execute access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		Execute = PAGE_EXECUTE,

		// Enables execute or read-only access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		ExecuteRead = PAGE_EXECUTE_READ,

		// Enables execute, read-only, or read/write access to the committed region of pages.
		ExecuteReadWrite = PAGE_EXECUTE_READWRITE,

		// Disables all access to the committed region of pages.
		// An attempt to read from, write to, or execute the committed region results in an access violation.
		NoAccess = PAGE_NOACCESS,

		// Enables read-only access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		// If Data Execution Prevention is enabled, an attempt to execute code in the committed
		// region results in an access violation.
		ReadOnly = PAGE_READONLY,

		// Enables read-only or read/write access to the committed region of pages.
		// If Data Execution Prevention is enabled, attempting to execute code in the committed
		// region results in an access violation.
		ReadWrite = PAGE_READWRITE,

		// Pages in the region become guard pages. Any attempt to access a guard page causes the system
		// to raise a STATUS_GUARD_PAGE_VIOLATION exception and turn off the guard page status.
		// Guard pages thus act as a one-time access alarm.
		// When an access attempt leads the system to turn off guard page status, the underlying page protection takes over.
		// If a guard page exception occurs during a system service, the service typically returns a failure status indicator.
		// This value cannot be used with NoAccess.
		Guard = PAGE_GUARD,

		// Sets all pages to be non-cachable. Applications should not use this attribute except when explicitly required for a device.
		// Using the interlocked functions with memory that is mapped with SEC_NOCACHE can result in an EXCEPTION_ILLEGAL_INSTRUCTION exception.
		// The NoCache flag cannot be used with the Guard, NoAccess, or WriteCombine flags.
		NoCache = PAGE_NOCACHE,

		// Sets all pages to be write-combined. 
		// Applications should not use this attribute except when explicitly required for a device.
		// Using the interlocked functions with memory that is mapped as write - combined can result in an EXCEPTION_ILLEGAL_INSTRUCTION exception.
		// The WriteCombine flag cannot be specified with the NoAccess, Guard, and NoCache flags.
		WriteCombine = PAGE_WRITECOMBINE
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryProtectionTypes);
#endif

	// @return false to stop enumeration
	typedef Function<bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	// @return false to stop enumeration
	typedef Function<bool(ProcessAddress address, ProcessAddress size)> ProcessEnumMemoryRegionsCallback;

	class Process
	{
#if defined(AL_PLATFORM_WINDOWS)
		HANDLE hProcess = NULL;
#endif

		bool isOpen = false;
		bool isCurrentProcess = false;
		mutable bool isExitCodeCached = false;

		ProcessId id = 0;
		mutable ProcessExitCode exitCode = 0;

		ProcessInteropTypes interopType = ProcessInteropTypes::SysAPI;

		Process(const Process&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		static void GetCurrentProcess(Process& process, ProcessInteropTypes interopType)
		{
			process.Close();

#if defined(AL_PLATFORM_WINDOWS)
			process.hProcess = ::GetCurrentProcess();
#endif

			process.isOpen = true;
			process.isCurrentProcess = true;

			process.id = static_cast<ProcessId>(
#if defined(AL_PLATFORM_LINUX)
				::getpid()
#elif defined(AL_PLATFORM_WINDOWS)
				::GetCurrentProcessId()
#endif
			);

			process.interopType = interopType;
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		static bool GetProcessById(Process& process, ProcessId id, ProcessInteropTypes interopType);
		
		// @throw AL::Exceptions::Exception
		// @return false if not found
		static bool GetProcessByName(Process& process, const String& name, ProcessInteropTypes interopType);

#if defined(AL_PLATFORM_LINUX)
		// @throw AL::Exceptions::Exception
		static void CreateProcess(Process& process, ProcessInteropTypes interopType);
#elif defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		static void CreateProcess(Process& process, ProcessStartupFlags flags, ProcessInteropTypes interopType);
#endif

		// @throw AL::Exceptions::Exception
		static void EnumerateProcesses(const ProcessEnumCallback& callback);

		Process()
		{
		}

		Process(Process&& process)
			: isOpen(
				process.isOpen
			),
			isCurrentProcess(
				process.isCurrentProcess
			),
			isExitCodeCached(
				process.isExitCodeCached
			),
			id(
				process.id
			),
			exitCode(
				process.exitCode
			),
			interopType(
				process.interopType
			)
		{
#if defined(AL_PLATFORM_WINDOWS)
			hProcess = process.hProcess;
			process.hProcess = NULL;
#endif

			process.isOpen = false;
			process.isCurrentProcess = false;
			process.isExitCodeCached = false;

			process.id = 0;
			process.exitCode = 0;
			process.interopType = ProcessInteropTypes::SysAPI;
		}

		virtual ~Process()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		// @throw AL::Exceptions::Exception
		bool IsRunning() const
		{
			AL_ASSERT(IsOpen(), "Process not open");

#if defined(AL_PLATFORM_LINUX)
			switch (kill(static_cast<pid_t>(GetId()), 0))
			{
				case 0:
					return true;

				case -1:
				{
					if (errno != ESRCH)
					{

						throw Exceptions::SystemException(
							"kill"
						);
					}
				} break;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD exitCode;

			if (!GetExitCodeProcess(hProcess, &exitCode))
			{

				throw Exceptions::SystemException(
					"GetExitCodeProcess"
				);
			}

			if (exitCode == STILL_ACTIVE)
			{

				return true;
			}
#endif

			return false;
		}

		bool IsCurrentProcess() const
		{
			return isCurrentProcess;
		}

		ProcessId GetId() const
		{
			return id;
		}

		auto GetInteropType() const
		{
			return interopType;
		}

#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		ProcessExitCode GetExitCode() const
		{
			AL_ASSERT(IsOpen(), "Process not open");

			if (!isExitCodeCached)
			{
				DWORD _exitCode;

				if (!GetExitCodeProcess(hProcess, &_exitCode))
				{

					throw Exceptions::SystemException(
						"GetExitCodeProcess"
					);
				}

				exitCode = static_cast<ProcessExitCode>(
					_exitCode
					);

				isExitCodeCached = true;
			}

			return exitCode;
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool GetExport(ProcessAddress& address, uint16 ordinal)
		{
			return GetExport(
				address,
				"",
				ordinal
			);
		}
		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool GetExport(ProcessAddress& address, const String& module, uint16 ordinal)
		{
			return GetExport(
				address,
				module,
				Integer<uint16>::ToString(
					ordinal
				)
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool GetExport(ProcessAddress& address, const String& name)
		{
			return GetExport(
				address,
				"",
				name
			);
		}
		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool GetExport(ProcessAddress& address, const String& module, const String& name);

		// @throw AL::Exceptions::Exception
		void CreateThread(ProcessAddress address, ProcessAddress param = 0);

		// @throw AL::Exceptions::Exception
		// @return address of library
		ProcessAddress LoadLibrary(const String& path);
		// @throw AL::Exceptions::Exception
		// @return address of library
		ProcessAddress LoadLibrary(const void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		void UnloadLibrary(ProcessAddress address);

		// @throw AL::Exceptions::Exception
		void SetMemoryProtection(ProcessAddress address, size_t size, ProcessMemoryProtectionTypes value);

		// @throw AL::Exceptions::Exception
		// @return address of newly allocated memory
		ProcessAddress AllocateMemory(size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtectionTypes protection)
		{
			return AllocateMemory(
				0,
				size,
				type,
				protection
			);
		}
		// @throw AL::Exceptions::Exception
		// @return address of newly allocated memory
		ProcessAddress AllocateMemory(ProcessAddress address, size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtectionTypes protection);

		// @throw AL::Exceptions::Exception
		void ReleaseMemory(ProcessAddress address);
#endif

		// @throw AL::Exceptions::Exception
		template<typename T>
		void ReadMemory(ProcessAddress address, T& value) const
		{
			ReadMemory(
				address,
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exceptions::Exception
		void ReadMemory(ProcessAddress address, void* lpBuffer, size_t size) const
		{
			AL_ASSERT(IsOpen(), "Process not open");

#if defined(AL_PLATFORM_LINUX)
			// TODO: read /proc/{pid}/mem

			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			SIZE_T numberOfBytesRead;

			if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), lpBuffer, static_cast<SIZE_T>(size), &numberOfBytesRead))
			{

				throw Exceptions::SystemException(
					"ReadProcessMemory"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		template<typename T>
		void WriteMemory(ProcessAddress address, const T& value)
		{
			WriteMemory(
				address,
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteMemory(ProcessAddress address, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Process not open");

#if defined(AL_PLATFORM_LINUX)
			// TODO: write /proc/{pid}/mem

			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			SIZE_T numberOfBytesWritten;

			if (!WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), lpBuffer, static_cast<SIZE_T>(size), &numberOfBytesWritten))
			{

				throw Exceptions::SystemException(
					"WriteProcessMemory"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool SearchMemory(ProcessAddress& address, const ProcessMemoryPattern& pattern) const
		{
			bool found = false;

			ProcessEnumMemoryRegionsCallback callback(
				[this, &found, &address, &pattern](ProcessAddress _address, ProcessAddress _size)
				{
					if (SearchMemory(address, pattern, _address, _size))
					{
						found = true;
						
						return false;
					}

					return true;
				}
			);

			try
			{
				EnumerateMemoryRegions(
					callback
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error enumerating memory regions"
				);
			}

			return found;
		}
		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool SearchMemory(ProcessAddress& address, const ProcessMemoryPattern& pattern, ProcessAddress offset, ProcessAddress length) const
		{
			AL_ASSERT(IsOpen(), "Process not open");

			auto patternLength = pattern.GetLength();

			if (patternLength <= length)
			{
				Collections::Array<uint8> buffer(
					length
				);

				ReadMemory(
					offset,
					&buffer[0],
					length
				);

				for (size_t i = 0; i < length; ++i)
				{
					bool isMatch = true;

					for (size_t j = 0; j < patternLength; ++j)
					{
						if (pattern[j].Required && (pattern[j].Value != buffer[i + j]))
						{
							isMatch = false;

							break;
						}
					}

					if (isMatch)
					{
						address = i;

						return true;
					}
					
					if ((i + patternLength) > length)
					{

						break;
					}
				}
			}

			return false;
		}

		// @throw AL::Exceptions::Exception
		void EnumerateMemoryRegions(const ProcessEnumMemoryRegionsCallback& callback) const
		{
			AL_ASSERT(IsOpen(), "Process not open");

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			ProcessAddress address = System::GetMinimumAddress();
			ProcessAddress addressEnd = System::GetMaximumAddress();

			MEMORY_BASIC_INFORMATION mbi;

			do
			{
				if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0)
				{

					throw Exceptions::SystemException(
						"VirtualQuery"
					);
				}

				if (mbi.State & MEM_COMMIT)
				{
					if (!callback(reinterpret_cast<ProcessAddress>(mbi.BaseAddress), static_cast<ProcessAddress>(mbi.RegionSize)))
					{

						break;
					}
				}
			} while ((address = (reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize)) < addressEnd);
#endif
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_WINDOWS)
				if (!IsCurrentProcess())
				{
					CloseHandle(hProcess);
					hProcess = NULL;
				}
#endif

				isOpen = false;
				isCurrentProcess = false;
				isExitCodeCached = false;

				id = 0;
				exitCode = 0;
			}
		}

		// @throw AL::Exceptions::Exception
		void Terminate(ProcessExitCode exitCode)
		{
			AL_ASSERT(IsOpen(), "Process not open");

			if (IsRunning())
			{
#if defined(AL_PLATFORM_LINUX)
				if (kill(static_cast<pid_t>(GetId()), SIGTERM) == -1)
				{
					if (errno != ESRCH)
					{

						throw Exceptions::SystemException(
							"kill"
						);
					}
				}
#elif defined(AL_PLATFORM_WINDOWS)
				if (!TerminateProcess(hProcess, static_cast<UINT>(exitCode)))
				{

					throw Exceptions::SystemException(
						"TerminateProcess"
					);
				}
#endif
			}
		}
	};

#if defined(AL_PLATFORM_WINDOWS)
	AL_INLINE TEB* GetThreadEnvironmentBlock()
	{
		return reinterpret_cast<PTEB>(
#if defined(AL_X86)
			__readfsdword(
#elif defined(AL_X86_64)
			__readgsqword(
#endif
				reinterpret_cast<DWORD_PTR>(
					&static_cast<NT_TIB*>(nullptr)->Self
				)
			)
		);
	}

	AL_INLINE PEB* GetProcessEnvironmentBlock()
	{
		return GetThreadEnvironmentBlock()->ProcessEnvironmentBlock;
	}

	AL_INLINE bool IsDebuggerPresent()
	{
		return GetProcessEnvironmentBlock()->BeingDebugged != 0;
	}

	AL_INLINE void SetDebuggerPresent(bool set = true)
	{
		GetProcessEnvironmentBlock()->BeingDebugged = set ? 1 : 0;
	}
#endif
}
