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
	typedef Linux::ProcessId                        ProcessId;
	typedef Linux::ProcessExitCode                  ProcessExitCode;
	typedef Linux::ProcessStartInfo                 ProcessStartInfo;
	typedef Linux::ProcessEnumCallback              ProcessEnumCallback;
	typedef Linux::ProcessMemoryPageTypes           ProcessMemoryPageTypes;
	typedef Linux::ProcessMemoryPageStates          ProcessMemoryPageStates;
	typedef Linux::ProcessMemoryAccessModes         ProcessMemoryAccessModes;
	typedef Linux::ProcessMemoryInformation         ProcessMemoryInformation;
	typedef Linux::ProcessMemoryProtections         ProcessMemoryProtections;
	typedef Linux::ProcessMemoryAllocationTypes     ProcessMemoryAllocationTypes;
	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Linux::ProcessMemoryEnumPagesCallback   ProcessMemoryEnumPagesCallback;
#elif defined(AL_PLATFORM_WINDOWS)
	typedef Windows::ProcessId                      ProcessId;
	typedef Windows::ProcessExitCode                ProcessExitCode;
	typedef Windows::ProcessStartInfo               ProcessStartInfo;
	typedef Windows::ProcessEnumCallback            ProcessEnumCallback;
	typedef Windows::ProcessMemoryPageTypes         ProcessMemoryPageTypes;
	typedef Windows::ProcessMemoryPageStates        ProcessMemoryPageStates;
	typedef Windows::ProcessMemoryAccessModes       ProcessMemoryAccessModes;
	typedef Windows::ProcessMemoryInformation       ProcessMemoryInformation;
	typedef Windows::ProcessMemoryProtections       ProcessMemoryProtections;
	typedef Windows::ProcessMemoryAllocationTypes   ProcessMemoryAllocationTypes;
	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Windows::ProcessMemoryEnumPagesCallback ProcessMemoryEnumPagesCallback;
#endif

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(ProcessId processId, const String& processName)> ProcessEnumCallback;

	class Process
	{
		friend class ProcessMemory;
		friend class ProcessLibrary;

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

	class ProcessMemory
	{
#if defined(AL_PLATFORM_LINUX)
		typedef Linux::ProcessMemory   _ProcessMemory;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef Windows::ProcessMemory _ProcessMemory;
#endif

		Process*       lpProcess;
		_ProcessMemory processMemory;

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
			if (!_ProcessMemory::Open(processMemory.processMemory, process.process))
			{

				return False;
			}

			processMemory.lpProcess = &process;

			return True;
		}

		ProcessMemory()
		{
		}

		ProcessMemory(ProcessMemory&& processMemory)
			: lpProcess(
				processMemory.lpProcess
			),
			processMemory(
				Move(processMemory.processMemory)
			)
		{
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
			return processMemory.IsOpen();
		}

		auto GetMode() const
		{
			return processMemory.GetMode();
		}

		auto GetHandle() const
		{
			return processMemory.GetHandle();
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

				processMemory.Close();
			}
		}

		// @throw AL::Exception
		Void SetProtection(Void* address, size_t size, ProcessMemoryProtections value)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			processMemory.SetProtection(
				address,
				size,
				value
			);
		}

		// @throw AL::Exception
		// @return address of newly allocated memory
		Void* Allocate(Void* address, size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtections protection)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			return processMemory.Allocate(
				address,
				size,
				type,
				protection
			);
		}

		// @throw AL::Exception
		Void Release(Void* address)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			processMemory.Release(
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

			processMemory.Read(
				address,
				lpBuffer,
				size
			);
		}

		// @throw AL::Exception
		Void Write(Void* address, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			processMemory.Write(
				address,
				lpBuffer,
				size
			);
		}

		// @throw AL::Exception
		// @return AL::False if not found
		Bool Search(Void*& address, const ProcessMemoryPattern& pattern)
		{
			Bool found = False;

			ProcessMemoryEnumPagesCallback callback(
				[this, &found, &address, &pattern](const ProcessMemoryInformation& _information)
				{
					if (Search(address, pattern, _information.Base, _information.Size))
					{
						found = True;

						return False;
					}

					return True;
				}
			);

			try
			{
				EnumeratePages(
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
		Bool Search(Void*& address, const ProcessMemoryPattern& pattern, Void* offset, size_t length)
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
						address = reinterpret_cast<Void*>(
							reinterpret_cast<size_t>(offset) + i
						);

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
		Void EnumeratePages(const ProcessMemoryEnumPagesCallback& callback)
		{
			AL_ASSERT(
				IsOpen(),
				"ProcessMemory not open"
			);

			processMemory.EnumeratePages(
				callback
			);
		}

		ProcessMemory& operator = (ProcessMemory&& processMemory)
		{
			if (IsOpen())
			{

				Close();
			}

			lpProcess = processMemory.lpProcess;

			this->processMemory = Move(
				processMemory.processMemory
			);

			return *this;
		}

		Bool operator == (const ProcessMemory& processMemory) const
		{
			if (this->processMemory != processMemory.processMemory)
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
#if defined(AL_PLATFORM_LINUX)
		typedef Linux::ProcessLibrary   _ProcessLibrary;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef Windows::ProcessLibrary _ProcessLibrary;
#endif

		Process*        lpProcess;
		_ProcessLibrary processLibrary;

	public:
		// @throw AL::Exception
		// @return AL::False if file not found
		static Bool Load(ProcessLibrary& library, Process& process, const FileSystem::Path& path)
		{
			if (!_ProcessLibrary::Load(library.processLibrary, process.process, path))
			{

				return False;
			}

			library.lpProcess = &process;

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if library not loaded
		static Bool Open(ProcessLibrary& library, Process& process, const String& name)
		{
			if (!_ProcessLibrary::Open(library.processLibrary, process.process, name))
			{

				return False;
			}

			library.lpProcess = &process;

			return True;
		}

		// @throw AL::Exception
		static Void OpenProcess(ProcessLibrary& library, Process& process)
		{
			_ProcessLibrary::OpenProcess(
				library.processLibrary,
				process.process
			);

			library.lpProcess = &process;
		}

		ProcessLibrary()
		{
		}

		ProcessLibrary(ProcessLibrary&& processLibrary)
			: lpProcess(
				processLibrary.lpProcess
			),
			processLibrary(
				Move(processLibrary.processLibrary)
			)
		{
		}

		virtual ~ProcessLibrary()
		{
		}

		Bool IsLoaded() const
		{
			return processLibrary.IsLoaded();
		}

		auto& GetPath() const
		{
			return processLibrary.GetPath();
		}

		auto GetHandle() const
		{
			return processLibrary.GetHandle();
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

			return processLibrary.GetExport<T>(
				lpValue,
				name
			);
		}

		Void Unload()
		{
			if (IsLoaded())
			{

				processLibrary.Unload();
			}
		}

		ProcessLibrary& operator = (ProcessLibrary&& processLibrary)
		{
			lpProcess = processLibrary.lpProcess;

			this->processLibrary = Move(
				processLibrary.processLibrary
			);

			return *this;
		}

		Bool operator == (const ProcessLibrary& processLibrary) const
		{
			if (this->processLibrary != processLibrary.processLibrary)
			{

				return False;
			}

			return True;
		}
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
#if defined(AL_PLATFORM_LINUX)
		return Linux::GetCurrentProcessId();
#elif defined(AL_PLATFORM_WINDOWS)
		return Windows::GetCurrentProcessId();
#endif
	}
}
