#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "Thread.hpp"
#include "VirtualMemory.hpp"

#include "AL/OS/System.hpp"
#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/Path.hpp"

#include <intrin.h>
#include <TlHelp32.h>
#include <winternl.h>

namespace AL::OS::Windows
{
	typedef uint32 ProcessId;
	typedef int32  ProcessExitCode;
	typedef String ProcessCommandLine;

	struct ProcessStartInfo
	{
		String             Path;
		ProcessCommandLine CommandLine;
		String             WorkingDirectory;
	};

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	class Process
	{
		Bool      isOpen           = False;
		Bool      isCurrentProcess = False;

		ProcessId id;
		::HANDLE  handle;

		Process(const Process&) = delete;

		Process(ProcessId id, ::HANDLE handle, Bool isCurrentProcess)
			: isOpen(
				True
			),
			isCurrentProcess(
				isCurrentProcess
			),
			id(
				id
			),
			handle(
				handle
			)
		{
		}

	public:
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(Process& process, ProcessId id)
		{
			::HANDLE hProcess;
			Bool     isCurrentProcess;

			if ((isCurrentProcess = (id == ::GetCurrentProcessId())) != False)
			{

				hProcess = ::GetCurrentProcess();
			}
			else if (!(hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, static_cast<::DWORD>(id))))
			{
				if (id && (GetLastError() == ERROR_INVALID_PARAMETER))
				{

					return False;
				}

				throw SystemException(
					"OpenProcess"
				);
			}

			process = Process(
				id,
				hProcess,
				isCurrentProcess
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(Process& process, const String& name)
		{
			ProcessId processId;
			Bool      isProcessFound = False;

			ProcessEnumCallback callback(
				[&name, &processId, &isProcessFound](ProcessId _processId, const String& _processName)
				{
					if (!name.Compare(_processName, True))
					{

						return True;
					}

					processId = _processId;
					isProcessFound = True;

					return False;
				}
			);

			try
			{
				Enumerate(
					callback
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error enumerating processes"
				);
			}

			if (!isProcessFound || !Open(process, processId))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		static Void OpenCurrent(Process& process);

		// @throw AL::Exception
		static Void Create(Process& process, const ProcessStartInfo& startInfo)
		{
			::STARTUPINFOA startup = { 0 };
			startup.cb         = sizeof(::STARTUPINFOA);
			startup.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
			startup.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
			startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

			::PROCESS_INFORMATION info = { 0 };

			if (!::CreateProcessA(startInfo.Path.GetCString(), const_cast<::LPSTR>(startInfo.CommandLine.GetCString()), nullptr, nullptr, FALSE, 0, nullptr, startInfo.WorkingDirectory.GetCString(), &startup, &info))
			{

				throw SystemException(
					"CreateProcessA"
				);
			}

			::CloseHandle(
				info.hThread
			);

			if (::WaitForInputIdle(info.hProcess, INFINITE) == WAIT_FAILED)
			{
				::CloseHandle(
					info.hProcess
				);

				throw SystemException(
					"WaitForInputIdle"
				);
			}

			process = Process(
				info.dwProcessId,
				info.hProcess,
				False
			);
		}

		// @throw AL::Exception
		static Void Enumerate(const ProcessEnumCallback& callback)
		{
			::HANDLE hSnapshot;

			if ((hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
			{

				throw SystemException(
					"CreateToolhelp32Snapshot"
				);
			}

			::PROCESSENTRY32 process = { 0 };
			process.dwSize = sizeof(::PROCESSENTRY32);

			if (::Process32First(hSnapshot, &process))
			{
				do
				{
					String fileName(
						process.szExeFile
					);

					try
					{
						if (!callback(static_cast<ProcessId>(process.th32ProcessID), fileName))
						{

							break;
						}
					}
					catch (Exception&)
					{
						::CloseHandle(
							hSnapshot
						);

						throw;
					}
				} while (::Process32Next(hSnapshot, &process));
			}
			else if (GetLastError() != ERROR_NO_MORE_FILES)
			{
				::CloseHandle(
					hSnapshot
				);

				throw SystemException(
					"Process32First"
				);
			}

			::CloseHandle(
				hSnapshot
			);
		}

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
			id(
				process.id
			),
			handle(
				process.handle
			)
		{
			process.isOpen = False;
		}

		virtual ~Process()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		// @throw AL::Exception
		Bool IsRunning() const
		{
			ProcessExitCode exitCode;

			try
			{
				exitCode = GetExitCode();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error getting exit code"
				);
			}

			return exitCode == STILL_ACTIVE;
		}

		Bool IsCurrentProcess() const
		{
			return isCurrentProcess;
		}

		auto GetId() const
		{
			return id;
		}

		auto GetHandle() const
		{
			return handle;
		}

		// @throw AL::Exception
		ProcessExitCode GetExitCode() const
		{
			::DWORD exitCode;

			if (!::GetExitCodeProcess(GetHandle(), &exitCode))
			{

				throw SystemException(
					"GetExitCodeProcess"
				);
			}

			return static_cast<ProcessExitCode>(
				exitCode
			);
		}

		Void Close()
		{
			if (IsOpen())
			{
				::CloseHandle(
					GetHandle()
				);

				isOpen = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False if time elapsed and process is still running
		Bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"Process not open"
			);

			switch (::WaitForSingleObject(GetHandle(), static_cast<::DWORD>(maxWaitTime.ToMilliseconds())))
			{
				case WAIT_FAILED:    throw SystemException("WaitForSingleObject");
				case WAIT_TIMEOUT:   return False;
				case WAIT_ABANDONED: throw Exception("Error calling 'WaitForSingleObject': WAIT_ABANDONED");
			}

			return True;
		}

		// @throw AL::Exception
		Void Terminate(ProcessExitCode exitCode)
		{
			AL_ASSERT(
				IsOpen(),
				"Process not open"
			);

			if (!::TerminateProcess(GetHandle(), static_cast<::UINT>(exitCode)))
			{

				throw SystemException(
					"TerminateProcess"
				);
			}

			Close();
		}

		Process& operator = (Process&& process)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = process.isOpen;
			process.isOpen = False;

			isCurrentProcess = process.isCurrentProcess;

			id     = process.id;
			handle = process.handle;

			return *this;
		}

		Bool operator == (const Process& process) const
		{
			if (IsOpen() != process.IsOpen())
			{

				return False;
			}

			if (IsOpen() && (GetId() != process.GetId()))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Process& process) const
		{
			if (operator==(process))
			{

				return False;
			}

			return True;
		}
	};

	enum class ProcessMemoryAccessModes : uint8
	{
		Read,
		ReadWrite
	};

	typedef VirtualMemoryPageTypes         ProcessMemoryPageTypes;
	typedef VirtualMemoryPageStates        ProcessMemoryPageStates;
	typedef VirtualMemoryInformation       ProcessMemoryInformation;
	typedef VirtualMemoryProtections       ProcessMemoryProtections;
	typedef VirtualMemoryAllocationTypes   ProcessMemoryAllocationTypes;
	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef VirtualMemoryEnumPagesCallback ProcessMemoryEnumPagesCallback;

	class ProcessMemory
	{
		Bool                     isOpen = False;

		ProcessMemoryAccessModes mode;
		::HANDLE                 handle;
		Process*                 lpProcess;

		ProcessMemory(const ProcessMemory&) = delete;

		ProcessMemory(Process& process, ::HANDLE handle, ProcessMemoryAccessModes mode)
			: isOpen(
				True
			),
			mode(
				mode
			),
			handle(
				handle
			),
			lpProcess(
				&process
			)
		{
		}

	public:
		// @throw AL::Exception
		// @return AL::False on access denied
		static Bool Open(ProcessMemory& processMemory, Process& process)
		{
			if (!Open(processMemory, process, ProcessMemoryAccessModes::ReadWrite))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on access denied
		static Bool Open(ProcessMemory& processMemory, Process& process, ProcessMemoryAccessModes mode)
		{
			::HANDLE         hMemory;
			BitMask<::DWORD> accessMode;

			accessMode.Add(PROCESS_VM_READ);
			accessMode.Add(PROCESS_VM_OPERATION);
			accessMode.Set(PROCESS_VM_WRITE, mode == ProcessMemoryAccessModes::ReadWrite);

			if (process.IsCurrentProcess())
			{

				hMemory = process.GetHandle();
			}
			else if (!(hMemory = ::OpenProcess(accessMode.Value, FALSE, static_cast<::DWORD>(process.GetId()))))
			{
				if (process.GetId() && (GetLastError() == ERROR_INVALID_PARAMETER))
				{

					return False;
				}

				throw SystemException(
					"OpenProcess"
				);
			}

			processMemory = ProcessMemory(
				process,
				hMemory,
				mode
			);

			return True;
		}

		ProcessMemory()
		{
		}

		ProcessMemory(ProcessMemory&& processMemory)
			: isOpen(
				processMemory.isOpen
			),
			mode(
				processMemory.mode
			),
			handle(
				processMemory.handle
			),
			lpProcess(
				processMemory.lpProcess
			)
		{
			processMemory.isOpen = False;
		}

		virtual ~ProcessMemory()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		auto GetMode() const
		{
			return mode;
		}

		auto GetHandle() const
		{
			return handle;
		}

		auto& GetProcess()
		{
			return *lpProcess;
		}
		auto& GetProcess() const
		{
			return *lpProcess;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::CloseHandle(
					GetHandle()
				);

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void* Allocate(Void* address, size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtections protection)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			return VirtualMemory::Allocate(
				GetProcess().GetHandle(),
				address,
				size,
				type,
				protection
			);
		}

		Void Release(Void* address)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			VirtualMemory::Release(
				GetProcess().GetHandle(),
				address
			);
		}

		// @throw AL::Exception
		Void Read(Void* address, Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			::SIZE_T numberOfBytesRead;

			if (!::ReadProcessMemory(GetHandle(), address, lpBuffer, static_cast<::SIZE_T>(size), &numberOfBytesRead))
			{

				throw SystemException(
					"ReadProcessMemory"
				);
			}
		}

		// @throw AL::Exception
		Void Write(Void* address, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			if (GetMode() != ProcessMemoryAccessModes::ReadWrite)
			{

				throw OperationNotSupportedException();
			}

			::SIZE_T numberOfBytesWritten;

			if (!::WriteProcessMemory(GetHandle(), address, lpBuffer, static_cast<::SIZE_T>(size), &numberOfBytesWritten))
			{

				throw SystemException(
					"WriteProcessMemory"
				);
			}
		}

		// @throw AL::Exception
		ProcessMemoryInformation Query(Void* address)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			return VirtualMemory::Query(
				GetProcess().GetHandle(),
				address
			);
		}

		// @throw AL::Exception
		// @return previous ProcessMemoryProtections
		ProcessMemoryProtections SetProtection(Void* address, size_t size, ProcessMemoryProtections value)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			return VirtualMemory::SetProtection(
				GetProcess().GetHandle(),
				address,
				size,
				value
			);
		}

		// @throw AL::Exception
		Void EnumeratePages(const ProcessMemoryEnumPagesCallback& callback)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			VirtualMemory::EnumeratePages(
				GetProcess().GetHandle(),
				callback
			);
		}

