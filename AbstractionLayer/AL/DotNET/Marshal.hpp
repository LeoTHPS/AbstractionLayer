#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#if defined(__cplusplus_cli)
	#include <msclr\marshal.h>
#endif

namespace AL::DotNET
{
#if !defined(__cplusplus_cli)
	class Marshal;
#else
	public ref class Marshal sealed abstract
	{
	public:
		static ::System::IntPtr ToPointer(void* lpValue)
		{
			return ::System::IntPtr(
				lpValue
			);
		}

		template<typename T = void>
		static T* ToNativePointer(::System::IntPtr pointer)
		{
			return reinterpret_cast<T*>(
				pointer.ToPointer()
			);
		}
		
		generic<typename T>
		static ::System::Runtime::InteropServices::GCHandle GetHandle(T value)
		{
			auto hValue = ::System::Runtime::InteropServices::GCHandle::Alloc(
				value
			);

			return hValue;
		}

		static void FreeHandle(::System::Runtime::InteropServices::GCHandle handle)
		{
			handle.Free();
		}

		static ::System::String^ ToString(const String& string)
		{
			return ToString(
				string.GetCString(),
				string.GetLength()
			);
		}
		static ::System::String^ ToString(const char* string, size_t length)
		{
			return msclr::interop::details::InternalAnsiToStringHelper(
				string,
				length
			);
		}

		static ::System::String^ ToString(const WString& wstring)
		{
			return ToString(
				wstring.GetCString(),
				wstring.GetLength()
			);
		}
		static ::System::String^ ToString(const wchar_t* wstring, size_t length)
		{
			return msclr::interop::details::InternalUnicodeToStringHelper(
				wstring,
				length
			);
		}

		static String ToNativeString(::System::String^ string)
		{
			String nativeString;

			if (auto stringSize = msclr::interop::details::GetAnsiStringSize(string))
			{
				// AL::String::SetCapacity automatically adds +1 for NULL terminator
				nativeString.SetCapacity(
					stringSize - 1
				);

				msclr::interop::details::WriteAnsiString(
					&nativeString[0],
					stringSize,
					string
				);

				nativeString.RefreshLength();
			}

			return nativeString;
		}

		static WString ToNativeWString(::System::String^ string)
		{
			return WString::Format(
				L"%S",
				ToNativeString(string).GetCString()
			);
		}

		generic<typename T>
		static array<::System::Byte>^ ToArray(T value)
		{
			if (T::typeid->IsEnum)
			{
				switch (SizeOf<T>())
				{
					case 1: return ToArray(static_cast<::System::Byte>(value));
					case 2: return ToArray(static_cast<::System::UInt16>(value));
					case 4: return ToArray(static_cast<::System::UInt32>(value));
					case 8: return ToArray(static_cast<::System::UInt64>(value));
				}
			}
			else
			{
				auto T_Size = SizeOf<T>();

				auto hBuffer = ::System::Runtime::InteropServices::Marshal::AllocHGlobal(
					T_Size
				);

				::System::Runtime::InteropServices::Marshal::StructureToPtr(
					value,
					hBuffer,
					false
				);

				auto buffer = gcnew array<::System::Byte>(
					T_Size
				);

				::System::Runtime::InteropServices::Marshal::Copy(
					hBuffer,
					buffer,
					0,
					T_Size
				);

				::System::Runtime::InteropServices::Marshal::FreeHGlobal(
					hBuffer
				);

				return buffer;
			}

			throw gcnew ::System::NotImplementedException();
		}

		generic<typename T>
		static T FromArray(array<::System::Byte>^ buffer, ::System::UInt32 offset)
		{
			if (T::typeid->IsEnum)
			{
				switch (SizeOf<T>())
				{
					case 1: return T(FromArray<::System::Byte>(buffer, offset));
					case 2: return T(FromArray<::System::UInt16>(buffer, offset));
					case 4: return T(FromArray<::System::UInt32>(buffer, offset));
					case 8: return T(FromArray<::System::UInt64>(buffer, offset));
				}
			}
			else
			{
				auto hBuffer = ::System::Runtime::InteropServices::Marshal::AllocHGlobal(
					buffer->Length
				);

				::System::Runtime::InteropServices::Marshal::Copy(
					buffer,
					offset,
					hBuffer,
					SizeOf<T>()
				);

				auto obj = ::System::Runtime::InteropServices::Marshal::PtrToStructure(
					hBuffer,
					T::typeid
				);

				::System::Runtime::InteropServices::Marshal::FreeHGlobal(
					hBuffer
				);

				return static_cast<T>(
					obj
				);
			}

			throw gcnew ::System::NotImplementedException();
		}

		generic<typename T>
		static ::System::UInt32 SizeOf()
		{
			// rhetorical: why is this even needed?
			if (T::typeid->IsEnum)
			{

				return sizeof(
					T
				);
			}
			
			return ::System::Runtime::InteropServices::Marshal::SizeOf(
				T::typeid
			);
		}
	};
#endif
}
