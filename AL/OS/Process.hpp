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
#elif defined(AL_PLATFORM_WINDOWS)
	typedef Windows::ProcessId                      ProcessId;
	typedef Windows::ProcessExitCode                ProcessExitCode;
	typedef Windows::ProcessStartInfo               ProcessStartInfo;
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

	enum class ProcessMemoryPageTypes : uint8
	{
		Image   = 0x01,
		Mapped  = 0x02,
		Private = 0x04
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryPageTypes);

	enum class ProcessMemoryPageStates : uint8
	{
		Free    = 0x01,
		Commit  = 0x02,
		Reserve = 0x04
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryPageStates);

	enum class ProcessMemoryAccessModes : uint8
	{
		Read,
		ReadWrite
	};

	enum class ProcessMemoryProtections : uint8
	{
		Read    = 0x01,
		Write   = 0x02,
		Execute = 0x04
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ProcessMemoryProtections);

	enum class ProcessMemoryAllocationTypes : uint8
	{
		Commit   = 0x01,
		Reserve  = 0x02,
		Physical = 0x04
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
		typedef Linux::ProcessMemory                  _ProcessMemory;
		typedef Linux::ProcessEnumCallback            _ProcessEnumCallback;
		typedef Linux::ProcessMemoryPageTypes         _ProcessMemoryPageTypes;
		typedef Linux::ProcessMemoryPageStates        _ProcessMemoryPageStates;
		typedef Linux::ProcessMemoryAccessModes       _ProcessMemoryAccessModes;
		typedef Linux::ProcessMemoryInformation       _ProcessMemoryInformation;
		typedef Linux::ProcessMemoryProtections       _ProcessMemoryProtections;
		typedef Linux::ProcessMemoryAllocationTypes   _ProcessMemoryAllocationTypes;
		typedef Linux::ProcessMemoryEnumPagesCallback _ProcessMemoryEnumPagesCallback;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef Windows::ProcessMemory                  _ProcessMemory;
		typedef Windows::ProcessEnumCallback            _ProcessEnumCallback;
		typedef Windows::ProcessMemoryPageTypes         _ProcessMemoryPageTypes;
		typedef Windows::ProcessMemoryPageStates        _ProcessMemoryPageStates;
		typedef Windows::ProcessMemoryAccessModes       _ProcessMemoryAccessModes;
		typedef Windows::ProcessMemoryInformation       _ProcessMemoryInformation;
		typedef Windows::ProcessMemoryProtections       _ProcessMemoryProtections;
		typedef Windows::ProcessMemoryAllocationTypes   _ProcessMemoryAllocationTypes;
		typedef Windows::ProcessMemoryEnumPagesCallback _ProcessMemoryEnumPagesCallback;
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
			if (!_ProcessMemory::Open(processMemory.processMemory, process.process, ProcessMemoryAccessModes_ToNative(mode)))
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
				ProcessMemoryProtections_ToNative(value)
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
				ProcessMemoryAllocationTypes_ToNative(type),
				ProcessMemoryProtections_ToNative(protection)
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

			_ProcessMemoryEnumPagesCallback _callback(
				[&callback](const _ProcessMemoryInformation& _information)
				{
					auto information = ProcessMemoryInformation_ToOS(
						_information
					);

					if (!callback(information))
					{

						return False;
					}

					return True;
				}
			);

			processMemory.EnumeratePages(
				_callback
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

	private:
		static ProcessMemoryPageTypes  ProcessMemoryPageTypes_ToOS(_ProcessMemoryPageTypes value)
		{
			BitMask<ProcessMemoryPageTypes> pageType;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			pageType.Set(ProcessMemoryPageTypes::Image, BitMask<_ProcessMemoryPageTypes>::IsSet(value, _ProcessMemoryPageTypes::Image));
			pageType.Set(ProcessMemoryPageTypes::Mapped, BitMask<_ProcessMemoryPageTypes>::IsSet(value, _ProcessMemoryPageTypes::Mapped));
			pageType.Set(ProcessMemoryPageTypes::Private, BitMask<_ProcessMemoryPageTypes>::IsSet(value, _ProcessMemoryPageTypes::Private));
#endif

			return pageType.Value;
		}
		static _ProcessMemoryPageTypes ProcessMemoryPageTypes_ToNative(ProcessMemoryPageTypes value)
		{
			BitMask<_ProcessMemoryPageTypes> pageType;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			pageType.Set(_ProcessMemoryPageTypes::Image,   BitMask<ProcessMemoryPageTypes>::IsSet(value, ProcessMemoryPageTypes::Image));
			pageType.Set(_ProcessMemoryPageTypes::Mapped,  BitMask<ProcessMemoryPageTypes>::IsSet(value, ProcessMemoryPageTypes::Mapped));
			pageType.Set(_ProcessMemoryPageTypes::Private, BitMask<ProcessMemoryPageTypes>::IsSet(value, ProcessMemoryPageTypes::Private));
#endif

			return pageType.Value;
		}

		static ProcessMemoryPageStates  ProcessMemoryPageStates_ToOS(_ProcessMemoryPageStates value)
		{
			BitMask<ProcessMemoryPageStates> pageState;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			pageState.Set(ProcessMemoryPageStates::Free,    BitMask<_ProcessMemoryPageStates>::IsSet(value, _ProcessMemoryPageStates::Free));
			pageState.Set(ProcessMemoryPageStates::Commit,  BitMask<_ProcessMemoryPageStates>::IsSet(value, _ProcessMemoryPageStates::Commit));
			pageState.Set(ProcessMemoryPageStates::Reserve, BitMask<_ProcessMemoryPageStates>::IsSet(value, _ProcessMemoryPageStates::Reserve));
#endif

			return pageState.Value;
		}
		static _ProcessMemoryPageStates ProcessMemoryPageStates_ToNative(ProcessMemoryPageStates value)
		{
			BitMask<_ProcessMemoryPageStates> pageState;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			pageState.Set(_ProcessMemoryPageStates::Free,    BitMask<ProcessMemoryPageStates>::IsSet(value, ProcessMemoryPageStates::Free));
			pageState.Set(_ProcessMemoryPageStates::Commit,  BitMask<ProcessMemoryPageStates>::IsSet(value, ProcessMemoryPageStates::Commit));
			pageState.Set(_ProcessMemoryPageStates::Reserve, BitMask<ProcessMemoryPageStates>::IsSet(value, ProcessMemoryPageStates::Reserve));
#endif

			return pageState.Value;
		}

		static ProcessMemoryAccessModes  ProcessMemoryAccessModes_ToOS(_ProcessMemoryAccessModes value)
		{
#if defined(AL_PLATFORM_LINUX)
			switch (value)
			{
				case _ProcessMemoryAccessModes::Read:
					return ProcessMemoryAccessModes::Read;

				case _ProcessMemoryAccessModes::ReadWrite:
					return ProcessMemoryAccessModes::ReadWrite;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			switch (value)
			{
				case _ProcessMemoryAccessModes::Read:
					return ProcessMemoryAccessModes::Read;

				case _ProcessMemoryAccessModes::ReadWrite:
					return ProcessMemoryAccessModes::ReadWrite;
			}
#endif

			return ProcessMemoryAccessModes(
				0
			);
		}
		static _ProcessMemoryAccessModes ProcessMemoryAccessModes_ToNative(ProcessMemoryAccessModes value)
		{
#if defined(AL_PLATFORM_LINUX)
			switch (value)
			{
				case ProcessMemoryAccessModes::Read:
					return _ProcessMemoryAccessModes::Read;

				case ProcessMemoryAccessModes::ReadWrite:
					return _ProcessMemoryAccessModes::ReadWrite;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			switch (value)
			{
				case ProcessMemoryAccessModes::Read:
					return _ProcessMemoryAccessModes::Read;

				case ProcessMemoryAccessModes::ReadWrite:
					return _ProcessMemoryAccessModes::ReadWrite;
			}
#endif

			return _ProcessMemoryAccessModes(
				0
			);
		}

		static ProcessMemoryInformation  ProcessMemoryInformation_ToOS(_ProcessMemoryInformation value)
		{
			ProcessMemoryInformation information;

#if defined(AL_PLATFORM_LINUX)
			information.Base             = value.Base;
			information.Size             = value.Size;
			information.PageTypes.Value  = ProcessMemoryPageTypes_ToOS(value.PageTypes.Value);
			information.PageStates.Value = ProcessMemoryPageStates_ToOS(value.PageStates.Value);
			information.Protection.Value = ProcessMemoryProtections_ToOS(value.Protection.Value);
#elif defined(AL_PLATFORM_WINDOWS)
			information.Base             = value.Base;
			information.Size             = value.Size;
			information.PageTypes.Value  = ProcessMemoryPageTypes_ToOS(value.PageTypes.Value);
			information.PageStates.Value = ProcessMemoryPageStates_ToOS(value.PageStates.Value);
			information.Protection.Value = ProcessMemoryProtections_ToOS(value.Protection.Value);
#endif

			return information;
		}
		static _ProcessMemoryInformation ProcessMemoryInformation_ToNative(ProcessMemoryInformation value)
		{
			_ProcessMemoryInformation information;

#if defined(AL_PLATFORM_LINUX)
			information.Base             = value.Base;
			information.Size             = value.Size;
			information.PageTypes.Value  = ProcessMemoryPageTypes_ToNative(value.PageTypes.Value);
			information.PageStates.Value = ProcessMemoryPageStates_ToNative(value.PageStates.Value);
			information.Protection.Value = ProcessMemoryProtections_ToNative(value.Protection.Value);
#elif defined(AL_PLATFORM_WINDOWS)
			information.Base             = value.Base;
			information.Size             = value.Size;
			information.PageTypes.Value  = ProcessMemoryPageTypes_ToNative(value.PageTypes.Value);
			information.PageStates.Value = ProcessMemoryPageStates_ToNative(value.PageStates.Value);
			information.Protection.Value = ProcessMemoryProtections_ToNative(value.Protection.Value);
#endif

			return information;
		}

		static ProcessMemoryProtections  ProcessMemoryProtections_ToOS(_ProcessMemoryProtections value)
		{
			BitMask<ProcessMemoryProtections> protection;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			protection.Set(ProcessMemoryProtections::Read,    BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Read));
			protection.Set(ProcessMemoryProtections::Write,   BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Read_Write) || BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Execute_Read_Write));
			protection.Set(ProcessMemoryProtections::Execute, BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Execute) || BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Execute_Read) || BitMask<_ProcessMemoryProtections>::IsSet(value, _ProcessMemoryProtections::Execute_Read_Write));
