#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "DLException.hpp"

#include "AL/OS/System.hpp"
#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/Path.hpp"
#include "AL/FileSystem/Directory.hpp"

#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

namespace AL::OS::Linux
{
	class Process;
	class ProcessMemory;
	class ProcessLibrary;

	struct ProcessStartInfo
	{
		String Path;
		String CommandLine;
		String WorkingDirectory;
	};

	typedef int32 ProcessId;
	typedef uint8 ProcessExitCode;

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	class Process
	{
		
		Bool      isOpen           = False;
		Bool      isCurrentProcess = False;

		ProcessId id;
		int       handle;

		Process(ProcessId id, int handle, Bool isCurrentProcess)
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
			if (::kill(static_cast<::pid_t>(id), 0) == -1)
			{
				auto lastError = GetLastError();

				switch (lastError)
				{
					case ESRCH:
					case EPERM:
						return False;
				}

				throw SystemException(
					"kill",
					lastError
				);
			}

			process = Process(
				id,
				0,
				(id == static_cast<ProcessId>(::getpid()))
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
			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		static Void Enumerate(const ProcessEnumCallback& callback)
		{
			FileSystem::DirectoryEnumDirectoriesCallback _callback(
				[&callback](const FileSystem::Directory& _directory)
				{
					Regex::MatchCollection matches;

					if (Regex::Match(matches, "^(\\d+)$", _directory.GetPath().GetFileOrDirectoryName()))
					{
						auto processId = FromString<ProcessId>(
							matches[1]
						);

						auto processName = GetProcessName(
							processId
						);

						if (!callback(processId, processName))
						{

							return False;
						}
					}

					return True;
				}
			);

			FileSystem::Directory::EnumerateDirectories(
				"/proc/",
				_callback
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
			// TODO: implement
			throw NotImplementedException();
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
			// TODO: implement
			throw NotImplementedException();
		}

		Void Close()
		{
			if (IsOpen())
			{
//				::close(
//					GetHandle()
//				);

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

	private:
		// @throw AL::Exception
		static String GetProcessName(ProcessId processId)
		{
			int  cmdline_fd;
			auto cmdline_Path = String::Format(
				"/proc/%lu/cmdline",
				processId
			);

			String processName;

			try
			{
				if ((cmdline_fd = ::open(cmdline_Path.GetCString(), O_RDONLY)) == -1)
				{

					throw SystemException(
						"open"
					);
				}

				Collections::Array<char> cmdline_Buffer(
					AL_MAX_PATH
				);

				if (::read(cmdline_fd, &cmdline_Buffer[0], cmdline_Buffer.GetCapacity()) == -1)
				{
					::close(
						cmdline_fd
					);

					throw SystemException(
						"read"
					);
				}

				::close(
					cmdline_fd
				);

				// cmdline_Buffer contains null terminated strings
				// String::Assign stops at the first null
				processName.Assign(
					&cmdline_Buffer[0]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading '%s'",
					cmdline_Path.GetCString()
				);
			}

			return processName;
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
			::getpid()
		);
	}
}

// @throw AL::Exception
inline AL::Void AL::OS::Linux::Process::OpenCurrent(Process& process)
{
	if (!Open(process, GetCurrentProcessId()))
	{

		throw Exception(
			"The current process could not be found"
		);
	}
}
