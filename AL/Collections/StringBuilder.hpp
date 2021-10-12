#pragma once
#include "AL/Common.hpp"

#include "String.hpp"

namespace AL::Collections
{
	template<typename T_CHAR>
	class _StringBuilder
	{
		_String<T_CHAR> buffer;

	public:
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

		template<typename T>
		_StringBuilder& Append(T value)
		{
			if constexpr (Is_Type<Char, typename String::Char>::Value)
			{
				auto string = AL::ToString<T>(
					Forward<T>(value)
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

			return *this;
		}
		_StringBuilder& Append(Char value)
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
