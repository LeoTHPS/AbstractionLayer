#pragma once
#include <AL/Common.hpp>

#include <AL/DotNET/Marshal.hpp>

namespace AL::DotNET::Collections
{
	ref class ByteBuffer;
}

namespace AL::DotNET
{
	public enum class Endians
		: typename Get_Enum_Base<AL::Endians>::Type
	{
		Big     = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Big),
		Little  = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Little),

		Machine = static_cast<typename Get_Enum_Base<AL::Endians>::Type>(AL::Endians::Machine)
	};

	public enum class Platforms
		: typename Get_Enum_Base<AL::Platforms>::Type
	{
		Win32      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Win32),
		Win64      = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Win64),
		Linux32    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Linux32),
		Linux64    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Linux64),
		LinuxARM   = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::LinuxARM),
		LinuxARM64 = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::LinuxARM64),

		Machine    = static_cast<typename Get_Enum_Base<AL::Platforms>::Type>(AL::Platforms::Current)
	};

	[System::Runtime::CompilerServices::Extension]
	public ref class Extensions abstract sealed
    {
	public:
		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static T GetAttribute(System::Type^ type)
        {
            return (T)System::Attribute::GetCustomAttribute(
                type,
                T::typeid
            );
        }
		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static T GetAttribute(System::Object^ object)
        {
            return (T)System::Attribute::GetCustomAttribute(
                object->GetType(),
                T::typeid
            );
        }

		[System::Runtime::CompilerServices::Extension]
        static array<System::Attribute^>^ GetAttributes(System::Type^ type)
        {
            return System::Attribute::GetCustomAttributes(
                type
            );
        }
		[System::Runtime::CompilerServices::Extension]
		static array<System::Attribute^>^ GetAttributes(System::Object^ object)
        {
            return System::Attribute::GetCustomAttributes(
                object->GetType()
            );
        }

		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static array<T>^ GetAttributes(System::Type^ type)
        {
            return (array<T>^)System::Attribute::GetCustomAttributes(
                type,
				T::typeid
            );
        }
		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static array<T>^ GetAttributes(System::Object^ object)
        {
            return (array<T>^)System::Attribute::GetCustomAttributes(
                object->GetType(),
                T::typeid
            );
        }
        
		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static T MarshalToStruct(array<System::Byte>^ buffer)
        {
			return Marshal::FromArray<T>(
				buffer,
				0,
				buffer->Length
			);
        }
		
		generic<typename T>
        [System::Runtime::CompilerServices::Extension]
		static array<System::Byte>^ MarshalFromStruct(T value)
        {
			return Marshal::ToArray(
				value
			);
        }

		static array<System::Byte>^ ToArray(Collections::ByteBuffer^ buffer);
	};
}

#include "Exceptions/Exception.hpp"