#endif

			return protection.Value;
		}
		static _ProcessMemoryProtections ProcessMemoryProtections_ToNative(ProcessMemoryProtections value)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			if (BitMask<ProcessMemoryProtections>::IsSet(value, ProcessMemoryProtections::Read))
			{
				if (BitMask<ProcessMemoryProtections>::IsSet(value, ProcessMemoryProtections::Write))
				{
					if (BitMask<ProcessMemoryProtections>::IsSet(value, ProcessMemoryProtections::Execute))
					{

						return _ProcessMemoryProtections::Execute_Read_Write;
					}

					return _ProcessMemoryProtections::Read_Write;
				}
				else if (BitMask<ProcessMemoryProtections>::IsSet(value, ProcessMemoryProtections::Execute))
				{

					return _ProcessMemoryProtections::Execute_Read;
				}

				return _ProcessMemoryProtections::Read;
			}

			return _ProcessMemoryProtections::NoAccess;
#endif

			return _ProcessMemoryProtections(
				0
			);
		}

		static ProcessMemoryAllocationTypes  ProcessMemoryAllocationTypes_ToOS(_ProcessMemoryAllocationTypes value)
		{
			BitMask<ProcessMemoryAllocationTypes> allocationType;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			allocationType.Set(ProcessMemoryAllocationTypes::Commit,   BitMask<_ProcessMemoryAllocationTypes>::IsSet(value, _ProcessMemoryAllocationTypes::Commit));
			allocationType.Set(ProcessMemoryAllocationTypes::Reserve,  BitMask<_ProcessMemoryAllocationTypes>::IsSet(value, _ProcessMemoryAllocationTypes::Reserve));
			allocationType.Set(ProcessMemoryAllocationTypes::Physical, BitMask<_ProcessMemoryAllocationTypes>::IsSet(value, _ProcessMemoryAllocationTypes::Physical));
#endif

			return allocationType.Value;
		}
		static _ProcessMemoryAllocationTypes ProcessMemoryAllocationTypes_ToNative(ProcessMemoryAllocationTypes value)
		{
			BitMask<_ProcessMemoryAllocationTypes> allocationType;

#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			allocationType.Set(_ProcessMemoryAllocationTypes::Commit,   BitMask<ProcessMemoryAllocationTypes>::IsSet(value, ProcessMemoryAllocationTypes::Commit));
			allocationType.Set(_ProcessMemoryAllocationTypes::Reserve,  BitMask<ProcessMemoryAllocationTypes>::IsSet(value, ProcessMemoryAllocationTypes::Reserve));
			allocationType.Set(_ProcessMemoryAllocationTypes::Physical, BitMask<ProcessMemoryAllocationTypes>::IsSet(value, ProcessMemoryAllocationTypes::Physical));
#endif

			return allocationType.Value;
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

	inline auto IsDebuggerPresent()
	{
#if defined(AL_PLATFORM_LINUX)
		try
		{
			return Linux::IsDebuggerPresent();
		}
		catch (const Exception&)
		{
		}

		return False;
#elif defined(AL_PLATFORM_WINDOWS)
		return Windows::IsDebuggerPresent();
#endif
	}

	inline auto SetDebuggerPresent(Bool value = True)
	{
#if defined(AL_PLATFORM_LINUX)
		// TODO: can this be implemented without a kernel patch?
#elif defined(AL_PLATFORM_WINDOWS)
		return Windows::SetDebuggerPresent(
			value
		);
#endif
	}
}
