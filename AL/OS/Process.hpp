#pragma once
#include "AL/Common.hpp"

#include "System.hpp"
#include "ErrorCode.hpp"
#include "SystemException.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/Path.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/DLException.hpp"

	#include "AL/FileSystem/Directory.hpp"

	#include <dlfcn.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <unistd.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <TlHelp32.h>
	#include <winternl.h>
#endif

namespace AL::OS
{
	class Process;

	typedef uint32 ProcessId;
	typedef uint32 ProcessExitCode;

	struct ProcessStartInfo
	{
		String Path;
		String CommandLine;
		String WorkingDirectory;
	};

#if defined(AL_X86) || defined(AL_ARM)
	typedef uint32 ProcessMemoryAddress;
#elif defined(AL_X86_64) || defined(AL_ARM64)
	typedef uint64 ProcessMemoryAddress;
#endif

	enum class ProcessMemoryAccessFlags
	{
		Read  = 0x1,
		Write = 0x2,

		All   = Read | Write
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryAccessFlags);

	struct ProcessMemoryRegion
	{
		size_t               Size;
		ProcessMemoryAddress Address;
	};

	struct ProcessMemoryPatternEntry
	{
		uint8 Value    = 0x00;
		Bool  Required = True;
	};

	class ProcessMemoryPattern
	{
		Collections::Array<ProcessMemoryPatternEntry> pattern;

	public:
		explicit ProcessMemoryPattern(size_t length)
			: pattern(
				length
			)
		{
		}

		template<size_t SIZE>
		ProcessMemoryPattern(const uint8(&pattern)[SIZE])
			: pattern(
				SIZE
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				this->pattern[i].Value = pattern[i];
				this->pattern[i].Required = True;
			}
		}

