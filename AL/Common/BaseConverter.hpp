#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

namespace AL
{
	typedef Collections::Array<uint8> BaseConverterBuffer;

	template<size_t BASE>
	class _BaseConverter_Converter;
	template<>
	class _BaseConverter_Converter<16>
	{
		static constexpr char HEX_TABLE[16] =
		{
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F'
		};

	public:
		template<typename T>
		static String Encode(const T& value)
		{
			return Encode(
				&value,
				sizeof(T)
			);
		}
		static String Encode(const Void* lpBuffer, size_t size)
		{
			String value(
				String::END,
				size * 2
			);

			auto lpBufferBegin = static_cast<const uint8*>(
				lpBuffer
			);

			auto lpBufferEnd = lpBufferBegin + size;

			auto lpSource = lpBufferBegin;
			auto lpDestination = &value[0];

			while (lpSource != lpBufferEnd)
			{
				lpDestination[0] = HEX_TABLE[*lpSource >> 4];
				lpDestination[1] = HEX_TABLE[*lpSource & 0x0F];

				++lpSource;
				lpDestination += 2;
			}

			return value;
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			BaseConverterBuffer buffer;
			Decode(buffer, string);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string)
		{
			auto length = string.GetLength();

			if ((length % 2) != 0)
			{

				return False;
			}

			buffer.SetCapacity(
				length / 2
			);

			auto try_get_byte = [](uint8& _value, const char* _lpString, size_t _offset)
			{
				auto c1 = _lpString[_offset];
				auto c2 = _lpString[_offset + 1];

				auto is_char_valid = [](char _value, Bool& _isDigit)
				{
					return
						(_isDigit = ((_value >= '0') && (_value <= '9'))) ||
						((_value >= 'A') && (_value <= 'F'));
				};

				Bool c1_IsDigit;
				Bool c2_IsDigit;

				if (!is_char_valid(c1, c1_IsDigit) ||
					!is_char_valid(c2, c2_IsDigit))
				{

					return False;
				}

				auto char_to_hex_table_index = [](char _c, Bool _isDigit)
				{
					return static_cast<uint8>(
						_isDigit ? (_c - '0') : ((_c - 'A') + 10)
					);
				};

				auto b1 = char_to_hex_table_index(
					c1,
					c1_IsDigit
				);
				
				auto b2 = char_to_hex_table_index(
					c2,
					c2_IsDigit
				);

				_value = (b1 << 4) | b2;

				return True;
			};

			auto lpBuffer = &buffer[0];
			auto lpSource = string.GetCString();

			for (size_t i = 0; i < length; i += 2, ++lpBuffer)
			{
				if (!try_get_byte(*lpBuffer, lpSource, i))
				{

					return False;
				}
			}

			return True;
		}
	};
	template<>
	class _BaseConverter_Converter<32>
	{
	public:
		template<typename T>
		static String Encode(const T& value);
		static String Encode(const Void* lpBuffer, size_t size);

		template<typename T>
		static T    Decode(const String& string);
		static Bool Decode(BaseConverterBuffer& buffer, const String& string);
	};
	template<>
	class _BaseConverter_Converter<64>
	{
	public:
		template<typename T>
		static String Encode(const T& value);
		static String Encode(const Void* lpBuffer, size_t size);

		template<typename T>
		static T    Decode(const String& string);
		static Bool Decode(BaseConverterBuffer& buffer, const String& string);
	};

	class BaseConverter
	{
		BaseConverter() = delete;

	public:
		typedef BaseConverterBuffer Buffer;

		template<typename T>
		static String ToBase16(const T& value)
		{
			return _BaseConverter_Converter<16>::Encode(
				value
			);
		}
		static String ToBase16(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<16>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase16(const String& string)
		{
			return _BaseConverter_Converter<16>::Decode<T>(
				string
			);
		}
		static Bool FromBase16(Buffer& buffer, const String& string)
		{
			if (!_BaseConverter_Converter<16>::Decode(buffer, string))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase32(const T& value)
		{
			return _BaseConverter_Converter<32>::Encode(
				value
			);
		}
		static String ToBase32(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<32>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase32(const String& string)
		{
			return _BaseConverter_Converter<32>::Decode<T>(
				string
			);
		}
		static Bool FromBase32(Buffer& buffer, const String& string)
		{
			if (!_BaseConverter_Converter<32>::Decode(buffer, string))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase64(const T& value)
		{
			return _BaseConverter_Converter<64>::Encode(
				value
			);
		}
		static String ToBase64(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter_Converter<64>::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase64(const String& string)
		{
			return _BaseConverter_Converter<64>::Decode<T>(
				string
			);
		}
		static Bool FromBase64(Buffer& buffer, const String& string)
		{
			if (!_BaseConverter_Converter<64>::Decode(buffer, string))
			{

				return False;
			}

			return True;
		}
	};
}
