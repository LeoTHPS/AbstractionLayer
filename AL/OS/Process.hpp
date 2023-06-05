#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Process.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Process.hpp"
#else
	#error Platform not supported
#endif

namespace AL::OS
{
	class Process;

#if defined(AL_PLATFORM_LINUX)
	typedef Linux::ProcessId             ProcessId;
	typedef Linux::ProcessExitCode       ProcessExitCode;
	typedef Linux::ProcessStartInfo      ProcessStartInfo;
	typedef Linux::ProcessEnumCallback   ProcessEnumCallback;
#elif defined(AL_PLATFORM_WINDOWS)
	typedef Windows::ProcessId           ProcessId;
	typedef Windows::ProcessExitCode     ProcessExitCode;
	typedef Windows::ProcessStartInfo    ProcessStartInfo;
	typedef Windows::ProcessEnumCallback ProcessEnumCallback;
#endif

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
	// @return AL::False to stop enumeration
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
		// @return AL::False on access denied
		static Bool Open(ProcessMemory& processMemory, Process& process)
		{
			if (!Open(processMemory, process, ProcessMemoryAccessFlags::All))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on access denied
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
#endif
		}

		// @throw AL::Exception
		// @return AL::False if not found
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
		// @return AL::False if not found
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
		// @return AL::False if file not found
		static Bool Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path);

		// @throw AL::Exception
		// @return AL::False if library not loaded
		static Bool Open(ProcessLibrary& library, Process& process, const String& name);

		// @throw AL::Exception
		static Void OpenProcess(ProcessLibrary& library, Process& process);

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

		Handle GetHandle() const
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
		// @return AL::False if export not found
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
		// @return AL::False if export not found
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
#endif

			return True;
		}

		Void Unload();

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
	// @return AL::False to stop enumeration
	typedef Function<Bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	class Process
	{
#if defined(AL_PLATFORM_LINUX)
		typedef Linux::Process   _Process;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef Windows::Process _Process;
#endif

		_Process                process;

	public:
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(Process& process, ProcessId id)
		{
			if (!_Process::Open(process.process, id))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(Process& process, const String& name)
		{
			if (!_Process::Open(process.process, name))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		static Void OpenCurrent(Process& process)
		{
			_Process::OpenCurrent(
				process.process
			);
		}

		// @throw AL::Exception
		static Void Create(Process& process, const ProcessStartInfo& startInfo)
		{
			_Process::Create(
				process.process,
				startInfo
			);
		}

		// @throw AL::Exception
		static Void Enumerate(const ProcessEnumCallback& callback)
		{
			_Process::Enumerate(
				callback
			);
		}

		Process()
		{
		}

		Process(Process&& process)
			: process(
				Move(process.process)
			)
		{
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
			return process.IsOpen();
		}

		// @throw AL::Exception
		Bool IsRunning() const
		{
			return process.IsRunning();
		}

		Bool IsCurrentProcess() const
		{
			return process.IsCurrentProcess();
		}

		auto GetId() const
		{
			return process.GetId();
		}

		auto GetHandle() const
		{
			return process.GetHandle();
		}

		// @throw AL::Exception
		ProcessExitCode GetExitCode() const
		{
			return process.GetExitCode();
		}

		Void Close()
		{
			if (IsOpen())
			{

				process.Close();
			}
		}

		Process& operator = (Process&& process)
		{
			this->process = Move(
				process.process
			);

			return *this;
		}

		Bool operator == (const Process& process) const
		{
			if (this->process != process.process)
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
#if defined(AL_PLATFORM_LINUX)
		return Linux::GetCurrentProcessId();
#elif defined(AL_PLATFORM_WINDOWS)
		return Windows::GetCurrentProcessId();
#endif
	}
}

// @throw AL::Exception
// @return AL::False on access denied
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
#endif

	return True;
}

// @throw AL::Exception
// @return AL::False if file not found
inline AL::Bool AL::OS::ProcessLibrary::Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path)
{
	if (!path.Exists())
	{

		return False;
	}

#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
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
		// TODO: implement
		throw NotImplementedException();
	}
#endif

	return True;
}

// @throw AL::Exception
// @return AL::False if library not loaded
inline AL::Bool AL::OS::ProcessLibrary::Open(ProcessLibrary& library, Process& process, const String& name)
{
#if defined(AL_PLATFORM_LINUX)
	// TODO: implement
	throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
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
#endif

	return True;
}

// @throw AL::Exception
inline AL::Void AL::OS::ProcessLibrary::OpenProcess(ProcessLibrary& library, Process& process)
{
#if defined(AL_PLATFORM_LINUX)
	// TODO: implement
	throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
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

		if ((pathLength = GetModuleFileNameA(hModule, &path[0], AL_MAX_PATH)) == 0)
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
#endif
}

inline AL::Void AL::OS::ProcessLibrary::Unload()
{
	if (IsLoaded())
	{
#if defined(AL_PLATFORM_LINUX)
		// TODO: implement
		throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
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
#endif

		isLoaded = False;
	}
}
