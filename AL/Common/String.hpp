#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/String.hpp"

#define AL_DEFINE_STRING_CONVERSION(__type__, __toString__, __fromString__) \
	template<> \
	inline AL::String AL::ToString __toString__ \
	template<> \
	inline __type__ AL::FromString __fromString__

#define AL_DEFINE_WSTRING_CONVERSION(__type__, __toString__, __fromString__) \
	template<> \
	inline AL::WString AL::ToWString __toString__ \
	template<> \
	inline __type__ AL::FromWString __fromString__

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
	}
}

AL_DEFINE_STRING_CONVERSION(
	AL::int8,
	(AL::int8 _value)
	{
		return AL::String::Format(
			"%i",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int8>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::int8,
	(AL::int8 _value)
	{
		return AL::WString::Format(
			L"%i",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int8>(
			value
		);
	}
);
AL_DEFINE_STRING_CONVERSION(
	AL::uint8,
	(AL::uint8 _value)
	{
		return AL::String::Format(
			"%u",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint8>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::uint8,
	(AL::uint8 _value)
	{
		return AL::WString::Format(
			L"%u",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint8>(
			value
		);
	}
);

AL_DEFINE_STRING_CONVERSION(
	AL::int16,
	(AL::int16 _value)
	{
		return AL::String::Format(
			"%i",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int16>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::int16,
	(AL::int16 _value)
	{
		return AL::WString::Format(
			L"%i",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int16>(
			value
		);
	}
);
AL_DEFINE_STRING_CONVERSION(
	AL::uint16,
	(AL::uint16 _value)
	{
		return AL::String::Format(
			"%u",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint16>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::uint16,
	(AL::uint16 _value)
	{
		return AL::WString::Format(
			L"%u",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint16>(
			value
		);
	}
);

AL_DEFINE_STRING_CONVERSION(
	AL::int32,
	(AL::int32 _value)
	{
		return AL::String::Format(
			"%i",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int32>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::int32,
	(AL::int32 _value)
	{
		return AL::WString::Format(
			L"%i",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstol(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int32>(
			value
		);
	}
);
AL_DEFINE_STRING_CONVERSION(
	AL::uint32,
	(AL::uint32 _value)
	{
		return AL::String::Format(
			"%u",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint32>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::uint32,
	(AL::uint32 _value)
	{
		return AL::WString::Format(
			L"%u",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstoul(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint32>(
			value
		);
	}
);

AL_DEFINE_STRING_CONVERSION(
	AL::int64,
	(AL::int64 _value)
	{
		return AL::String::Format(
			"%lli",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtoll(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int64>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::int64,
	(AL::int64 _value)
	{
		return AL::WString::Format(
			L"%lli",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstoll(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<int64>(
			value
		);
	}
);
AL_DEFINE_STRING_CONVERSION(
	AL::uint64,
	(AL::uint64 _value)
	{
		return AL::String::Format(
			"%llu",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtoull(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint64>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::uint64,
	(AL::uint64 _value)
	{
		return AL::WString::Format(
			L"%llu",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstoull(
			_string.GetCString(),
			nullptr,
			0
		);

		return static_cast<uint64>(
			value
		);
	}
);

AL_DEFINE_STRING_CONVERSION(
	AL::Float,
	(AL::Float _value)
	{
		return AL::String::Format(
			"%f",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtof(
			_string.GetCString(),
			nullptr
		);

		return static_cast<Float>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::Float,
	(AL::Float _value)
	{
		return AL::WString::Format(
			L"%f",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstof(
			_string.GetCString(),
			nullptr
		);

		return static_cast<Float>(
			value
		);
	}
);
AL_DEFINE_STRING_CONVERSION(
	AL::Double,
	(AL::Double _value)
	{
		return AL::String::Format(
			"%f",
			_value
		);
	},
	(const AL::String& _string)
	{
		auto value = std::strtod(
			_string.GetCString(),
			nullptr
		);

		return static_cast<Double>(
			value
		);
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::Double,
	(AL::Double _value)
	{
		return AL::WString::Format(
			L"%f",
			_value
		);
	},
	(const AL::WString& _string)
	{
		auto value = std::wcstod(
			_string.GetCString(),
			nullptr
		);

		return static_cast<Double>(
			value
		);
	}
);

AL_DEFINE_STRING_CONVERSION(
	AL::Bool,
	(AL::Bool _value)
	{
		return _value ? "true" : "false";
	},
	(const AL::String& _string)
	{
		return _string.ToLower() == "true";
	}
);
AL_DEFINE_WSTRING_CONVERSION(
	AL::Bool,
	(AL::Bool _value)
	{
		return _value ? L"true" : L"false";
	},
	(const AL::WString& _wstring)
	{
		return _wstring.ToLower() == L"true";
	}
);
