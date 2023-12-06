#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

namespace AL
{
	typedef Collections::Array<uint8> BaseConverterBuffer;

	template<size_t BIT_COUNT>
	class _BaseConverter;
	template<>
	class _BaseConverter<16>
	{
		static constexpr String::Char CHARACTER_TABLE[16] =
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
				lpDestination[0] = CHARACTER_TABLE[*lpSource >> 4];
				lpDestination[1] = CHARACTER_TABLE[*lpSource & 0x0F];

				++lpSource;
				lpDestination += 2;
			}

			return value;
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			BaseConverterBuffer buffer;
			Decode(buffer, string, offset, length);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}

		static Bool Decode(BaseConverterBuffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length)
		{
			if ((length % 2) != 0)
			{

				return False;
			}

			if ((offset + length) >= string.GetLength())
			{

				return False;
			}

			buffer.SetCapacity(
				length / 2
			);

			auto try_get_byte = [](uint8& _value, const String::Char* _lpString, size_t _offset)
			{
				auto c1 = _lpString[_offset];
				auto c2 = _lpString[_offset + 1];

				auto is_char_valid = [](String::Char _value, Bool& _isDigit)
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

				auto char_to_hex_table_index = [](String::Char _c, Bool _isDigit)
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
			auto lpSource = &string[offset];

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
	class _BaseConverter<32>
	{
		static constexpr String::Char CHARACTER_TABLE[32] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'2', '3', '4', '5', '6', '7'
		};

		static constexpr String::Char CHARACTER_TABLE_HEX[32] =
		{
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'
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
			return Encode(
				lpBuffer,
				size,
				CHARACTER_TABLE
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			BaseConverterBuffer buffer;
			Decode(buffer, string, offset, length);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!Decode(buffer, string, offset, length, CHARACTER_TABLE))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String EncodeHex(const T& value)
		{
			return EncodeHex(
				&value,
				sizeof(T)
			);
		}
		static String EncodeHex(const Void* lpBuffer, size_t size)
		{
			return Encode(
				lpBuffer,
				size,
				CHARACTER_TABLE_HEX
			);
		}

		template<typename T>
		static T    DecodeHex(const String& string)
		{
			return DecodeHex<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    DecodeHex(const String& string, size_t offset, size_t length)
		{
			BaseConverterBuffer buffer;
			DecodeHex(buffer, string, offset, length);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}
		static Bool DecodeHex(BaseConverterBuffer& buffer, const String& string)
		{
			return DecodeHex(
				buffer,
				string,
				0,
				string.GetLength()
			);
		}
		static Bool DecodeHex(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length)
		{
			return Decode(
				buffer,
				string,
				offset,
				length,
				CHARACTER_TABLE_HEX
			);
		}

	private:
		static String Encode(const Void* lpBuffer, size_t size, const String::Char(&table)[32]);
		static Bool   Decode(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length, const String::Char(&table)[32]);
	};
	template<>
	class _BaseConverter<64>
	{
		static constexpr String::Char CHARACTER_TABLE[64] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'e', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'+', '/'
		};

		static constexpr String::Char CHARACTER_TABLE_URL[64] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'e', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'-', '_'
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
			return Encode(
				lpBuffer,
				size,
				CHARACTER_TABLE
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			BaseConverterBuffer buffer;
			Decode(buffer, string, offset, length);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!Decode(buffer, string, offset, length, CHARACTER_TABLE))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String EncodeUrl(const T& value)
		{
			return EncodeUrl(
				&value,
				sizeof(T)
			);
		}
		static String EncodeUrl(const Void* lpBuffer, size_t size)
		{
			return Encode(
				lpBuffer,
				size,
				CHARACTER_TABLE_URL
			);
		}

		template<typename T>
		static T    DecodeUrl(const String& string)
		{
			return DecodeUrl<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    DecodeUrl(const String& string, size_t offset, size_t length)
		{
			BaseConverterBuffer buffer;
			DecodeUrl(buffer, string, offset, length);

			return *reinterpret_cast<T*>(
				&buffer[0]
			);
		}
		static Bool DecodeUrl(BaseConverterBuffer& buffer, const String& string)
		{
			if (!DecodeUrl(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool DecodeUrl(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!Decode(buffer, string, offset, length, CHARACTER_TABLE_URL))
			{

				return False;
			}

			return True;
		}

	private:
		static String Encode(const Void* lpBuffer, size_t size, const String::Char(&table)[64]);
		static Bool   Decode(BaseConverterBuffer& buffer, const String& string, size_t offset, size_t length, const String::Char(&table)[64]);
	};

	class BaseConverter
	{
		typedef _BaseConverter<16> _BaseConverter16;
		typedef _BaseConverter<32> _BaseConverter32;
		typedef _BaseConverter<64> _BaseConverter64;

		BaseConverter() = delete;

	public:
		typedef BaseConverterBuffer Buffer;

		template<typename T>
		static String ToBase16(const T& value)
		{
			return _BaseConverter16::Encode(
				value
			);
		}
		static String ToBase16(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter16::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase16(const String& string)
		{
			return FromBase16<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    FromBase16(const String& string, size_t offset, size_t length)
		{
			return _BaseConverter16::Decode<T>(
				string,
				offset,
				length
			);
		}
		static Bool FromBase16(Buffer& buffer, const String& string)
		{
			if (!FromBase16(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool FromBase16(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!_BaseConverter16::Decode(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase32(const T& value)
		{
			return _BaseConverter32::Encode(
				value
			);
		}
		static String ToBase32(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter32::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase32(const String& string)
		{
			return FromBase32<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    FromBase32(const String& string, size_t offset, size_t length)
		{
			return _BaseConverter32::Decode<T>(
				string,
				offset,
				length
			);
		}
		static Bool FromBase32(Buffer& buffer, const String& string)
		{
			if (!FromBase32(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool FromBase32(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!_BaseConverter32::Decode(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase32Hex(const T& value)
		{
			return _BaseConverter32::EncodeHex(
				value
			);
		}
		static String ToBase32Hex(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter32::EncodeHex(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase32Hex(const String& string)
		{
			return FromBase32Hex<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    FromBase32Hex(const String& string, size_t offset, size_t length)
		{
			return _BaseConverter32::DecodeHex<T>(
				string,
				offset,
				length
			);
		}
		static Bool FromBase32Hex(Buffer& buffer, const String& string)
		{
			if (!FromBase32Hex(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool FromBase32Hex(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!_BaseConverter32::DecodeHex(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase64(const T& value)
		{
			return _BaseConverter64::Encode(
				value
			);
		}
		static String ToBase64(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter64::Encode(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase64(const String& string)
		{
			return FromBase64<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    FromBase64(const String& string, size_t offset, size_t length)
		{
			return _BaseConverter64::Decode<T>(
				string,
				offset,
				length
			);
		}
		static Bool FromBase64(Buffer& buffer, const String& string)
		{
			if (!FromBase64(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool FromBase64(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!_BaseConverter64::Decode(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String ToBase64Url(const T& value)
		{
			return _BaseConverter64::EncodeUrl(
				value
			);
		}
		static String ToBase64Url(const Void* lpBuffer, size_t size)
		{
			return _BaseConverter64::EncodeUrl(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    FromBase64Url(const String& string)
		{
			return FromBase64Url<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    FromBase64Url(const String& string, size_t offset, size_t length)
		{
			return _BaseConverter64::DecodeUrl<T>(
				string,
				offset,
				length
			);
		}
		static Bool FromBase64Url(Buffer& buffer, const String& string)
		{
			if (!FromBase64Url(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool FromBase64Url(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!_BaseConverter64::DecodeUrl(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}
	};

	class BaseConverter16
	{
		BaseConverter16() = delete;

	public:
		typedef typename BaseConverter::Buffer Buffer;

		template<typename T>
		static String Encode(const T& value)
		{
			return BaseConverter::ToBase16(
				value
			);
		}
		static String Encode(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase16(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			return BaseConverter::FromBase16<T>(
				string,
				offset,
				length
			);
		}

		static Bool Decode(Buffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!BaseConverter::FromBase16(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}
	};

	class BaseConverter32
	{
		BaseConverter32() = delete;

	public:
		typedef typename BaseConverter::Buffer Buffer;

		template<typename T>
		static String Encode(const T& value)
		{
			return BaseConverter::ToBase32(
				value
			);
		}
		static String Encode(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase32(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			return BaseConverter::FromBase32<T>(
				string,
				offset,
				length
			);
		}
		static Bool Decode(Buffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!BaseConverter::FromBase32(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String EncodeHex(const T& value)
		{
			return BaseConverter::ToBase32Hex(
				value
			);
		}
		static String EncodeHex(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase32Hex(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    DecodeHex(const String& string)
		{
			return DecodeHex<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    DecodeHex(const String& string, size_t offset, size_t length)
		{
			return BaseConverter::FromBase32Hex<T>(
				string,
				offset,
				length
			);
		}
		static Bool DecodeHex(Buffer& buffer, const String& string)
		{
			if (!DecodeHex(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool DecodeHex(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!BaseConverter::FromBase32Hex(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}
	};

	class BaseConverter64
	{
		BaseConverter64() = delete;

	public:
		typedef typename BaseConverter::Buffer Buffer;

		template<typename T>
		static String Encode(const T& value)
		{
			return BaseConverter::ToBase64(
				value
			);
		}
		static String Encode(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase64(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return Decode<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    Decode(const String& string, size_t offset, size_t length)
		{
			return BaseConverter::FromBase64<T>(
				string,
				offset,
				length
			);
		}
		static Bool Decode(Buffer& buffer, const String& string)
		{
			if (!Decode(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool Decode(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!BaseConverter::FromBase64(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		static String EncodeUrl(const T& value)
		{
			return BaseConverter::ToBase64Url(
				value
			);
		}
		static String EncodeUrl(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase64Url(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    DecodeUrl(const String& string)
		{
			return DecodeUrl<T>(
				string,
				0,
				string.GetLength()
			);
		}
		template<typename T>
		static T    DecodeUrl(const String& string, size_t offset, size_t length)
		{
			return BaseConverter::FromBase64Url<T>(
				string,
				offset,
				length
			);
		}
		static Bool DecodeUrl(Buffer& buffer, const String& string)
		{
			if (!DecodeUrl(buffer, string, 0, string.GetLength()))
			{

				return False;
			}

			return True;
		}
		static Bool DecodeUrl(Buffer& buffer, const String& string, size_t offset, size_t length)
		{
			if (!BaseConverter::FromBase64Url(buffer, string, offset, length))
			{

				return False;
			}

			return True;
		}
	};
}
