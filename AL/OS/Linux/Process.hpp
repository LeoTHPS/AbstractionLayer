#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "DLException.hpp"

#include "AL/OS/Timer.hpp"
#include "AL/OS/System.hpp"
#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/Path.hpp"
#include "AL/FileSystem/TextFile.hpp"
#include "AL/FileSystem/Directory.hpp"

#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

namespace AL::OS::Linux
{
	typedef ::pid_t                    ProcessId;
	typedef uint8                      ProcessExitCode;
	typedef Collections::Array<String> ProcessCommandLine;

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

		Process(ProcessId id, Bool isCurrentProcess)
			: isOpen(
				True
			),
			isCurrentProcess(
				isCurrentProcess
			),
			id(
				id
			)
		{
		}

	public:
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(Process& process, ProcessId id)
		{
			if (!FileSystem::Directory::Exists(String::Format("/proc/%s", ToString(id).GetCString())))
			{

				return False;
			}

			process = Process(
				id,
				(id == static_cast<ProcessId>(::getpid()))
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
			ProcessId id;

			if ((id = ::fork()) == -1)
			{

				throw SystemException(
					"fork"
				);
			}
			else if (id == 0)
			{
				if (::chdir(startInfo.WorkingDirectory.GetCString()) == -1)
				{

					exit(0);
				}

				auto args = [&startInfo]()
				{
					Collections::Array<char*> value(
						startInfo.CommandLine.GetSize() + 2
					);

					size_t i = 0;

					value[i++] = const_cast<char*>(startInfo.Path.GetCString());

					for (auto& v : startInfo.CommandLine)
						value[i++] = const_cast<char*>(v.GetCString());

					value[i] = nullptr;

					return value;
				}();

				if (::execv(startInfo.Path.GetCString(), &args[0]) == -1)
				{

					exit(0);
				}
			}

			process = Process(
				id,
				False
			);
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
			AL_ASSERT(
				IsOpen(),
				"Process not open"
			);

			if (::kill(id, 0) == -1)
			{
				auto errorCode = GetLastError();

				if (errorCode != ESRCH)
					throw SystemException("kill", errorCode);

				return False;
			}

			return True;
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
		}

		// @throw AL::Exception
		ProcessExitCode GetExitCode() const
		{
			AL_ASSERT(
				IsOpen(),
				"Process not open"
			);

			// TODO: implement
			throw NotImplementedException();
		}

		Void Close()
		{
			if (IsOpen())
			{

				isOpen = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False if time elapsed and process is still running
		Bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			Timer timer;

			while (timer.GetElapsed() <= maxWaitTime)
			{
				if (!IsRunning())
				{

					return True;
				}

				Sleep();
			}

			return False;
		}

		// @throw AL::Exception
		Void Terminate(int32 signal)
		{
			AL_ASSERT(
				IsOpen(),
				"Process not open"
			);

			if (::kill(GetId(), signal) == -1)
			{

				throw SystemException(
					"kill"
				);
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

			id = process.id;

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

	enum class ProcessMemoryPageTypes : uint32
	{
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryPageTypes);

	enum class ProcessMemoryPageStates : uint32
	{
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryPageStates);

	enum class ProcessMemoryAccessModes : uint8
	{
		Read,
		ReadWrite
	};

	enum class ProcessMemoryProtections : uint32
	{
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryProtections);

	enum class ProcessMemoryAllocationTypes : uint32
	{
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryAllocationTypes);

	struct ProcessMemoryInformation
	{
		Void*                             Base;
		size_t                            Size;
		BitMask<ProcessMemoryPageTypes>   PageTypes;
		BitMask<ProcessMemoryPageStates>  PageStates;
		BitMask<ProcessMemoryProtections> Protection;
	};

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(const ProcessMemoryInformation& information)> ProcessMemoryEnumPagesCallback;

	class ProcessMemory
	{
		Bool                     isOpen = False;

		ProcessMemoryAccessModes mode;
		int                      handle;
		Process*                 lpProcess;

		ProcessMemory(const ProcessMemory&) = delete;

		ProcessMemory(Process& process, int handle, ProcessMemoryAccessModes mode)
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
			int fdMemory;

			if ((fdMemory = ::open(String::Format("/proc/%lu/mem", process.GetId()).GetCString(), ((mode == ProcessMemoryAccessModes::Read) ? O_RDONLY : O_RDWR) | O_DIRECT | O_SYNC)) == -1)
			{
				auto errorCode = GetLastError();

				if (errorCode == ENOENT)
				{

					return False;
				}

				throw SystemException(
					"open",
					errorCode
				);
			}

			processMemory = ProcessMemory(
				process,
				fdMemory,
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
				::close(
					GetHandle()
				);

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void* Allocate(Void* address, size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtections protection)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		Void Release(Void* address)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void Read(Void* address, Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			if (::lseek(GetHandle(), reinterpret_cast<::off_t>(address), SEEK_SET) == -1)
			{

				throw SystemException(
					"lseek"
				);
			}

			ssize_t bytesRead;

			if ((bytesRead = ::read(GetHandle(), lpBuffer, size)) == -1)
			{

				throw SystemException(
					"read"
				);
			}

			if (bytesRead == 0)
			{

				throw Exception(
					"EOF"
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

			if (::lseek(GetHandle(), reinterpret_cast<::off_t>(address), SEEK_SET) == -1)
			{

				throw SystemException(
					"lseek"
				);
			}

			if (::write(GetHandle(), lpBuffer, size) == -1)
			{

				throw SystemException(
					"write"
				);
			}
		}

		// @throw AL::Exception
		ProcessMemoryInformation Query(Void* address)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		// @return previous ProcessMemoryProtections
		ProcessMemoryProtections SetProtection(Void* address, size_t size, ProcessMemoryProtections value)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void EnumeratePages(const ProcessMemoryEnumPagesCallback& callback)
		{
			// TODO: implement
			throw NotImplementedException();
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

	class ProcessLibrary
	{
		Bool     isLoaded = False;

		String   path;
		void*    handle;
		Process* lpProcess;

		ProcessLibrary(const ProcessLibrary&) = delete;

		ProcessLibrary(Process& process, void* handle, String&& path)
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
				void* handle;

				if ((handle = ::dlopen(path.GetString().GetCString(), RTLD_NOW | RTLD_GLOBAL)) == NULL)
				{

					throw Linux::DLException(
						"dlopen"
					);
				}

				library = ProcessLibrary(
					process,
					handle,
					String(path.GetString())
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
		// @return AL::False if library not loaded
		static Bool Open(ProcessLibrary& library, Process& process, const String& name)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		static Void OpenProcess(ProcessLibrary& library, Process& process)
		{
			// TODO: implement
			throw NotImplementedException();
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
			if (IsLoaded())
			{
				::dlclose(
					handle
				);
			}
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

			void* result;

			if ((result = ::dlsym(GetHandle(), name.GetCString())) == NULL)
			{

				throw Linux::DLException(
					"dlsym"
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
				// TODO: implement

				isLoaded = False;
			}
		}

		ProcessLibrary& operator = (ProcessLibrary&& processLibrary)
		{
			if (IsLoaded())
			{
				::dlclose(
					GetHandle()
				);
			}

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
	static Bool IsDebuggerPresent()
	{
		FileSystem::TextFile file(
			"/proc/self/status"
		);

		try
		{
			if (!file.Open(FileSystem::FileOpenModes::Read))
			{

				throw Exception(
					"File not found"
				);
			}
		}
		catch (Exception& exception)
		{

			throw Exception(
				Move(exception),
				"Error opening %s",
				file.GetPath().GetString().GetCString()
			);
		}

		// @return 0 if not found
		// @return -1 if end of file
		auto file_TryReadLineAndTracerPID = [&file](String& _line, ProcessId& _value)
		{
			try
			{
				if (!file.ReadLine(_line))
				{

					return -1;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading line"
				);
			}

			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^TracerPid:\\s*(\\d+)$", _line))
			{

				return 0;
			}

			_value = FromString<ProcessId>(
				matches[1]
			);

			return 1;
		};

		ProcessId tracerPID = 0;

		try
		{
			String line;
			int    value;

			while ((value = file_TryReadLineAndTracerPID(line, tracerPID)) <= 0)
			{
				if (value == -1)
				{
					file.Close();

					return False;
				}
			}
		}
		catch (Exception& exception)
		{
			file.Close();

			throw Exception(
				Move(exception),
				"Error reading %s",
				file.GetPath().GetString().GetCString()
			);
		}

		file.Close();

		return tracerPID != 0;
	}

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
