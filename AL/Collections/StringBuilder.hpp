#pragma once
#include "AL/Common.hpp"

#include "String.hpp"

namespace AL::Collections
{
	template<typename T_CHAR>
	class _StringBuilder
	{
		_String<T_CHAR> buffer;
		Bool            appendAsDecimal  = True;
		size_t          appendAsHexCount = 0;

	public:
		struct _dec {};
		struct _hex {};
		struct _hex_once {};

		inline static const _dec      dec;
		inline static const _hex      hex;
		inline static const _hex_once hex_once;

		typedef T_CHAR        Char;
		typedef _String<Char> String;

		_StringBuilder()
		{
		}

		_StringBuilder(_StringBuilder&& stringBuilder)
			: buffer(
				Move(stringBuilder.buffer)
			)
		{
		}
		_StringBuilder(const _StringBuilder& stringBuilder)
			: buffer(
				stringBuilder.buffer
			)
		{
		}

		virtual ~_StringBuilder()
		{
		}

		auto GetSize() const
		{
			return buffer.GetSize();
		}

		auto GetLength() const
		{
			return buffer.GetLength();
		}

		auto ToString() const
		{
			return buffer;
		}

		Void Clear()
		{
			buffer.Clear();
		}

		Void SetHex(Bool set = True)
		{
			appendAsDecimal  = !set;
			appendAsHexCount = 0;
		}

		template<typename T>
		_StringBuilder& Append(T value)
		{
			if ((appendAsHexCount != 0) && (--appendAsHexCount == 0))
			{
				SetHex(
					False
				);
			}

			if constexpr (Is_Type<T, _dec>::Value || Is_Type<T, _hex>::Value || Is_Type<T, _hex_once>::Value)
			{
				SetHex(
					Is_Type<T, _hex>::Value || Is_Type<T, _hex_once>::Value
				);

				appendAsHexCount = Is_Type<T, _hex_once>::Value ? 2 : 0;
			}
			else if constexpr (Is_Type<Char, typename AL::String::Char>::Value)
			{
				if (!appendAsDecimal && (Is_Pointer<T>::Value || Is_Enum_Or_Integer<T>::Value))
				{
					auto string = AL::String::Format(
						"%X",
						value
					);

					buffer.Append(
						string
					);
				}
				else
				{
					auto string = AL::ToString<T>(
						Forward<T>(value)
					);

					buffer.Append(
						string
					);
				}
			}
			else
			{
				if (!appendAsDecimal && (Is_Pointer<T>::Value || Is_Enum_Or_Integer<T>::Value))
				{
					auto string = AL::WString::Format(
						L"%X",
						value
					);

					buffer.Append(
						string
					);
				}
				else
				{
					auto string = AL::ToWString<T>(
						Forward<T>(value)
					);

					buffer.Append(
						string
					);
				}
			}

			return *this;
		}
		_StringBuilder& Append(Char value)
		{
			buffer.Append(
				value
			);

			return *this;
		}
		_StringBuilder& Append(Char* value)
		{
			buffer.Append(
				value
			);

			return *this;
		}
		_StringBuilder& Append(const Char* value)
		{
			buffer.Append(
				value
			);

			return *this;
		}
		_StringBuilder& Append(const String& value)
		{
			buffer.Append(
				value
			);

			return *this;
		}

		template<typename T>
		_StringBuilder& operator << (T value)
		{
			Append(
				Forward<T>(value)
			);

			return *this;
		}
		_StringBuilder& operator << (Char value)
		{
			Append(
				value
			);

			return *this;
		}
		_StringBuilder& operator << (Char* value)
		{
			Append(
				value
			);

			return *this;
		}
		_StringBuilder& operator << (const Char* value)
		{
			Append(
				value
			);

			return *this;
		}
		_StringBuilder& operator << (const String& value)
		{
			Append(
				value
			);

			return *this;
		}

		_StringBuilder& operator = (_StringBuilder&& stringBuilder)
		{
			buffer = Move(
				stringBuilder.buffer
			);

			return *this;
		}
		_StringBuilder& operator = (const _StringBuilder& stringBuilder)
		{
			buffer         = stringBuilder.buffer;

			return *this;
		}

		Bool operator == (const _StringBuilder& stringBuilder) const
		{
			size_t bufferLength;

			if ((bufferLength = buffer.GetLength()) != stringBuilder.GetLength())
			{

				return False;
			}

			if (!memcmp(buffer.GetCString(), stringBuilder.buffer.GetCString(), bufferLength * sizeof(Char)))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const _StringBuilder& stringBuilder) const
		{
			if (operator==(stringBuilder))
			{

				return False;
			}

			return True;
		}
	};

	typedef _StringBuilder<String::Char>  StringBuilder;
	typedef _StringBuilder<WString::Char> WStringBuilder;
}
