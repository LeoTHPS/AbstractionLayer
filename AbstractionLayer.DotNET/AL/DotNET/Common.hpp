#pragma once
#include <AL/Common.hpp>

#include <AL/DotNET/Marshal.hpp>

namespace AL::DotNET
{
	public enum class Endians
		: typename Get_Enum_Base<AL::Endians>::Type
	{
		Big     = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Big),
		Little  = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Little),

		Machine = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Machine)
	};

	[::System::FlagsAttribute]
	public enum class Platforms
		: typename Get_Enum_Base<AL::Platforms>::Type
	{
		x86        = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::x86),
		x86_64     = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::x86_64),

		ARM        = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::ARM),
		ARM64      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::ARM64),

		Linux      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Linux),
		Windows    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Windows),

		Win32      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Win32),
		Win64      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Win64),

		Linux32    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Linux32),
		Linux64    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Linux64),
		LinuxARM   = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::LinuxARM),
		LinuxARM64 = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::LinuxARM64),

		Machine    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Machine)
	};

	[::System::Runtime::CompilerServices::Extension]
	public ref class Extensions abstract sealed
	{
	public:
		generic<typename T>
		[::System::Runtime::CompilerServices::Extension]
		static T GetAttribute(::System::Type^ type)
		{
			return (T)::System::Attribute::GetCustomAttribute(
				type,
				T::typeid
			);
		}
		generic<typename T>
		[::System::Runtime::CompilerServices::Extension]
		static T GetAttribute(::System::Object^ object)
		{
			return (T)::System::Attribute::GetCustomAttribute(
				object->GetType(),
				T::typeid
			);
		}

		[::System::Runtime::CompilerServices::Extension]
		static array<::System::Attribute^>^ GetAttributes(::System::Type^ type)
		{
			return ::System::Attribute::GetCustomAttributes(
				type
			);
		}
		[::System::Runtime::CompilerServices::Extension]
		static array<::System::Attribute^>^ GetAttributes(::System::Object^ object)
		{
			return ::System::Attribute::GetCustomAttributes(
				object->GetType()
			);
		}

		generic<typename T>
		[::System::Runtime::CompilerServices::Extension]
		static array<T>^ GetAttributes(::System::Type^ type)
		{
			return (array<T>^)::System::Attribute::GetCustomAttributes(
				type,
				T::typeid
			);
		}
		generic<typename T>
		[::System::Runtime::CompilerServices::Extension]
		static array<T>^ GetAttributes(::System::Object^ object)
		{
			return (array<T>^)::System::Attribute::GetCustomAttributes(
				object->GetType(),
				T::typeid
			);
		}
	};

	public ref class BitConverter abstract sealed
	{
	public:
		static bool IsBigEndian()
		{
			return AL::BitConverter::IsBigEndian();
		}

		static bool IsLittleEndian()
		{
			return AL::BitConverter::IsLittleEndian();
		}

#define AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(name) \
			static ::System::Single name(::System::Single value) { return AL::BitConverter::name(value); } \
			static ::System::Double name(::System::Double value) { return AL::BitConverter::name(value); } \
			static ::System::SByte name(::System::SByte value) { return AL::BitConverter::name(value); } \
			static ::System::Int16 name(::System::Int16 value) { return AL::BitConverter::name(value); } \
			static ::System::Int32 name(::System::Int32 value) { return AL::BitConverter::name(value); } \
			static ::System::Int64 name(::System::Int64 value) { return AL::BitConverter::name(value); } \
			static ::System::Byte name(::System::Byte value) { return AL::BitConverter::name(value); } \
			static ::System::UInt16 name(::System::UInt16 value) { return AL::BitConverter::name(value); } \
			static ::System::UInt32 name(::System::UInt32 value) { return AL::BitConverter::name(value); } \
			static ::System::UInt64 name(::System::UInt64 value) { return AL::BitConverter::name(value); }

		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(Flip);
		
		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(ToBigEndian);
		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(ToLittleEndian);
		
		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(FromBigEndian);
		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(FromLittleEndian);

		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(NetworkToHost);
		AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS(HostToNetwork);

#undef AL_DOTNET_BITCONVERTER_DEFINE_NATIVE_FUNCTIONS
	};
}

#include "Exceptions/Exception.hpp"
#include "Exceptions/NotImplementedException.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#include "OS/ntdll.hpp"
#endif

#include "OS/Timer.hpp"
#include "OS/Detour.hpp"
#include "OS/System.hpp"
#include "OS/Library.hpp"
#include "OS/Process.hpp"
#include "OS/Environment.hpp"
