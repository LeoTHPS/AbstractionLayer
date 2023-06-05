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
	class Process;
	class ProcessMemory;
	class ProcessThread;
	class ProcessLibrary;

	struct ProcessStartInfo
	{
		String Path;
		String CommandLine;
		String WorkingDirectory;
	};

	typedef uint32 ProcessId;
	typedef int32  ProcessExitCode;

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

			if ((isCurrentProcess = ((id == ::GetCurrentProcessId()) != 0)))
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

	inline Void SetDebuggerPresent(Bool set = True)
	{
		GetProcessEnvironmentBlock()->BeingDebugged = set ? 1 : 0;
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
