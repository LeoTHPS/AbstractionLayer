#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/String.hpp"

#define AL_DEFINE_TO_STRING(__type__, ...) \
	template<> \
	inline AL::String AL::ToString<__type__>__VA_ARGS__

#define AL_DEFINE_FROM_STRING(__type__, ...) \
	template<> \
	inline __type__ AL::FromString<__type__>__VA_ARGS__

#define AL_DEFINE_TO_WSTRING(__type__, ...) \
	template<> \
	inline AL::WString AL::ToWString<__type__>__VA_ARGS__

#define AL_DEFINE_FROM_WSTRING(__type__, ...) \
	template<> \
	inline __type__ AL::FromWString<__type__>__VA_ARGS__

namespace AL
{
	typedef Collections::String  String;
	typedef Collections::WString WString;

	template<typename T>
	inline String ToString(T value)
	{
		if constexpr (Is_Enum_Or_Integer<T>::Value)
		{
			typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE;

			auto string = ToString(
				static_cast<T_BASE>(value)
			);

			return string;
		}
		else if constexpr (Is_Decimal<T>::Value)
		{
			typedef typename Get_Decimal_Base<T>::Type T_BASE;

			auto string = ToString(
				static_cast<T_BASE>(value)
			);

			return string;
		}
		else if constexpr (Is_Type<T, String>::Value)
		{

			return value;
		}
		else if constexpr (Is_Type<T, WString>::Value)
		{

			return value.ToString();
		}
	}
	template<typename T>
	inline WString ToWString(T value)
	{
		if constexpr (Is_Enum_Or_Integer<T>::Value)
		{
			typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE;

			auto wstring = ToWString(
				static_cast<T_BASE>(value)
			);

			return wstring;
		}
		else if constexpr (Is_Decimal<T>::Value)
		{
			typedef typename Get_Decimal_Base<T>::Type T_BASE;

			auto wstring = ToWString(
				static_cast<T_BASE>(value)
			);

			return wstring;
		}
		else if constexpr (Is_Type<T, String>::Value)
		{

			return value.ToWString();
		}
		else if constexpr (Is_Type<T, WString>::Value)
		{

			return value;
		}
	}

	template<typename T>
	inline T FromString(const String& string)
	{
		if constexpr (Is_Enum_Or_Integer<T>::Value)
		{
			typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE;

			auto value = FromString<T_BASE>(
				string
			);

			return static_cast<T>(
				value
			);
		}
		else if constexpr (Is_Decimal<T>::Value)
		{
			typedef typename Get_Decimal_Base<T>::Type T_BASE;

			auto value = FromString<T_BASE>(
				string
			);

			return static_cast<T>(
				value
			);
		}
		else if constexpr (Is_Type<T, String>::Value)
		{

			return string;
		}
		else if constexpr (Is_Type<T, WString>::Value)
		{

			return string.ToWString();
		}
	}
	template<typename T>
	inline T FromWString(const WString& wstring)
	{
		if constexpr (Is_Enum_Or_Integer<T>::Value)
		{
			typedef typename Get_Enum_Or_Integer_Base<T>::Type T_BASE;

			auto value = FromWString<T_BASE>(
				wstring
			);

			return static_cast<T>(
				value
			);
		}
		else if constexpr (Is_Decimal<T>::Value)
		{
			typedef typename Get_Decimal_Base<T>::Type T_BASE;

			auto value = FromWString<T_BASE>(
				wstring
			);

			return static_cast<T>(
				value
			);
		}
		else if constexpr (Is_Type<T, String>::Value)
		{

			return wstring.ToString();
		}
		else if constexpr (Is_Type<T, WString>::Value)
		{

			return wstring;
		}
	}
}

