#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"

namespace AL::Collections
{
	template<Endians ENDIAN>
	class ByteBuffer
	{
		Array<uint8> buffer;
		size_t readPosition = 0;
		size_t writePosition = 0;

	public:
		ByteBuffer()
			: ByteBuffer(
				0
			)
		{
		}

		ByteBuffer(ByteBuffer&& buffer)
			: buffer(
				Move(buffer.buffer)
			),
			readPosition(
				buffer.readPosition
			),
			writePosition(
				buffer.writePosition
			)
		{
			buffer.readPosition = 0;
			buffer.writePosition = 0;
		}

		ByteBuffer(const ByteBuffer& buffer)
			: buffer(
				buffer.buffer
			),
			readPosition(
				buffer.readPosition
			),
			writePosition(
				buffer.writePosition
			)
		{
		}

		explicit ByteBuffer(size_t capacity)
			: buffer(
				capacity
			)
		{
		}

		ByteBuffer(const void* lpBuffer, size_t size)
			: buffer(
				static_cast<const uint8*>(lpBuffer),
				size
			),
			writePosition(
				size
			)
		{
		}

		virtual ~ByteBuffer()
		{
		}

		auto GetSize() const
		{
			return writePosition;
		}

		auto GetEndian() const
		{
			return ENDIAN;
		}

		auto GetCapacity() const
		{
			return buffer.GetSize();
		}

		auto GetReadPosition() const
		{
			return readPosition;
		}

		auto GetWritePosition() const
		{
			return writePosition;
		}

		void SetCapacity(size_t value)
		{
			if (readPosition > value)
			{

				readPosition = value;
			}

			if (writePosition > value)
			{

				writePosition = value;
			}

			buffer.SetSize(
				value
			);
		}

		void SetReadPosition(size_t value)
		{
			if ((readPosition = value) > GetCapacity())
			{
				readPosition = GetCapacity();
			}
		}

		void SetWritePosition(size_t value)
		{
			if ((writePosition = value) > GetCapacity())
			{
				writePosition = GetCapacity();
			}
		}

