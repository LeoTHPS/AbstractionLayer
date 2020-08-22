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
					_to_obj.SetSize(_size - 1);

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
	};
#endif
}
