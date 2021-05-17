#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	typedef AL::OS::ProcessId       ProcessId;
	typedef AL::OS::ProcessExitCode ProcessExitCode;

	typedef AL::OS::ProcessAddress  ProcessAddress;
	
	public ref class ProcessMemoryPatternEntry
	{
		uint8 value    = 0x00;
		bool  required = true;

	public:
		property uint8 Value
		{
		public:
			uint8 get()
			{
				return value;
			}
			void set(uint8 value)
			{
				this->value = value;
			}
		}

		property bool  Required
		{
		public:
			bool get()
			{
				return required;
			}
			void set(bool value)
			{
				required = value;
			}
		}
	};

	public ref class ProcessMemoryPattern
	{
		array<ProcessMemoryPatternEntry^>^ pattern;

	public:
		ProcessMemoryPattern(array<uint8>^ pattern)
			: pattern(
				gcnew array<ProcessMemoryPatternEntry^>(
					pattern->Length
				)
			)
		{
			for (System::Int32 i = 0; i < pattern->Length; ++i)
			{
				this->pattern[i]->Value = pattern[i];
				this->pattern[i]->Required = true;
			}
		}

		ProcessMemoryPattern(array<ProcessMemoryPatternEntry^>^ pattern)
			: pattern(
				pattern
			)
		{
		}

		explicit ProcessMemoryPattern(System::Int32 length)
			: ProcessMemoryPattern(
				gcnew array<ProcessMemoryPatternEntry^>(
					length
				)
			)
		{
		}

		auto GetLength()
		{
			return pattern->Length;
		}

		property ProcessMemoryPatternEntry^ default[System::UInt32]
		{
		public:
			ProcessMemoryPatternEntry^ get(System::UInt32 index)
			{
				return pattern[index];
			}
			void set(System::UInt32 index, ProcessMemoryPatternEntry^ value)
			{
				pattern[index] = value;
			}
		}
	};

	[System::FlagsAttribute]
	public enum class ProcessStartupFlags
		: typename Get_Enum_Base<AL::OS::ProcessStartupFlags>::Type
	{
		None            = static_cast<typename Get_Enum_Base<AL::OS::ProcessStartupFlags>::Type>(AL::OS::ProcessStartupFlags::None),

#if defined(AL_PLATFORM_WINDOWS)
		InheritHandles  = static_cast<typename Get_Enum_Base<AL::OS::ProcessStartupFlags>::Type>(AL::OS::ProcessStartupFlags::InheritHandles),
		UntrustedSource = static_cast<typename Get_Enum_Base<AL::OS::ProcessStartupFlags>::Type>(AL::OS::ProcessStartupFlags::UntrustedSource)
#elif defined(AL_PLATFORM_LINUX)

#endif
	};
	
	public enum class ProcessMemoryReleaseTypes
		: typename Get_Enum_Base<AL::OS::ProcessMemoryReleaseTypes>::Type
	{
#if defined(AL_PLATFORM_WINDOWS)
		Decommit = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryReleaseTypes>::Type>(AL::OS::ProcessMemoryReleaseTypes::Decommit),
		Release  = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryReleaseTypes>::Type>(AL::OS::ProcessMemoryReleaseTypes::Release)
#elif defined(AL_PLATFORM_LINUX)

#endif
	};

	[System::FlagsAttribute]
	public enum class ProcessMemoryAllocationTypes
		: typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type
	{
#if defined(AL_PLATFORM_WINDOWS)
		Commit    = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::Commit),
		Reserve   = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::Reserve),
		Reset     = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::Reset),
		ResetUndo = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::ResetUndo),
		LargePage = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::LargePage),
		Physical  = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::Physical),
		TopDown   = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryAllocationTypes>::Type>(AL::OS::ProcessMemoryAllocationTypes::TopDown)
#elif defined(AL_PLATFORM_LINUX)