		template<size_t SIZE>
		ProcessMemoryPattern(const ProcessMemoryPatternEntry(&pattern)[SIZE])
			: pattern(
				pattern
			)
		{
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

	enum class ProcessMemoryProtectionTypes : uint32
	{
		None,

		Read,
		ReadWrite,

		Execute,
		ExecuteRead,
		ExecuteReadWrite
	};

	// @throw AL::Exception
	// @return False to stop enumeration
	typedef Function<Bool(const ProcessMemoryRegion& region)> ProcessMemoryEnumRegionsCallback;

	class ProcessMemory
	{
		Bool                              isOpen = False;

		BitMask<ProcessMemoryAccessFlags> flags;
		Process*                          lpProcess;

#if defined(AL_PLATFORM_LINUX)
		int                               fdMemory;

		ProcessMemory(Process& process, int fdMemory, ProcessMemoryAccessFlags flags)
			: isOpen(
				True
			),
			flags(
				flags
			),
			lpProcess(
				&process
			),
			fdMemory(
				fdMemory
			)
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		::HANDLE                          hMemory;

		ProcessMemory(Process& process, ::HANDLE hMemory, ProcessMemoryAccessFlags flags)
			: isOpen(
				True
			),
			flags(
				flags
			),
			lpProcess(
				&process
			),
			hMemory(
				hMemory
			)
		{
		}
#endif

		ProcessMemory(const ProcessMemory&) = delete;

	public:
		// @throw AL::Exception
		// @return False on access denied
		static Bool Open(ProcessMemory& processMemory, Process& process)
		{
			if (!Open(processMemory, process, ProcessMemoryAccessFlags::All))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return False on access denied
		static Bool Open(ProcessMemory& processMemory, Process& process, ProcessMemoryAccessFlags flags);

		ProcessMemory()
		{
		}

		ProcessMemory(ProcessMemory&& processMemory)
			: isOpen(
				processMemory.isOpen
			),
			flags(
				Move(processMemory.flags)
			),
			lpProcess(
				processMemory.lpProcess
			),
#if defined(AL_PLATFORM_LINUX)
			fdMemory(
				processMemory.fdMemory
			)
#elif defined(AL_PLATFORM_WINDOWS)
			hMemory(
				processMemory.hMemory
			)
#endif
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

		auto& GetFlags() const
		{
			return flags;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)
			return fdMemory;
#elif defined(AL_PLATFORM_WINDOWS)
			return hMemory;
#endif
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
#if defined(AL_PLATFORM_LINUX)
				::close(
					fdMemory
				);
#elif defined(AL_PLATFORM_WINDOWS)
				::CloseHandle(
					hMemory
				);
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void SetProtection(ProcessMemoryAddress address, size_t size, ProcessMemoryProtectionTypes protection)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			::DWORD newProtectionType;

			switch (protection)
			{
				case ProcessMemoryProtectionTypes::None:
					newProtectionType = PAGE_NOACCESS;
					break;

				case ProcessMemoryProtectionTypes::Read:
					newProtectionType = PAGE_READONLY;
					break;
				case ProcessMemoryProtectionTypes::ReadWrite:
					newProtectionType = PAGE_READWRITE;
					break;

				case ProcessMemoryProtectionTypes::Execute:
					newProtectionType = PAGE_EXECUTE;
					break;
				case ProcessMemoryProtectionTypes::ExecuteRead:
					newProtectionType = PAGE_EXECUTE_READ;
					break;
				case ProcessMemoryProtectionTypes::ExecuteReadWrite:
					newProtectionType = PAGE_EXECUTE_READWRITE;
					break;
			}

			::DWORD oldProtectionType;

			if (!::VirtualProtectEx(GetHandle(), reinterpret_cast<::LPVOID>(address), static_cast<::SIZE_T>(size), newProtectionType, &oldProtectionType))
			{

				throw SystemException(
					"VirtualProtectEx"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		// @return address of newly allocated memory
		ProcessMemoryAddress Allocate(ProcessMemoryAddress address, size_t size, ProcessMemoryProtectionTypes protection)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (auto lpAddress = ::VirtualAllocEx(GetHandle(), reinterpret_cast<::LPVOID>(address), static_cast<::SIZE_T>(size), MEM_COMMIT, static_cast<::DWORD>(protection)))
			{

				return reinterpret_cast<ProcessMemoryAddress>(
					lpAddress
				);
			}

			throw SystemException(
				"VirtualAllocEx"
			);
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		Void Release(ProcessMemoryAddress address, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (!::VirtualFreeEx(GetHandle(), reinterpret_cast<::LPVOID>(address), 0, MEM_RELEASE))
			{

				throw SystemException(
					"VirtualFreeEx"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		template<typename T>
		Void Read(ProcessMemoryAddress address, T& value) const
		{
			Read(
				address,
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exception
		Void Read(ProcessMemoryAddress address, Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			if (::lseek(GetHandle(), static_cast<::off_t>(address), SEEK_SET) == -1)
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
#elif defined(AL_PLATFORM_WINDOWS)
			::SIZE_T numberOfBytesRead;

			if (!::ReadProcessMemory(GetHandle(), reinterpret_cast<::LPCVOID>(address), lpBuffer, static_cast<::SIZE_T>(size), &numberOfBytesRead))
			{

				throw SystemException(
					"ReadProcessMemory"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		template<typename T>
		Void Write(ProcessMemoryAddress address, const T& value)
		{
			Write(
				address,
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exception
		Void Write(ProcessMemoryAddress address, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			if (::lseek(GetHandle(), static_cast<::off_t>(address), SEEK_SET) == -1)
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
#elif defined(AL_PLATFORM_WINDOWS)
			::SIZE_T numberOfBytesWritten;

			if (!::WriteProcessMemory(GetHandle(), reinterpret_cast<::LPVOID>(address), lpBuffer, static_cast<::SIZE_T>(size), &numberOfBytesWritten))
			{

				throw SystemException(
					"WriteProcessMemory"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		// @return False if not found
		Bool Search(ProcessMemoryAddress& address, const ProcessMemoryPattern& pattern) const
		{
			Bool found = False;

			ProcessMemoryEnumRegionsCallback callback(
				[this, &found, &address, &pattern](const ProcessMemoryRegion& _region)
				{
					if (Search(address, pattern, _region.Address, _region.Size))
					{
						found = True;

						return False;
					}

					return True;
				}
			);

			try
			{
				EnumerateRegions(
					callback
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error enumerating memory regions"
				);
			}

			return found;
		}
		// @throw AL::Exception
		// @return False if not found
		Bool Search(ProcessMemoryAddress& address, const ProcessMemoryPattern& pattern, ProcessMemoryAddress offset, ProcessMemoryAddress length) const
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			auto patternLength = pattern.GetLength();

			if (patternLength <= length)
			{
				Collections::Array<uint8> buffer(
					length
				);

				Read(
					offset,
					&buffer[0],
					length
				);

				for (size_t i = 0; i < length; ++i)
				{
					Bool isMatch = True;

					for (size_t j = 0; j < patternLength; ++j)
					{
						if (pattern[j].Required && (pattern[j].Value != buffer[i + j]))
						{
							isMatch = False;

							break;
						}
					}

					if (isMatch)
					{
						address = i;

						return True;
					}
					
					if ((i + patternLength) > length)
					{

						break;
					}
				}
			}

			return False;
		}

		// @throw AL::Exception
		Void EnumerateRegions(const ProcessMemoryEnumRegionsCallback& callback) const
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			ProcessMemoryAddress address    = System::GetMinimumAddress();
			ProcessMemoryAddress addressEnd = System::GetMaximumAddress();

			::MEMORY_BASIC_INFORMATION mbi;
			ProcessMemoryRegion        region;

			do
			{
				if (::VirtualQueryEx(GetHandle(), reinterpret_cast<::LPCVOID>(address), &mbi, sizeof(mbi)) == 0)
				{

					throw SystemException(
						"VirtualQuery"
					);
				}

				if (mbi.State & MEM_COMMIT)
				{
					region.Size    = static_cast<ProcessMemoryAddress>(mbi.RegionSize);
					region.Address = reinterpret_cast<ProcessMemoryAddress>(mbi.BaseAddress);

					if (!callback(region))
					{

						break;
					}
				}
			} while ((address = (reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize)) < addressEnd);
#else
			throw PlatformNotSupportedException();
#endif
		}

		ProcessMemory& operator = (ProcessMemory&& processMemory)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = processMemory.isOpen;
			processMemory.isOpen = False;

			flags = Move(
				processMemory.flags
			);

			lpProcess = processMemory.lpProcess;

#if defined(AL_PLATFORM_LINUX)
			fdMemory = processMemory.fdMemory;
#elif defined(AL_PLATFORM_WINDOWS)
			hMemory = processMemory.hMemory;
#endif

			return *this;
		}

		Bool operator == (const ProcessMemory& processMemory) const
		{
			if (IsOpen() != processMemory.IsOpen())
			{

				return False;
			}

			if (IsOpen())
			{
				if (lpProcess != processMemory.lpProcess)
				{

					return False;
				}

				if (GetFlags() != processMemory.GetFlags())
				{

					return False;
				}
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
#if defined(AL_PLATFORM_LINUX)
		typedef void*     Handle;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::HMODULE Handle;
#endif

		Bool     isLoaded = False;

		String   path;
		Handle   hLibrary;
		Process* lpProcess;

		ProcessLibrary(const ProcessLibrary&) = delete;

		ProcessLibrary(Process& process, Handle handle, String&& path)
			: isLoaded(
				True
			),
			path(
				Move(path)
			),
			hLibrary(
				handle
			),
			lpProcess(
				&process
			)
		{
		}

	public:
		// @throw AL::Exception
		// @return False if file not found
		static Bool Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path);

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
			hLibrary(
				processLibrary.hLibrary
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
#if defined(AL_PLATFORM_LINUX)
				::dlclose(
					hLibrary
				);
#endif
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
			return hLibrary;
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
		// @return False if export not found
		template<typename T>
		Bool GetExport(T*& lpValue, uint16 ordinal) const
		{
			auto name = ToString(
				ordinal
			);

			if (!GetExport(lpValue, name))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return False if export not found
		template<typename T>
		Bool GetExport(T*& lpValue, const String& name) const
		{
#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
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
#else
			throw NotImplementedException();
#endif

			return True;
		}

		ProcessLibrary& operator = (ProcessLibrary&& processLibrary)
		{
			if (IsLoaded())
			{
#if defined(AL_PLATFORM_LINUX)
				::dlclose(
					GetHandle()
				);
#endif
			}

			isLoaded = processLibrary.isLoaded;
			processLibrary.isLoaded = False;

			path      = Move(processLibrary.path);
			hLibrary  = processLibrary.hLibrary;
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
	// @return False to stop enumeration
	typedef Function<Bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	class Process
	{
		Bool                    isOpen           = False;
		Bool                    isCurrentProcess = False;

		ProcessId               id;

#if defined(AL_PLATFORM_LINUX)
		int                     fdProcess;

		Process(ProcessId id, int fdProcess, Bool isCurrentProcess)
			: isOpen(
				True
			),
			isCurrentProcess(
				isCurrentProcess
			),
			id(
				id
			),
			fdProcess(
				fdProcess
			)
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		::HANDLE                hProcess;

		Process(ProcessId id, ::HANDLE hProcess, Bool isCurrentProcess)
			: isOpen(
				True
			),
			isCurrentProcess(
				isCurrentProcess
			),
			id(
				id
			),
			hProcess(
				hProcess
			)
		{
		}
#endif

	public:
		// @throw AL::Exception
		// @return False if not found
		static Bool Open(Process& process, ProcessId id)
		{
#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
			::HANDLE hProcess;
			Bool     isCurrentProcess;

			if ((isCurrentProcess = (id == ::GetCurrentProcessId())) != 0)
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
#else
			throw PlatformNotSupportedException();
#endif

			return True;
		}
		// @throw AL::Exception
		// @return False if not found
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
		static Void Enumerate(const ProcessEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
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
					try
					{
						if (!callback(static_cast<ProcessId>(process.th32ProcessID), String(process.szExeFile)))
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
#else
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		static Void Create(Process& process, const ProcessStartInfo& info)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			::STARTUPINFOA startup = { 0 };
			startup.cb         = sizeof(::STARTUPINFOA);
			startup.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
			startup.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
			startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

			::PROCESS_INFORMATION _info = { 0 };

			if (!::CreateProcessA(info.Path.GetCString(), const_cast<::LPSTR>(info.CommandLine.GetCString()), nullptr, nullptr, FALSE, 0, nullptr, info.WorkingDirectory.GetCString(), &startup, &_info))
			{

				throw SystemException(
					"CreateProcessA"
				);
			}

			::CloseHandle(
				_info.hThread
			);

			process = Process(
				_info.dwProcessId,
				_info.hProcess,
				False
			);
#else
			throw PlatformNotSupportedException();
#endif
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
#if defined(AL_PLATFORM_LINUX)
			fdProcess(
				process.fdProcess
			)
#elif defined(AL_PLATFORM_WINDOWS)
			hProcess(
				process.hProcess
			)
#endif
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
#if defined(AL_PLATFORM_LINUX)
			
#elif defined(AL_PLATFORM_WINDOWS)
			return hProcess;
#endif
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
//				::close(
//					fdProcess
//				);
#elif defined(AL_PLATFORM_WINDOWS)
				::CloseHandle(
					hProcess
				);
#endif

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

			id = process.id;

#if defined(AL_PLATFORM_LINUX)
			fdProcess = process.fdProcess;
#elif defined(AL_PLATFORM_WINDOWS)
			hProcess = process.hProcess;
#endif

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
#if defined(AL_PLATFORM_LINUX)
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

				// cmdline_Buffer contains null terminated strings
				// String::Assign stops at the first null
				processName.Assign(
					&cmdline_Buffer[0]
				);

				::close(
					cmdline_fd
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
#endif
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
#if defined(AL_PLATFORM_LINUX)
		return static_cast<ProcessId>(
			::getpid()
		);
#elif defined(AL_PLATFORM_WINDOWS)
		return static_cast<ProcessId>(
			::GetCurrentProcessId()
		);
#else
		throw PlatformNotSupportedException();
#endif
	}

#if defined(AL_PLATFORM_WINDOWS)
	inline ::TEB* GetThreadEnvironmentBlock()
	{
		return reinterpret_cast<::PTEB>(
#if defined(AL_X86)
			::__readfsdword(
#elif defined(AL_X86_64)
			::__readgsqword(
#endif
				reinterpret_cast<::DWORD_PTR>(
					&static_cast<::NT_TIB*>(nullptr)->Self
				)
			)
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
#endif
}

// @throw AL::Exception
// @return False on access denied
inline AL::Bool AL::OS::ProcessMemory::Open(ProcessMemory& processMemory, Process& process, ProcessMemoryAccessFlags flags)
{
#if defined(AL_PLATFORM_LINUX)
	int fdMemory;

	if ((fdMemory = ::open(String::Format("/proc/%lu/mem", process.GetId()).GetCString(), O_RDWR | O_DIRECT | O_SYNC)) == -1)
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
		flags
	);
#elif defined(AL_PLATFORM_WINDOWS)
	::HANDLE hMemory;

	if (process.IsCurrentProcess())
	{

		hMemory = process.GetHandle();
	}
	else if (!(hMemory = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, static_cast<::DWORD>(process.GetId()))))
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
		flags
	);
#else
	throw PlatformNotSupportedException();
#endif

	return True;
}

// @throw AL::Exception
// @return False if file not found
inline AL::Bool AL::OS::ProcessLibrary::Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path)
{
	if (!path.Exists())
	{

		return False;
	}

#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
	::HMODULE handle;

	if ((handle = ::LoadLibraryA(path.GetString().GetCString())) == NULL)
	{

		throw SystemException(
			"LoadLibraryA"
		);
	}

	library = ProcessLibrary(
		process,
		handle,
		String(path.GetString())
	);
#else
	throw PlatformNotSupportedException();
#endif

	return True;
}

// @throw AL::Exception
inline AL::Void AL::OS::Process::OpenCurrent(Process& process)
{
	if (!Open(process, GetCurrentProcessId()))
	{

		throw Exception(
			"The current process could not be found"
		);
	}
}
