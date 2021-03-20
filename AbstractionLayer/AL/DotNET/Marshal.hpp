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
			using namespace msclr::interop::details;

			String _to_obj;

			if (string)
			{
				size_t _size = GetAnsiStringSize(string);

				if (_size > 1)
				{
					// -1 because resize will automatically +1 for the NULL
					_to_obj.SetCapacity(_size - 1);

					char* _dest_buf = &(_to_obj[0]);

					WriteAnsiString(_dest_buf, _size, string);
				}
			}

			return _to_obj;
		}
		static WString ToNativeWString(::System::String^ string)
		{
			return WString::Format(
				L"%S",
				ToNativeString(string)
			);
		}

		generic<typename T>
		static array<System::Byte>^ ToArray(T value)
		{
			auto hBuffer = System::Runtime::InteropServices::Marshal::AllocHGlobal(
				SizeOf<T>()
			);

			System::Runtime::InteropServices::Marshal::StructureToPtr(
				value,
				hBuffer,
				false
			);

			auto buffer = gcnew array<System::Byte>(
				SizeOf<T>()
			);

			System::Runtime::InteropServices::Marshal::Copy(
				hBuffer,
				buffer,
				0,
				SizeOf<T>()
			);

			System::Runtime::InteropServices::Marshal::FreeHGlobal(
				hBuffer
			);

			return buffer;
		}

		generic<typename T>
		static T FromArray(array<System::Byte>^ buffer, System::UInt32 offset, System::UInt32 count)
		{
			auto hBuffer = System::Runtime::InteropServices::Marshal::AllocHGlobal(
				buffer->Length
			);

			System::Runtime::InteropServices::Marshal::Copy(
				buffer,
				offset,
				hBuffer,
				count
			);

			auto obj = System::Runtime::InteropServices::Marshal::PtrToStructure(
				hBuffer,
				T::typeid
			);

			System::Runtime::InteropServices::Marshal::FreeHGlobal(
				hBuffer
			);

			return static_cast<T>(
				obj
			);
		}

		static void Copy(void* source, array<System::Byte>^% destination, System::UInt32 offset, System::UInt32 count)
		{
			Copy(System::IntPtr(source), destination, offset, count);
		}
		static void Copy(System::IntPtr source, array<System::Byte>^% destination, System::UInt32 offset, System::UInt32 count)
		{
			System::Runtime::InteropServices::Marshal::Copy(
				source,
				destination,
				static_cast<int>(offset),
				static_cast<int>(count)
			);
		}
		static void Copy(array<System::Byte>^ source, void* destination, System::UInt32 offset, System::UInt32 count)
		{
			Copy(source, System::IntPtr(destination), offset, count);
		}
		static void Copy(array<System::Byte>^ source, System::IntPtr destination, System::UInt32 offset, System::UInt32 count)
		{
			System::Runtime::InteropServices::Marshal::Copy(
				source,
				static_cast<int>(offset),
				destination,
				static_cast<int>(count)
			);
		}

		generic<typename T>
		static System::UInt32 SizeOf()
		{
			return System::Runtime::InteropServices::Marshal::SizeOf(
				T::typeid
			);
		}
	};
#endif
}