#endif
	};
	
	[System::FlagsAttribute]
	public enum class ProcessMemoryProtectionTypes
		: typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type
	{
#if defined(AL_PLATFORM_WINDOWS)
		Execute          = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::Execute),
		ExecuteRead      = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::ExecuteRead),
		ExecuteReadWrite = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::ExecuteReadWrite),
		NoAccess         = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::NoAccess),
		ReadOnly         = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::ReadOnly),
		ReadWrite        = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::ReadWrite),
		Guard            = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::Guard),
		NoCache          = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::NoCache),
		WriteCombine     = static_cast<typename Get_Enum_Base<AL::OS::ProcessMemoryProtectionTypes>::Type>(AL::OS::ProcessMemoryProtectionTypes::WriteCombine)
#elif defined(AL_PLATFORM_LINUX)

#endif
	};

	public ref class ProcessStartInfo
	{
		System::String^     path             = System::String::Empty;
		System::String^     commandLine      = System::String::Empty;
		System::String^     workingDirectory = System::String::Empty;

#if defined(AL_PLATFORM_WINDOWS)
		ProcessStartupFlags flags            = ProcessStartupFlags::None;
#endif

	public:
		property System::String^ Path
		{
			System::String^ get()
			{
				return path;
			}
			void set(System::String^ value)
			{
				path = value;
			}
		}
		
		property System::String^ CommandLine
		{
			System::String^ get()
			{
				return commandLine;
			}
			void set(System::String^ value)
			{
				commandLine = value;
			}
		}

		property System::String^ WorkingDirectory
		{
			System::String^ get()
			{
				return workingDirectory;
			}
			void set(System::String^ value)
			{
				workingDirectory = value;
			}
		}

#if defined(AL_PLATFORM_WINDOWS)
		property ProcessStartupFlags Flags
		{
			ProcessStartupFlags get()
			{
				return flags;
			}
			void set(ProcessStartupFlags value)
			{
				flags = value;
			}
		}
#endif
	};

	/// <summary>
	/// Return false to stop enumeration
	/// </summary>
	/// <exception cref="AL::Exceptions::Exception" />
	public delegate bool ProcessEnumCallback(ProcessId processId, System::String^ processName);

	class ProcessEnumNativeCallback
	{
		typedef bool(NativeCallback)(ProcessId, const char*);

		NativeCallback* const lpCallback;

	public:
		explicit ProcessEnumNativeCallback(ProcessEnumCallback^ callback)
			: lpCallback(
				reinterpret_cast<NativeCallback*>(
					System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(callback).ToPointer()
				)
			)
		{
		}

		virtual ~ProcessEnumNativeCallback()
		{
		}

		bool Execute(AL::OS::ProcessId processId, const String& processName) const
		{
			return lpCallback(
				static_cast<ProcessId>(processId),
				processName.GetCString()
			);
		}
	};

	/// <summary>
	/// Return false to stop enumeration
	/// </summary>
	/// <exception cref="AL::Exceptions::Exception" />
	public delegate bool ProcessEnumMemoryRegionsCallback(ProcessAddress address, ProcessAddress size);

	class ProcessEnumMemoryRegionsNativeCallback
	{
		typedef bool(NativeCallback)(ProcessAddress, ProcessAddress);

		NativeCallback* const lpCallback;

	public:
		explicit ProcessEnumMemoryRegionsNativeCallback(ProcessEnumMemoryRegionsCallback^ callback)
			: lpCallback(
				reinterpret_cast<NativeCallback*>(
					System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(callback).ToPointer()
				)
			)
		{
		}

		virtual ~ProcessEnumMemoryRegionsNativeCallback()
		{
		}

		bool Execute(AL::OS::ProcessAddress address, AL::OS::ProcessAddress size) const
		{
			return lpCallback(
				static_cast<ProcessAddress>(address),
				static_cast<ProcessAddress>(size)
			);
		}
	};

	public ref class Process
	{
		AL::OS::Process* const lpProcess;

		explicit Process(AL::OS::Process&& process)
			: lpProcess(
				new AL::OS::Process(
					Move(process)
				)
			)
		{
		}

	public:
		/// <exception cref="AL::Exceptions::Exception" />
		static void GetCurrentProcess([System::Runtime::InteropServices::OutAttribute] Process^% process)
		{
			AL::OS::Process _process;

			try
			{
				AL::OS::Process::GetCurrentProcess(
					_process
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			process = gcnew Process(
				Move(_process)
			);
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		static bool GetProcessById([System::Runtime::InteropServices::OutAttribute] Process^% process, ProcessId id)
		{
			AL::OS::Process _process;

			try
			{
				if (!AL::OS::Process::GetProcessById(_process, static_cast<AL::OS::ProcessId>(id)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			process = gcnew Process(
				Move(_process)
			);

			return true;
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		static bool GetProcessByName([System::Runtime::InteropServices::OutAttribute] Process^% process, System::String^ name)
		{
			AL::OS::Process _process;

			try
			{
				if (!AL::OS::Process::GetProcessByName(_process, Marshal::ToNativeString(name)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			process = gcnew Process(
				Move(_process)
			);

			return true;
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static void CreateProcess([System::Runtime::InteropServices::OutAttribute] Process^% process, ProcessStartInfo^ info)
		{
			AL::OS::Process _process;

			AL::OS::ProcessStartInfo _info;
			_info.Path = Marshal::ToNativeString(info->Path);
			_info.CommandLine = Marshal::ToNativeString(info->CommandLine);
			_info.WorkingDirectory = Marshal::ToNativeString(info->WorkingDirectory);
			_info.Flags = static_cast<AL::OS::ProcessStartupFlags>(info->Flags);

			try
			{
				AL::OS::Process::CreateProcess(
					_process,
					_info
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			process = gcnew Process(
				Move(_process)
			);
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static void EnumerateProcesses(ProcessEnumCallback^ callback)
		{
			System::GC::KeepAlive(
				callback
			);

			ProcessEnumNativeCallback nativeCallback(
				callback
			);

			AL::OS::Process::EnumerateProcesses(
				AL::OS::ProcessEnumCallback(
					&ProcessEnumNativeCallback::Execute,
					nativeCallback
				)
			);
		}

		Process()
			: Process(
				AL::OS::Process()
			)
		{
		}

		virtual ~Process()
		{
			if (lpProcess != nullptr)
			{
				delete lpProcess;
			}
		}

		bool IsOpen()
		{
			return lpProcess->IsOpen();
		}

		/// <exception cref="AL::Exceptions::Exception" />
		bool IsRunning()
		{
			try
			{
				return lpProcess->IsRunning();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		bool IsCurrentProcess()
		{
			return lpProcess->IsCurrentProcess();
		}

		auto GetId()
		{
			return static_cast<ProcessId>(
				lpProcess->GetId()
			);
		}

		/// <exception cref="AL::Exceptions::Exception" />
		auto GetExitCode()
		{
			try
			{
				return static_cast<ProcessExitCode>(
					lpProcess->GetExitCode()
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool GetExport([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, System::UInt16 ordinal)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				if (!lpProcess->GetExport(_address, static_cast<uint16>(ordinal)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}
		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool GetExport([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, System::String^ module, System::UInt16 ordinal)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				if (!lpProcess->GetExport(_address, Marshal::ToNativeString(module), static_cast<uint16>(ordinal)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool GetExport([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, System::String^ name)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				if (!lpProcess->GetExport(_address, Marshal::ToNativeString(name)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}
		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool GetExport([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, System::String^ module, System::String^ name)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				if (!lpProcess->GetExport(_address, Marshal::ToNativeString(module), Marshal::ToNativeString(name)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool GetModule([System::Runtime::InteropServices::OutAttribute] ProcessAddress& address, System::String^ name)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				if (!lpProcess->GetModule(_address, Marshal::ToNativeString(name)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void CreateThread(ProcessAddress address)
		{
			CreateThread(
				address,
				0
			);
		}
		/// <exception cref="AL::Exceptions::Exception" />
		void CreateThread(ProcessAddress address, ProcessAddress param)
		{
			try
			{
				lpProcess->CreateThread(
					static_cast<AL::OS::ProcessAddress>(address),
					static_cast<AL::OS::ProcessAddress>(param)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::Exceptions::Exception" />
		System::UInt32 CreateThreadAndWait(ProcessAddress address)
		{
			return CreateThreadAndWait(
				address,
				0
			);
		}
		/// <exception cref="AL::Exceptions::Exception" />
		System::UInt32 CreateThreadAndWait(ProcessAddress address, ProcessAddress param)
		{
			try
			{
				return lpProcess->CreateThreadAndWait(
					static_cast<AL::OS::ProcessAddress>(address),
					static_cast<AL::OS::ProcessAddress>(param)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <summary>
		/// Returns address of library
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		ProcessAddress LoadLibrary(System::String^ path)
		{
			AL::OS::ProcessAddress address;

			try
			{
				address = lpProcess->LoadLibrary(
					Marshal::ToNativeString(path)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			return static_cast<ProcessAddress>(
				address
			);
		}
		/// <summary>
		/// Returns address of library
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		ProcessAddress LoadLibrary(array<System::Byte>^ buffer)
		{
			auto lpBuffer = new uint8[
				static_cast<size_t>(buffer->Length)
			];

			Marshal::Copy(
				buffer,
				lpBuffer,
				0,
				static_cast<System::UInt32>(buffer->Length)
			);

			AL::OS::ProcessAddress address;

			try
			{
				address = lpProcess->LoadLibrary(
					lpBuffer,
					static_cast<size_t>(buffer->Length)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				delete[] lpBuffer;

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			delete[] lpBuffer;

			return static_cast<ProcessAddress>(
				address
			);
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void UnloadLibrary(ProcessAddress address)
		{
			try
			{
				lpProcess->UnloadLibrary(
					static_cast<AL::OS::ProcessAddress>(address)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void SetMemoryProtection(ProcessAddress address, size_t size, ProcessMemoryProtectionTypes value)
		{
			try
			{
				lpProcess->SetMemoryProtection(
					static_cast<AL::OS::ProcessAddress>(address),
					size,
					static_cast<AL::OS::ProcessMemoryProtectionTypes>(value)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}
		/// <exception cref="AL::Exceptions::Exception" />
		void SetMemoryProtection(ProcessAddress address, size_t size, ProcessMemoryProtectionTypes value, [System::Runtime::InteropServices::OutAttribute] ProcessMemoryProtectionTypes% oldType)
		{
			AL::OS::ProcessMemoryProtectionTypes _oldType;

			try
			{
				lpProcess->SetMemoryProtection(
					static_cast<AL::OS::ProcessAddress>(address),
					size,
					static_cast<AL::OS::ProcessMemoryProtectionTypes>(value),
					_oldType
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			oldType = static_cast<ProcessMemoryProtectionTypes>(
				_oldType
			);
		}

		/// <summary>
		/// Returns address of newly allocated memory
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		ProcessAddress AllocateMemory(size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtectionTypes protection)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				_address = lpProcess->AllocateMemory(
					size,
					static_cast<AL::OS::ProcessMemoryAllocationTypes>(type),
					static_cast<AL::OS::ProcessMemoryProtectionTypes>(protection)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			return static_cast<ProcessAddress>(
				_address
			);
		}
		/// <summary>
		/// Returns address of newly allocated memory
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		ProcessAddress AllocateMemory(ProcessAddress address, size_t size, ProcessMemoryAllocationTypes type, ProcessMemoryProtectionTypes protection)
		{
			AL::OS::ProcessAddress _address;

			try
			{
				_address = lpProcess->AllocateMemory(
					static_cast<AL::OS::ProcessAddress>(address),
					size,
					static_cast<AL::OS::ProcessMemoryAllocationTypes>(type),
					static_cast<AL::OS::ProcessMemoryProtectionTypes>(protection)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			return static_cast<ProcessAddress>(
				_address
			);
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void ReleaseMemory(ProcessAddress address, ProcessMemoryReleaseTypes type, size_t size)
		{
			try
			{
				lpProcess->ReleaseMemory(
					address,
					static_cast<AL::OS::ProcessMemoryReleaseTypes>(type),
					size
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::Exceptions::Exception" />
		generic<typename T>
		void ReadMemory(ProcessAddress address, T% value)
		{
			auto buffer = gcnew array<System::Byte>(
				Marshal::SizeOf<T>()
			);

			ReadMemory(
				address,
				buffer
			);

			value = Marshal::FromArray<T>(
				buffer,
				0,
				Marshal::SizeOf<T>()
			);
		}
		/// <exception cref="AL::Exceptions::Exception" />
		void ReadMemory(ProcessAddress address, array<System::Byte>^% buffer)
		{
			auto lpBuffer = new uint8[
				static_cast<size_t>(buffer->Length)
			];

			try
			{
				lpProcess->ReadMemory(
					static_cast<AL::OS::ProcessAddress>(address),
					lpBuffer,
					static_cast<size_t>(buffer->Length)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				delete[] lpBuffer;

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			Marshal::Copy(
				lpBuffer,
				buffer,
				0,
				static_cast<System::UInt32>(buffer->Length)
			);

			delete[] lpBuffer;
		}

		/// <exception cref="AL::Exceptions::Exception" />
		generic<typename T>
		void WriteMemory(ProcessAddress address, T value)
		{
			auto buffer = Marshal::ToArray(
				value
			);

			WriteMemory(
				address,
				buffer
			);
		}
		/// <exception cref="AL::Exceptions::Exception" />
		void WriteMemory(ProcessAddress address, array<System::Byte>^ buffer)
		{
			auto lpBuffer = new uint8[
				static_cast<size_t>(buffer->Length)
			];
			
			Marshal::Copy(
				buffer,
				lpBuffer,
				0,
				static_cast<System::UInt32>(buffer->Length)
			);

			try
			{
				lpProcess->WriteMemory(
					static_cast<AL::OS::ProcessAddress>(address),
					lpBuffer,
					static_cast<size_t>(buffer->Length)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				delete[] lpBuffer;

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			delete[] lpBuffer;
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool SearchMemory([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, ProcessMemoryPattern^ pattern)
		{
			AL::OS::ProcessAddress _address;

			AL::OS::ProcessMemoryPattern _pattern(
				static_cast<size_t>(pattern->GetLength())
			);

			for (size_t i = 0; i < _pattern.GetLength(); ++i)
			{
				_pattern[i].Value = pattern[static_cast<System::UInt32>(i)]->Value;
				_pattern[i].Required = pattern[static_cast<System::UInt32>(i)]->Required;
			}
			
			try
			{
				if (!lpProcess->SearchMemory(_address, _pattern))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}
		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::Exceptions::Exception" />
		bool SearchMemory([System::Runtime::InteropServices::OutAttribute] ProcessAddress% address, ProcessMemoryPattern^ pattern, ProcessAddress offset, ProcessAddress length)
		{
			AL::OS::ProcessAddress _address;

			AL::OS::ProcessMemoryPattern _pattern(
				static_cast<size_t>(pattern->GetLength())
			);

			for (size_t i = 0; i < _pattern.GetLength(); ++i)
			{
				_pattern[i].Value = pattern[static_cast<System::UInt32>(i)]->Value;
				_pattern[i].Required = pattern[static_cast<System::UInt32>(i)]->Required;
			}

			try
			{
				if (!lpProcess->SearchMemory(_address, _pattern, static_cast<AL::OS::ProcessAddress>(offset), static_cast<AL::OS::ProcessAddress>(length)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			address = static_cast<ProcessAddress>(
				_address
			);

			return true;
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void EnumerateMemoryRegions(ProcessEnumMemoryRegionsCallback^ callback)
		{
			System::GC::KeepAlive(
				callback
			);

			ProcessEnumMemoryRegionsNativeCallback nativeCallback(
				callback
			);

			lpProcess->EnumerateMemoryRegions(
				AL::OS::ProcessEnumMemoryRegionsCallback(
					&ProcessEnumMemoryRegionsNativeCallback::Execute,
					nativeCallback
				)
			);
		}

		void Close()
		{
			lpProcess->Close();
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void Terminate(ProcessExitCode exitCode)
		{
			try
			{
				lpProcess->Terminate(
					static_cast<AL::OS::ProcessExitCode>(exitCode)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}
	};
}