		template<typename T>
		bool PeekEnum(T& value) const
		{
			if (Peek(&value, sizeof(T)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool PeekBool(bool& value) const
		{
			return Peek(
				&value,
				sizeof(bool)
			);
		}

		bool PeekInt8(int8& value) const
		{
			if (Peek(&value, sizeof(int8)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekInt16(int16& value) const
		{
			if (Peek(&value, sizeof(int16)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekInt32(int32& value) const
		{
			if (Peek(&value, sizeof(int32)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekInt64(int64& value) const
		{
			if (Peek(&value, sizeof(int64)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool PeekUInt8(uint8& value) const
		{
			if (Peek(&value, sizeof(uint8)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekUInt16(uint16& value) const
		{
			if (Peek(&value, sizeof(uint16)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekUInt32(uint32& value) const
		{
			if (Peek(&value, sizeof(uint32)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekUInt64(uint64& value) const
		{
			if (Peek(&value, sizeof(uint64)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		
		bool PeekFloat(float& value) const
		{
			if (Peek(&value, sizeof(float)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool PeekDouble(double& value) const
		{
			if (Peek(&value, sizeof(double)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool PeekString(String& value) const
		{
			uint32 length;

			if (PeekUInt32(length))
			{
				String _value(
					'\0',
					length
				);

				auto readPosition = GetReadPosition() + sizeof(length);

				if (Peek(&_value[0], length * sizeof(String::Char), readPosition))
				{
					value = Move(
						_value
					);

					return true;
				}
			}

			return false;
		}
		bool PeekWString(WString& value) const
		{
			uint32 length;

			if (PeekUInt32(length))
			{
				WString _value(
					L'\0',
					length
				);

				auto readPosition = GetReadPosition() + sizeof(length);

				if (Peek(&_value[0], length * sizeof(WString::Char), readPosition))
				{
					value = Move(
						_value
					);

					return true;
				}
			}

			return false;
		}

		bool Peek(void* lpBuffer, size_t size) const
		{
			return Peek(
				lpBuffer,
				size,
				GetReadPosition()
			);
		}
		bool Peek(void* lpBuffer, size_t size, size_t index) const
		{
			if ((index + size) <= GetCapacity())
			{
				memcpy(
					lpBuffer,
					&buffer[index],
					size
				);

				return true;
			}

			return false;
		}

		template<typename T>
		bool ReadEnum(T& value)
		{
			if (Read(&value, sizeof(T)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool ReadBool(bool& value)
		{
			return Read(
				&value,
				sizeof(bool)
			);
		}

		bool ReadInt8(int8& value)
		{
			if (Read(&value, sizeof(int8)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadInt16(int16& value)
		{
			if (Read(&value, sizeof(int16)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadInt32(int32& value)
		{
			if (Read(&value, sizeof(int32)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadInt64(int64& value)
		{
			if (Read(&value, sizeof(int64)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool ReadUInt8(uint8& value)
		{
			if (Read(&value, sizeof(uint8)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadUInt16(uint16& value)
		{
			if (Read(&value, sizeof(uint16)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadUInt32(uint32& value)
		{
			if (Read(&value, sizeof(uint32)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadUInt64(uint64& value)
		{
			if (Read(&value, sizeof(uint64)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		
		bool ReadFloat(float& value)
		{
			if (Read(&value, sizeof(float)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}
		bool ReadDouble(double& value)
		{
			if (Read(&value, sizeof(double)))
			{
				value = FromEndian(
					value
				);

				return true;
			}

			return false;
		}

		bool ReadString(String& value)
		{
			uint32 length;

			auto readPosition = GetReadPosition();

			if (ReadUInt32(length))
			{
				String _value(
					'\0',
					length
				);

				if (Read(&_value[0], length * sizeof(String::Char)))
				{
					value = Move(
						_value
					);

					return true;
				}

				SetReadPosition(
					readPosition
				);
			}

			return false;
		}
		bool ReadWString(WString& value)
		{
			uint32 length;

			auto readPosition = GetReadPosition();

			if (ReadUInt32(length))
			{
				WString _value(
					L'\0',
					length
				);

				if (Read(&_value[0], length * sizeof(WString::Char)))
				{
					value = Move(
						_value
					);

					return true;
				}

				SetReadPosition(
					readPosition
				);
			}

			return false;
		}

		bool Read(void* lpBuffer, size_t size)
		{
			if (size && ((GetReadPosition() + size) <= GetCapacity()))
			{
				memcpy(
					lpBuffer,
					&buffer[readPosition],
					size
				);

				readPosition += size;

				return true;
			}

			return size == 0;
		}

		template<typename T>
		void WriteEnum(T value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(T)
			);
		}

		void WriteBool(bool value)
		{
			Write(
				&value,
				sizeof(bool)
			);
		}

		void WriteInt8(int8 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(int8)
			);
		}
		void WriteInt16(int16 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(int16)
			);
		}
		void WriteInt32(int32 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(int32)
			);
		}
		void WriteInt64(int64 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(int64)
			);
		}
		
		void WriteUInt8(uint8 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(uint8)
			);
		}
		void WriteUInt16(uint16 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(uint16)
			);
		}
		void WriteUInt32(uint32 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(uint32)
			);
		}
		void WriteUInt64(uint64 value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(uint64)
			);
		}

		void WriteFloat(float value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(float)
			);
		}
		void WriteDouble(double value)
		{
			value = ToEndian(
				value
			);

			Write(
				&value,
				sizeof(double)
			);
		}

		void WriteString(const String& value)
		{
			auto length = static_cast<uint32>(
				value.GetLength()
			);

			WriteUInt32(
				length
			);
			
			Write(
				&value[0],
				length * sizeof(String::Char)
			);
		}
		void WriteWString(const WString& value)
		{
			auto length = static_cast<uint32>(
				value.GetLength()
			);

			WriteUInt32(
				length
			);

			Write(
				&value[0],
				length * sizeof(WString::Char)
			);
		}

		void Write(const void* lpBuffer, size_t size)
		{
			if (size)
			{
				if ((writePosition += size) >= GetCapacity())
				{
					buffer.SetSize(
						writePosition
					);
				}

				memcpy(
					&buffer[writePosition - size],
					lpBuffer,
					size
				);
			}
		}

		template<typename T>
		static constexpr T ToEndian(T value);

		template<typename T>
		static constexpr T FromEndian(T value);
		
		uint8& operator [] (size_t index)
		{
			return buffer[index];
		}
		const uint8& operator [] (size_t index) const
		{
			return buffer[index];
		}

		auto& operator = (ByteBuffer&& buffer)
		{
			this->buffer = Move(buffer.buffer);
			readPosition = buffer.readPosition;
			writePosition = buffer.writePosition;

			buffer.readPosition = 0;
			buffer.writePosition = 0;

			return *this;
		}
		auto& operator = (const ByteBuffer& buffer)
		{
			this->buffer = buffer.buffer;
			readPosition = buffer.readPosition;
			writePosition = buffer.writePosition;

			return *this;
		}

		bool operator == (const ByteBuffer& buffer) const
		{
			return this->buffer == buffer.buffer;
		}
		bool operator != (const ByteBuffer& buffer) const
		{
			return !operator==(buffer);
		}
	};
}

template<>
template<typename T>
inline constexpr T AL::Collections::ByteBuffer<AL::Endians::Big>::ToEndian(T value)
{
	return BitConverter::ToBigEndian(
		value
	);
}
template<>
template<typename T>
inline constexpr T AL::Collections::ByteBuffer<AL::Endians::Little>::ToEndian(T value)
{
	return BitConverter::ToLittleEndian(
		value
	);
}

template<>
template<typename T>
inline constexpr T AL::Collections::ByteBuffer<AL::Endians::Big>::FromEndian(T value)
{
	return BitConverter::FromBigEndian(
		value
	);
}
template<>
template<typename T>
inline constexpr T AL::Collections::ByteBuffer<AL::Endians::Little>::FromEndian(T value)
{
	return BitConverter::FromLittleEndian(
		value
	);
}