AL_DEFINE_TO_STRING(
	AL::int8,
	(AL::int8 value)
	{
		return AL::String::Format(
			"%i",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::int8,
	(const AL::String& string)
	{
		auto value = std::strtol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int8>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::int8,
	(AL::int8 value)
	{
		return AL::WString::Format(
			L"%i",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::int8,
	(const AL::WString& string)
	{
		auto value = std::wcstol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int8>(
			value
		);
	}
);
AL_DEFINE_TO_STRING(
	AL::uint8,
	(AL::uint8 value)
	{
		return AL::String::Format(
			"%u",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::uint8,
	(const AL::String& string)
	{
		auto value = std::strtoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint8>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::uint8,
	(AL::uint8 value)
	{
		return AL::WString::Format(
			L"%u",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::uint8,
	(const AL::WString& string)
	{
		auto value = std::wcstoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint8>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::int16,
	(AL::int16 value)
	{
		return AL::String::Format(
			"%i",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::int16,
	(const AL::String& string)
	{
		auto value = std::strtol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int16>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::int16,
	(AL::int16 value)
	{
		return AL::WString::Format(
			L"%i",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::int16,
	(const AL::WString& string)
	{
		auto value = std::wcstol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int16>(
			value
		);
	}
);
AL_DEFINE_TO_STRING(
	AL::uint16,
	(AL::uint16 value)
	{
		return AL::String::Format(
			"%u",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::uint16,
	(const AL::String& string)
	{
		auto value = std::strtoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint16>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::uint16,
	(AL::uint16 value)
	{
		return AL::WString::Format(
			L"%u",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::uint16,
	(const AL::WString& string)
	{
		auto value = std::wcstoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint16>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::int32,
	(AL::int32 value)
	{
		return AL::String::Format(
			"%i",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::int32,
	(const AL::String& string)
	{
		auto value = std::strtol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int32>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::int32,
	(AL::int32 value)
	{
		return AL::WString::Format(
			L"%i",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::int32,
	(const AL::WString& string)
	{
		auto value = std::wcstol(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int32>(
			value
		);
	}
);
AL_DEFINE_TO_STRING(
	AL::uint32,
	(AL::uint32 value)
	{
		return AL::String::Format(
			"%u",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::uint32,
	(const AL::String& string)
	{
		auto value = std::strtoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint32>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::uint32,
	(AL::uint32 value)
	{
		return AL::WString::Format(
			L"%u",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::uint32,
	(const AL::WString& string)
	{
		auto value = std::wcstoul(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint32>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::int64,
	(AL::int64 value)
	{
		return AL::String::Format(
			"%lli",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::int64,
	(const AL::String& string)
	{
		auto value = std::strtoll(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int64>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::int64,
	(AL::int64 value)
	{
		return AL::WString::Format(
			L"%lli",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::int64,
	(const AL::WString& string)
	{
		auto value = std::wcstoll(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int64>(
			value
		);
	}
);
AL_DEFINE_TO_STRING(
	AL::uint64,
	(AL::uint64 value)
	{
		return AL::String::Format(
			"%llu",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::uint64,
	(const AL::String& string)
	{
		auto value = std::strtoull(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint64>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::uint64,
	(AL::uint64 value)
	{
		return AL::WString::Format(
			L"%llu",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::uint64,
	(const AL::WString& string)
	{
		auto value = std::wcstoull(
			string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint64>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::Float,
	(AL::Float value)
	{
		return AL::String::Format(
			"%f",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::Float,
	(const AL::String& string)
	{
		auto value = std::strtof(
			string.GetCString(),
			nullptr
		);

		return static_cast<Float>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::Float,
	(AL::Float value)
	{
		return AL::WString::Format(
			L"%f",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::Float,
	(const AL::WString& string)
	{
		auto value = std::wcstof(
			string.GetCString(),
			nullptr
		);

		return static_cast<Float>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::Double,
	(AL::Double value)
	{
		return AL::String::Format(
			"%f",
			value
		);
	}
);
AL_DEFINE_FROM_STRING(
	AL::Double,
	(const AL::String& string)
	{
		auto value = std::strtod(
			string.GetCString(),
			nullptr
		);

		return static_cast<Double>(
			value
		);
	}
);
AL_DEFINE_TO_WSTRING(
	AL::Double,
	(AL::Double value)
	{
		return AL::WString::Format(
			L"%f",
			value
		);
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::Double,
	(const AL::WString& string)
	{
		auto value = std::wcstod(
			string.GetCString(),
			nullptr
		);

		return static_cast<Double>(
			value
		);
	}
);

AL_DEFINE_TO_STRING(
	AL::Bool,
	(AL::Bool value)
	{
		return value ? "true" : "false";
	}
);
AL_DEFINE_FROM_STRING(
	AL::Bool,
	(const AL::String& string)
	{
		return string.ToLower() == "true";
	}
);
AL_DEFINE_TO_WSTRING(
	AL::Bool,
	(AL::Bool value)
	{
		return value ? L"true" : L"false";
	}
);
AL_DEFINE_FROM_WSTRING(
	AL::Bool,
	(const AL::WString& wstring)
	{
		return wstring.ToLower() == L"true";
	}
);

AL_DEFINE_TO_STRING(
	AL::Platforms,
	(AL::Platforms value)
	{
		String string;
		Bool   string_IsAppended = False;
		auto   string_AppendIfSet = [&string, &string_IsAppended, value](const String::Char* _lpBuffer, Platforms _flag)
		{
			if (BitMask<Platforms>::IsSet(value, _flag))
			{
				if (string_IsAppended)
				{
					string.Append(
						" | "
					);
				}

				string.Append(
					_lpBuffer
				);

				string_IsAppended = True;
			}
		};

		// Debug
		{
			string_AppendIfSet("Debug", Platforms::Debug);
		}

		// GNU/MSVC/Clang
		{
			string_AppendIfSet("GNU",   Platforms::GNU);
			string_AppendIfSet("MSVC",  Platforms::MSVC);
			string_AppendIfSet("Clang", Platforms::Clang);
		}

		// ARM/ARM64/x86/x86_64
		{
			string_AppendIfSet("ARM",    Platforms::ARM);
			string_AppendIfSet("ARM64",  Platforms::ARM64);
			string_AppendIfSet("x86",    Platforms::x86);
			string_AppendIfSet("x86_64", Platforms::x86_64);
		}

		// Pico/Linux/MinGW/Windows
		{
			string_AppendIfSet("Pico",    Platforms::Pico);
			string_AppendIfSet("Linux",   Platforms::Linux);
			string_AppendIfSet("MinGW",   Platforms::MinGW);
			string_AppendIfSet("Windows", Platforms::Windows);
		}

		return string;
	}
);
AL_DEFINE_TO_WSTRING(
	AL::Platforms,
	(AL::Platforms value)
	{
		WString wstring;
		Bool    wstring_IsAppended = False;
		auto    wstring_AppendIfSet = [&wstring, &wstring_IsAppended, value](const WString::Char* _lpBuffer, Platforms _flag)
		{
			if (BitMask<Platforms>::IsSet(value, _flag))
			{
				if (wstring_IsAppended)
				{
					wstring.Append(
						L" | "
					);
				}

				wstring.Append(
					_lpBuffer
				);

				wstring_IsAppended = True;
			}
		};

		// Debug
		{
			wstring_AppendIfSet(L"Debug", Platforms::Debug);
		}

		// GNU/MSVC/Clang
		{
			wstring_AppendIfSet(L"GNU",   Platforms::GNU);
			wstring_AppendIfSet(L"MSVC",  Platforms::MSVC);
			wstring_AppendIfSet(L"Clang", Platforms::Clang);
		}

		// ARM/ARM64/x86/x86_64
		{
			wstring_AppendIfSet(L"ARM",    Platforms::ARM);
			wstring_AppendIfSet(L"ARM64",  Platforms::ARM64);
			wstring_AppendIfSet(L"x86",    Platforms::x86);
			wstring_AppendIfSet(L"x86_64", Platforms::x86_64);
		}

		// Pico/Linux/MinGW/Windows
		{
			wstring_AppendIfSet(L"Pico",    Platforms::Pico);
			wstring_AppendIfSet(L"Linux",   Platforms::Linux);
			wstring_AppendIfSet(L"MinGW",   Platforms::MinGW);
			wstring_AppendIfSet(L"Windows", Platforms::Windows);
		}

		return wstring;
	}
);
