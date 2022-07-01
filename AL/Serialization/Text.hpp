#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Serialization
{
	enum class TextLineEndings
	{
		Auto, LF, CRLF
	};

	template<typename T_STRING, TextLineEndings LINE_ENDING>
	struct Get_Text_Line_Ending;
	template<>
	struct Get_Text_Line_Ending<String, TextLineEndings::LF>
	{
		static constexpr String::Char Value[] = "\n";
	};
	template<>
	struct Get_Text_Line_Ending<WString, TextLineEndings::LF>
	{
		static constexpr WString::Char Value[] = L"\n";
	};
	template<>
	struct Get_Text_Line_Ending<String, TextLineEndings::CRLF>
	{
		static constexpr String::Char Value[] = "\r\n";
	};
	template<>
	struct Get_Text_Line_Ending<WString, TextLineEndings::CRLF>
	{
		static constexpr WString::Char Value[] = L"\r\n";
	};
	template<>
	struct Get_Text_Line_Ending<String, TextLineEndings::Auto>
	{
#if defined(AL_PLATFORM_PICO)
		static constexpr String::Char Value[] = "\n";
#elif defined(AL_PLATFORM_LINUX)
		static constexpr String::Char Value[] = "\n";
#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr String::Char Value[] = "\r\n";
#endif
	};
	template<>
	struct Get_Text_Line_Ending<WString, TextLineEndings::Auto>
	{
#if defined(AL_PLATFORM_PICO)
		static constexpr WString::Char Value[] = L"\n";
#elif defined(AL_PLATFORM_LINUX)
		static constexpr WString::Char Value[] = L"\n";
#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr WString::Char Value[] = L"\r\n";
#endif
	};

	class Text
	{
		WString         string;
		size_t          stringReadOffset = 0;

		TextLineEndings lineEnding = TextLineEndings::Auto;

	public:
		Text()
		{
		}

		Text(Text&& text)
			: string(
				Move(text.string)
			),
			stringReadOffset(
				text.stringReadOffset
			),
			lineEnding(
				text.lineEnding
			)
		{
			text.stringReadOffset = 0;
			text.lineEnding       = TextLineEndings::Auto;
		}
		Text(const Text& text)
			: string(
				text.string
			),
			stringReadOffset(
				text.stringReadOffset
			),
			lineEnding(
				text.lineEnding
			)
		{
		}

		Text(String&& string)
			: string(
				string.ToWString()
			)
		{
		}
		Text(const String& string)
			: string(
				string.ToWString()
			)
		{
		}

		Text(WString&& wstring)
			: string(
				Move(wstring)
			)
		{
		}
		Text(const WString& wstring)
			: string(
				wstring
			)
		{
		}

		virtual ~Text()
		{
		}

		auto GetLineEnding() const
		{
			return lineEnding;
		}

		Void SetLineEnding(TextLineEndings value)
		{
			lineEnding = value;
		}

		// @return AL::False if end of buffer
		Bool Read(String::Char& value)
		{
			if (!Read(&value, 1))
			{

				return False;
			}

			return True;
		}
		// @return AL::False if end of buffer
		template<size_t S>
		Bool Read(String::Char(&value)[S])
		{
			if (!Read(&value[0], S - 1))
			{

				return False;
			}

			return True;
		}
		// @return AL::False if end of buffer
		Bool Read(String& value, size_t length)
		{
			WString _value;

			if (!Read(_value, length))
			{

				return False;
			}

			value.Assign(
				_value.ToString()
			);

			return True;
		}
		// @return AL::False if end of buffer
		Bool Read(String::Char* lpValue, size_t length)
		{
			WString value;

			if (!Read(value, length))
			{

				return False;
			}

			memcpy(
				lpValue,
				value.GetCString(),
				length
			);

			return True;
		}

		// @return AL::False if end of buffer
		Bool Read(WString::Char& value)
		{
			if (!Read(&value, 1))
			{

				return False;
			}

			return True;
		}
		// @return AL::False if end of buffer
		template<size_t S>
		Bool Read(WString::Char(&value)[S])
		{
			if (!Read(&value[0], S - 1))
			{

				return False;
			}

			return True;
		}
		// @return AL::False if end of buffer
		Bool Read(WString& value, size_t length)
		{
			Collections::Array<WString::Char> _value(
				length
			);

			if (!Read(&_value[0], length))
			{

				return False;
			}

			value.Assign(
				&_value[0],
				length
			);

			return True;
		}
		// @return AL::False if end of buffer
		Bool Read(WString::Char* lpValue, size_t length)
		{
			if ((stringReadOffset + length) > string.GetLength())
			{

				return False;
			}

			memcpy(
				lpValue,
				&string[stringReadOffset],
				length
			);

			stringReadOffset += length;

			return True;
		}

		// @return AL::False if end of buffer
		Bool ReadAll(String& value)
		{
			WString _value;

			if (!ReadAll(_value))
			{

				return False;
			}

			value.Assign(
				_value.ToString()
			);

			return True;
		}
		// @return AL::False if end of buffer
		Bool ReadAll(WString& value)
		{
			auto length = string.GetLength();

			if (stringReadOffset == length)
			{

				return False;
			}

			value.Assign(
				string.SubString(
					stringReadOffset
				)
			);

			stringReadOffset = length;

			return True;
		}

		// @return AL::False if end of buffer
		Bool ReadLine(String& value)
		{
			WString _value;

			if (!ReadLine(_value))
			{

				return False;
			}

			value.Assign(
				_value.ToString()
			);

			return True;
		}
		// @return AL::False if end of buffer
		Bool ReadLine(WString& value)
		{
			if (stringReadOffset >= string.GetLength())
			{

				return False;
			}

			switch (GetLineEnding())
			{
				case TextLineEndings::Auto:
				{
					auto i = string.IndexOfAt(
						Get_Text_Line_Ending<WString, TextLineEndings::Auto>::Value,
						stringReadOffset
					);

					if (i == WString::NPOS)
					{
						value.Assign(
							string.SubString(
								stringReadOffset
							)
						);

						stringReadOffset = string.GetLength();
					}
					else
					{
						value.Assign(
							string.SubString(
								stringReadOffset,
								i - stringReadOffset
							)
						);

						stringReadOffset = (i + (sizeof(Get_Text_Line_Ending<WString, TextLineEndings::Auto>::Value) - 1));
					}
				}
				break;

				case TextLineEndings::LF:
				{
					auto i = string.IndexOfAt(
						Get_Text_Line_Ending<WString, TextLineEndings::LF>::Value,
						stringReadOffset
					);

					if (i == WString::NPOS)
					{
						value.Assign(
							string.SubString(
								stringReadOffset
							)
						);

						stringReadOffset = string.GetLength();
					}
					else
					{
						value.Assign(
							string.SubString(
								stringReadOffset,
								i - stringReadOffset
							)
						);

						stringReadOffset = (i + (sizeof(Get_Text_Line_Ending<WString, TextLineEndings::LF>::Value) - 1));
					}
				}
				break;

				case TextLineEndings::CRLF:
				{
					auto i = string.IndexOfAt(
						Get_Text_Line_Ending<WString, TextLineEndings::CRLF>::Value,
						stringReadOffset
					);

					if (i == WString::NPOS)
					{
						value.Assign(
							string.SubString(
								stringReadOffset
							)
						);

						stringReadOffset = string.GetLength();
					}
					else
					{
						value.Assign(
							string.SubString(
								stringReadOffset,
								i - stringReadOffset
							)
						);

						stringReadOffset = (i + (sizeof(Get_Text_Line_Ending<WString, TextLineEndings::CRLF>::Value) - 1));
					}
				}
				break;
			}

			return True;
		}

		Void Write(String::Char value)
		{
			String _value(
				value,
				1
			);

			Write(
				_value
			);
		}
		Void Write(const String& value)
		{
			Write(
				value.GetCString(),
				value.GetLength()
			);
		}
		template<size_t S>
		Void Write(const String::Char(&value)[S])
		{
			Write(
				&value[0],
				S - 1
			);
		}
		Void Write(const String::Char* lpValue, size_t length)
		{
			String string(
				lpValue,
				length
			);

			auto value = string.ToWString();

			Write(
				value
			);
		}

		Void Write(WString::Char value)
		{
			string.Append(
				value
			);
		}
		Void Write(const WString& value)
		{
			string.Append(
				value
			);
		}
		template<size_t S>
		Void Write(const WString::Char(&value)[S])
		{
			string.Append(
				&value[0],
				S - 1
			);
		}
		Void Write(const WString::Char* lpValue, size_t length)
		{
			string.Append(
				lpValue,
				length
			);
		}

		Void WriteLine()
		{
			switch (GetLineEnding())
			{
				case TextLineEndings::Auto:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::Auto>::Value);
					break;

				case TextLineEndings::LF:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::LF>::Value);
					break;

				case TextLineEndings::CRLF:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::CRLF>::Value);
					break;
			}
		}

		Void WriteLine(String::Char value)
		{
			WriteLine(
				&value,
				1
			);
		}
		Void WriteLine(const String& value)
		{
			WriteLine(
				value.GetCString(),
				value.GetLength()
			);
		}
		template<size_t S>
		Void WriteLine(const String::Char(&value)[S])
		{
			WriteLine(
				&value[0],
				S - 1
			);
		}
		Void WriteLine(const String::Char* lpValue, size_t length)
		{
			Write(
				lpValue,
				length
			);

			switch (GetLineEnding())
			{
				case TextLineEndings::Auto:
					Write(Get_Text_Line_Ending<String, TextLineEndings::Auto>::Value);
					break;

				case TextLineEndings::LF:
					Write(Get_Text_Line_Ending<String, TextLineEndings::LF>::Value);
					break;

				case TextLineEndings::CRLF:
					Write(Get_Text_Line_Ending<String, TextLineEndings::CRLF>::Value);
					break;
			}
		}

		Void WriteLine(WString::Char value)
		{
			WriteLine(
				&value,
				1
			);
		}
		Void WriteLine(const WString& value)
		{
			WriteLine(
				value.GetCString(),
				value.GetLength()
			);
		}
		template<size_t S>
		Void WriteLine(const WString::Char(&value)[S])
		{
			WriteLine(
				&value[0],
				S - 1
			);
		}
		Void WriteLine(const WString::Char* lpValue, size_t length)
		{
			Write(
				lpValue,
				length
			);

			switch (GetLineEnding())
			{
				case TextLineEndings::Auto:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::Auto>::Value);
					break;

				case TextLineEndings::LF:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::LF>::Value);
					break;

				case TextLineEndings::CRLF:
					Write(Get_Text_Line_Ending<WString, TextLineEndings::CRLF>::Value);
					break;
			}
		}

		String  ToString() const
		{
			auto wstring = ToWString();

			return wstring.ToString();
		}
		WString ToWString() const
		{
			return string;
		}

		Text& operator = (Text&& text)
		{
			string           = Move(text.string);
			stringReadOffset = text.stringReadOffset;
			text.stringReadOffset = 0;

			lineEnding = text.lineEnding;
			text.lineEnding = TextLineEndings::Auto;

			return *this;
		}
		Text& operator = (const Text& text)
		{
			string           = text.string;
			stringReadOffset = text.stringReadOffset;

			lineEnding       = text.lineEnding;

			return *this;
		}

		Text& operator = (String&& string)
		{
			this->string.Assign(
				string.ToWString()
			);

			stringReadOffset = 0;

			return *this;
		}
		Text& operator = (const String& string)
		{
			this->string.Assign(
				string.ToWString()
			);

			stringReadOffset = 0;

			return *this;
		}

		Text& operator = (WString&& wstring)
		{
			string = Move(wstring);
			stringReadOffset = 0;

			return *this;
		}
		Text& operator = (const WString& wstring)
		{
			string = wstring;
			stringReadOffset = 0;

			return *this;
		}

		Bool operator == (const Text& text) const
		{
			if (lineEnding != text.lineEnding)
			{

				return False;
			}

			if (stringReadOffset != text.stringReadOffset)
			{

				return False;
			}

			if (string != text.string)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Text& text) const
		{
			if (operator==(text))
			{

				return False;
			}

			return True;
		}
	};
}