		ProcessMemory& operator = (ProcessMemory&& processMemory)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = processMemory.isOpen;
			processMemory.isOpen = False;

			mode      = processMemory.mode;
			handle    = processMemory.handle;
			lpProcess = processMemory.lpProcess;

			return *this;
		}

		Bool operator == (const ProcessMemory& processMemory) const
		{
			if (IsOpen() != processMemory.IsOpen())
			{

				return False;
			}

			if (GetHandle() != processMemory.GetHandle())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ProcessMemory& processMemory) const
		{
			if (operator==(processMemory))
			{

				return False;
			}

			return True;
		}
	};

	class ProcessThread
	{
		Bool     isOpen = False;

		::HANDLE handle;
		Process* lpProcess;

		ProcessThread(const ProcessThread&) = delete;

		ProcessThread(Process& process, ::HANDLE handle)
			: isOpen(True),
			handle(handle),
			lpProcess(&process)
		{
		}

	public:
		// @throw AL::Exception
		static Void Start(ProcessThread& thread, Process& process, Void* address, Void* param = nullptr)
		{
			::HANDLE hThread;

			if ((hThread = ::CreateRemoteThread(process.GetHandle(), nullptr, 0, reinterpret_cast<::LPTHREAD_START_ROUTINE>(address), param, 0, nullptr)) == NULL)
			{

				throw SystemException(
					"CreateRemoteThread"
				);
			}

			thread = ProcessThread(
				process,
				hThread
			);
		}

		ProcessThread()
		{
		}

		ProcessThread(ProcessThread&& processThread)
			: isOpen(processThread.isOpen),
			handle(processThread.handle),
			lpProcess(processThread.lpProcess)
		{
			processThread.isOpen = False;
		}

		virtual ~ProcessThread()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		auto GetHandle() const
		{
			return handle;
		}

		// @throw AL::Exception
		Bool IsRunning() const
		{
			::DWORD exitCode;

			if (!::GetExitCodeThread(GetHandle(), &exitCode))
			{

				throw SystemException(
					"GetExitCodeThread"
				);
			}

			return exitCode == STILL_ACTIVE;
		}

		// @throw AL::Exception
		Bool GetExitCode(uint32& value) const
		{
			::DWORD exitCode;

			if (!::GetExitCodeThread(GetHandle(), &exitCode))
			{

				throw SystemException(
					"GetExitCodeThread"
				);
			}

			if (exitCode == STILL_ACTIVE)
			{

				return False;
			}

			value = static_cast<uint32>(
				exitCode
			);

			return True;
		}

		auto& GetProcess()
		{
			return *lpProcess;
		}
		auto& GetProcess() const
		{
			return *lpProcess;
		}

		// @throw AL::Exception
		// @return AL::False if time elapsed and ProcessThread is still running
		Bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessThread not open"
			);

			switch (::WaitForSingleObject(GetHandle(), static_cast<::DWORD>(maxWaitTime.ToMilliseconds())))
			{
				case WAIT_FAILED:    throw SystemException("WaitForSingleObject");
				case WAIT_TIMEOUT:   return False;
				case WAIT_ABANDONED: throw Exception("Error calling 'WaitForSingleObject': WAIT_ABANDONED");
			}

			return True;
		}

		// @throw AL::Exception
		Void Terminate(uint32 exitCode)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessThread not open"
			);

			if (::TerminateThread(GetHandle(), static_cast<::DWORD>(exitCode)) == 0)
			{

				throw SystemException(
					"TerminateThread"
				);
			}

			Close();
		}

		Void Close()
		{
			if (IsOpen())
			{
				::CloseHandle(
					GetHandle()
				);

				isOpen = False;
			}
		}

		ProcessThread& operator = (ProcessThread&& processThread)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = processThread.isOpen;
			processThread.isOpen = False;

			handle    = processThread.handle;
			lpProcess = processThread.lpProcess;

			return *this;
		}

		Bool operator == (const ProcessThread& processThread) const
		{
			if (IsOpen() != processThread.IsOpen())
			{

				return False;
			}

			if (GetHandle() != processThread.GetHandle())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ProcessThread& processThread) const
		{
			if (operator==(processThread))
			{

				return False;
			}

			return True;
		}
	};

	class ProcessLibrary
	{
		Bool      isLoaded = False;

		String    path;
		::HMODULE handle;
		Process*  lpProcess;

		ProcessLibrary(const ProcessLibrary&) = delete;

		ProcessLibrary(Process& process, ::HMODULE handle, String&& path)
			: isLoaded(
				True
			),
			path(
				Move(path)
			),
			handle(
				handle
			),
			lpProcess(
				&process
			)
		{
		}

	public:
		// @throw AL::Exception
		// @return AL::False if file not found
		static Bool Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path)
		{
			if (!path.Exists())
			{

				return False;
			}

			if (process.IsCurrentProcess())
			{
				::HMODULE hModule;

				if ((hModule = ::LoadLibraryA(path.GetString().GetCString())) == NULL)
				{

					throw SystemException(
						"LoadLibraryA"
					);
				}

				library = ProcessLibrary(
					process,
					hModule,
					String(path.GetString())
				);
			}
			else
			{
				ProcessMemory memory;

				if (!ProcessMemory::Open(memory, process, ProcessMemoryAccessModes::ReadWrite))
					throw Exception("Access denied");

				auto lpPath = memory.Allocate(
					nullptr,
					path.GetString().GetSize(),
					ProcessMemoryAllocationTypes::Commit | ProcessMemoryAllocationTypes::Reserve,
					ProcessMemoryProtections::Read_Write
				);

#if defined(AL_ARCH_X86)
				Void* lpCodeCave;
				uint8 codeCaveBuffer[] =
				{
					/* 0  */ 0x55,                         // push ebp
					/* 1  */ 0x89, 0xE5,                   // mov ebp, esp
					/* 3  */ 0xFF, 0x75, 0x08,             // push DWORD PTR [ebp+8]
					/* 6  */ 0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, LoadLibraryA
					/* 11 */ 0xFF, 0xD0,                   // call eax
					/* 13 */ 0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, GetLastError
					/* 18 */ 0xFF, 0xD0,                   // call eax
					/* 20 */ 0x5D,                         // pop ebp
					/* 21 */ 0xC2, 0x04, 0x00              // ret 4
				};

				*reinterpret_cast<size_t*>(&codeCaveBuffer[7])  = reinterpret_cast<size_t>(&::LoadLibraryA);
				*reinterpret_cast<size_t*>(&codeCaveBuffer[14]) = reinterpret_cast<size_t>(&::GetLastError);
#endif

				try
				{
					memory.Write(
						lpPath,
						path.GetString().GetCString(),
						path.GetString().GetSize()
					);

#if defined(AL_ARCH_X86)
					lpCodeCave = memory.Allocate(
						nullptr,
						sizeof(codeCaveBuffer),
						ProcessMemoryAllocationTypes::Commit | ProcessMemoryAllocationTypes::Reserve,
						ProcessMemoryProtections::Execute_Read_Write
					);
#endif
				}
				catch (Exception&)
				{
					memory.Release(lpPath);

					throw;
				}

				ProcessThread thread;

				try
				{
#if defined(AL_ARCH_X86)
					memory.Write(
						lpCodeCave,
						&codeCaveBuffer[0],
						sizeof(codeCaveBuffer)
					);
#endif

					ProcessThread::Start(
						thread,
						process,
#if defined(AL_ARCH_X86)
						lpCodeCave,
#else
						reinterpret_cast<Void*>(::GetProcAddress(::GetModuleHandleA("kernel32.dll"), "LoadLibraryA")),
#endif
						lpPath
					);

					thread.Join();
				}
				catch (Exception&)
				{
					memory.Release(lpPath);
#if defined(AL_ARCH_X86)
					memory.Release(lpCodeCave);
#endif

					throw;
				}

				memory.Release(lpPath);
#if defined(AL_ARCH_X86)
				memory.Release(lpCodeCave);
#endif

#if defined(AL_ARCH_X86)
				if (uint32 threadExitCode; thread.GetExitCode(threadExitCode) && threadExitCode)
#else
				if (uint32 threadExitCode; thread.GetExitCode(threadExitCode) && !threadExitCode)
#endif
				{

					throw SystemException(
						"LoadLibraryA",
						threadExitCode
					);
				}
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if library not loaded
		static Bool Open(ProcessLibrary& library, Process& process, const String& name)
		{
			if (process.IsCurrentProcess())
			{
				::HMODULE hModule;

				if ((hModule = ::GetModuleHandleA(name.GetCString())) == NULL)
				{
					auto errorCode = GetLastError();

					if (errorCode == ERROR_MOD_NOT_FOUND)
					{

						return False;
					}

					throw SystemException(
						"GetModuleHandleA",
						errorCode
					);
				}

				library = ProcessLibrary(
					process,
					hModule,
					String(name)
				);
			}
			else
			{
				// TODO: implement
				throw NotImplementedException();
			}

			return True;
		}

		// @throw AL::Exception
		static Void OpenProcess(ProcessLibrary& library, Process& process)
		{
			if (process.IsCurrentProcess())
			{
				::HMODULE hModule;

				if ((hModule = ::GetModuleHandleA(NULL)) == NULL)
				{

					throw SystemException(
						"GetModuleHandleA"
					);
				}

				char    path[AL_MAX_PATH] = { 0  };
				::DWORD pathLength;

				if ((pathLength = ::GetModuleFileNameA(hModule, &path[0], AL_MAX_PATH)) == 0)
				{

					throw SystemException(
						"GetModuleFileName"
					);
				}

				library = ProcessLibrary(
					process,
					hModule,
					String(
						path,
						pathLength
					)
				);
			}
			else
			{
				// TODO: implement
				throw NotImplementedException();
			}
		}

		ProcessLibrary()
		{
		}

		ProcessLibrary(ProcessLibrary&& processLibrary)
			: isLoaded(
				processLibrary.isLoaded
			),
			path(
				Move(processLibrary.path)
			),
			handle(
				processLibrary.handle
			),
			lpProcess(
				processLibrary.lpProcess
			)
		{
			processLibrary.isLoaded = False;
		}

		virtual ~ProcessLibrary()
		{
		}

		Bool IsLoaded() const
		{
			return isLoaded;
		}

		auto& GetPath() const
		{
			return path;
		}

		auto GetHandle() const
		{
			return handle;
		}

		auto& GetProcess()
		{
			return *lpProcess;
		}
		auto& GetProcess() const
		{
			return *lpProcess;
		}

		// @throw AL::Exception
		// @return AL::False if export not found
		template<typename T>
		Bool GetExport(T*& lpValue, const String& name) const
		{
			AL_ASSERT(
				IsLoaded(),
				"ProcessLibrary not loaded"
			);

			::FARPROC result;

			if ((result = ::GetProcAddress(GetHandle(), name.GetCString())) == NULL)
			{

				throw SystemException(
					"GetProcAddress"
				);
			}

			lpValue = reinterpret_cast<T*>(
				result
			);

			return True;
		}

		Void Unload()
		{
			if (IsLoaded())
			{
				if (GetProcess().IsCurrentProcess())
				{
					::FreeLibrary(
						GetHandle()
					);
				}
				else
				{
					// TODO: implement
					throw NotImplementedException();
				}

				isLoaded = False;
			}
		}

		ProcessLibrary& operator = (ProcessLibrary&& processLibrary)
		{
			isLoaded = processLibrary.isLoaded;
			processLibrary.isLoaded = False;

			path      = Move(processLibrary.path);
			handle    = processLibrary.handle;
			lpProcess = processLibrary.lpProcess;

			return *this;
		}

		Bool operator == (const ProcessLibrary& processLibrary) const;
		Bool operator != (const ProcessLibrary& processLibrary) const
		{
			if (operator==(processLibrary))
			{

				return False;
			}

			return True;
		}
	};

	// @throw AL::Exception
	inline auto GetCurrentProcess()
	{
		Process process;
		Process::OpenCurrent(process);

		return process;
	}

	inline auto GetCurrentProcessId()
	{
		return static_cast<ProcessId>(
			::GetCurrentProcessId()
		);
	}

	inline ::PEB* GetProcessEnvironmentBlock()
	{
		return GetThreadEnvironmentBlock()->ProcessEnvironmentBlock;
	}

	inline Bool IsDebuggerPresent()
	{
		return GetProcessEnvironmentBlock()->BeingDebugged != 0;
	}

	inline Void SetDebuggerPresent(Bool value = True)
	{
		GetProcessEnvironmentBlock()->BeingDebugged = value ? 1 : 0;
	}
}

// @throw AL::Exception
inline AL::Void AL::OS::Windows::Process::OpenCurrent(Process& process)
{
	if (!Open(process, GetCurrentProcessId()))
	{

		throw Exception(
			"The current process could not be found"
		);
	}
}
