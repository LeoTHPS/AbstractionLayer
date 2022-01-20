#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"

namespace AL::Collections
{
	template<Endians ENDIAN>
	class ByteBuffer
	{
		Array<uint8> buffer;
		const Void*  lpBuffer_R          = nullptr;
		Void*        lpBuffer_W          = nullptr;
		size_t       bufferIndex_R       = 0;
		size_t       bufferIndex_W       = 0;
		size_t       bufferCapacity      = 0;
		size_t       bufferLastReadSize  = 0;
		size_t       bufferLastWriteSize = 0;

		ByteBuffer(const Void* lpBuffer_R, Void* lpBuffer_W, size_t capacity)
			: lpBuffer_R(
				lpBuffer_R
			),
			lpBuffer_W(
				lpBuffer_W
			),
			bufferCapacity(
				capacity
			)
		{
		}

	public:
		static constexpr Bool IsBigEndian()
		{
			return ENDIAN == Endians::Big;
		}

		static constexpr Bool IsLittleEndian()
		{
			return ENDIAN == Endians::Little;
		}

		static constexpr Bool IsMachineEndian()
		{
			return ENDIAN == Endians::Machine;
		}

		static constexpr Endians GetEndian()
		{
			return ENDIAN;
		}

		template<typename T>
		static constexpr T ToEndian(T value)
		{
			if constexpr (IsBigEndian())
			{

				return BitConverter::ToBigEndian(
					value
				);
			}
			else if constexpr (IsLittleEndian())
			{

				return BitConverter::ToLittleEndian(
					value
				);
			}
		}

		template<typename T>
		static constexpr T FromEndian(T value)
		{
			if constexpr (IsBigEndian())
			{

				return BitConverter::FromBigEndian(
					value
				);
			}
			else if constexpr (IsLittleEndian())
			{

				return BitConverter::FromLittleEndian(
					value
				);
			}
		}

		static ByteBuffer CreateWriter(Void* lpBuffer, size_t size)
		{
			ByteBuffer buffer(
				lpBuffer,
				lpBuffer,
				size
			);

			return buffer;
		}

		static ByteBuffer CreateReader(const Void* lpBuffer, size_t size)
		{
			ByteBuffer buffer(
				lpBuffer,
				nullptr,
				size
			);

			return buffer;
		}

		ByteBuffer(ByteBuffer&& byteBuffer)
			: buffer(
				Move(byteBuffer.buffer)
			),
			lpBuffer_R(
				byteBuffer.lpBuffer_R
			),
			lpBuffer_W(
				byteBuffer.lpBuffer_W
			),
			bufferIndex_R(
				byteBuffer.bufferIndex_R
			),
			bufferIndex_W(
				byteBuffer.bufferIndex_W
			),
			bufferCapacity(
				byteBuffer.bufferCapacity
			),
			bufferLastReadSize(
				byteBuffer.bufferLastReadSize
			),
			bufferLastWriteSize(
				byteBuffer.bufferLastWriteSize
			)
		{
			byteBuffer.lpBuffer_R = nullptr;
			byteBuffer.lpBuffer_W = nullptr;
			byteBuffer.bufferIndex_R = 0;
			byteBuffer.bufferIndex_W = 0;
			byteBuffer.bufferCapacity = 0;
			byteBuffer.bufferLastReadSize = 0;
			byteBuffer.bufferLastWriteSize = 0;
		}
		ByteBuffer(const ByteBuffer& byteBuffer)
			: buffer(
				byteBuffer.buffer
			),
			lpBuffer_R(
				byteBuffer.IsSelfHosted() ? &buffer[0] : byteBuffer.lpBuffer_R
			),
			lpBuffer_W(
				!byteBuffer.IsReadOnly() ? (byteBuffer.IsSelfHosted() ? &buffer[0] : byteBuffer.lpBuffer_W) : nullptr
			),
			bufferIndex_R(
				byteBuffer.bufferIndex_R
			),
			bufferIndex_W(
				byteBuffer.bufferIndex_W
			),
			bufferCapacity(
				byteBuffer.bufferCapacity
			),
			bufferLastReadSize(
				byteBuffer.bufferLastReadSize
			),
			bufferLastWriteSize(
				byteBuffer.bufferLastWriteSize
			)
		{
		}

		explicit ByteBuffer(size_t capacity)
			: buffer(
				capacity
			),
			lpBuffer_R(
				&buffer[0]
			),
			lpBuffer_W(
				&buffer[0]
			),
			bufferCapacity(
				capacity
			)
		{
		}

		ByteBuffer(const Void* lpBuffer, size_t size)
			: buffer(
				reinterpret_cast<const uint8*>(lpBuffer),
				size
			),
			lpBuffer_R(
				&buffer[0]
			),
			lpBuffer_W(
				&buffer[0]
			),
			bufferIndex_W(
				size
			),
			bufferCapacity(
				size
			)
		{
		}

		virtual ~ByteBuffer()
		{
		}

		Bool IsReadOnly() const
		{
			return lpBuffer_R != lpBuffer_W;
		}

		Bool IsSelfHosted() const
		{
			return lpBuffer_R != &buffer[0];
		}

		auto GetBuffer() const
		{
			return lpBuffer_R;
		}

		auto GetCapacity() const
		{
			return bufferCapacity;
		}

		auto GetReadPosition() const
		{
			return bufferIndex_R;
		}

		auto GetWritePosition() const
		{
			return bufferIndex_W;
		}

		auto GetLastReadSize() const
		{
			return bufferLastReadSize;
		}

		auto GetLastWriteSize() const
		{
			return bufferLastWriteSize;
		}

		Void Clear()
		{
			SetReadPosition(0);
			SetWritePosition(0);
		}

		Bool PopLastRead()
		{
			auto lastReadSize = GetLastReadSize();

			if (lastReadSize == 0)
			{

				return False;
			}

			bufferIndex_R -= lastReadSize;

			bufferLastReadSize = 0;

			return True;
		}

		Bool PopLastWrite()
		{
			auto lastWriteSize = GetLastWriteSize();

			if (lastWriteSize == 0)
			{

				return False;
			}

			bufferIndex_W -= lastWriteSize;

			bufferLastWriteSize = 0;

			return True;
		}

		Bool SetCapacity(size_t value, Bool retainValues = True)
		{
			if (!IsSelfHosted())
			{

				return False;
			}

			if (retainValues)
			{
				buffer.SetSize(
					value
				);
			}
			else
			{
				buffer.SetCapacity(
					value
				);
			}

			lpBuffer_R = &buffer[0];
			lpBuffer_W = &buffer[0];

			return True;
		}

		Void SetReadPosition(size_t value)
		{
			if ((bufferIndex_R = value) > GetCapacity())
			{

				bufferIndex_R = GetCapacity();
			}
		}

		Bool SetWritePosition(size_t value)
		{
			if (IsReadOnly())
			{

				return False;
			}

			size_t capacity;

			if ((bufferIndex_W = value) > (capacity = GetCapacity()))
			{

				bufferIndex_W = capacity;
			}

			return True;
		}

		Bool ReadBool(Bool& value)
		{
			if (!Read(&value, sizeof(Bool)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}

		template<typename T>
		Bool ReadEnum(T& value)
		{
			if (!Read(&value, sizeof(T)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}

		Bool ReadInt8(int8& value)
		{
			if (!Read(&value, sizeof(int8)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadInt16(int16& value)
		{
			if (!Read(&value, sizeof(int16)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadInt32(int32& value)
		{
			if (!Read(&value, sizeof(int32)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadInt64(int64& value)
		{
			if (!Read(&value, sizeof(int64)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}

		Bool ReadUInt8(uint8& value)
		{
			if (!Read(&value, sizeof(uint8)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadUInt16(uint16& value)
		{
			if (!Read(&value, sizeof(uint16)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadUInt32(uint32& value)
		{
			if (!Read(&value, sizeof(uint32)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadUInt64(uint64& value)
		{
			if (!Read(&value, sizeof(uint64)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}

		Bool ReadFloat(Float& value)
		{
			if (!Read(&value, sizeof(Float)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}
		Bool ReadDouble(Double& value)
		{
			if (!Read(&value, sizeof(Double)))
			{

				return False;
			}

			value = FromEndian(
				value
			);

			return True;
		}

		Bool ReadString(String& value)
		{
			uint32 length;

			if (!ReadUInt32(length))
			{

				return False;
			}

			Array<String::Char> buffer(
				length
			);

			if (!Read(&buffer[0], length * sizeof(String::Char)))
			{
				PopLastRead();

				return False;
			}

			value.Assign(
				&buffer[0],
				length
			);

			return True;
		}
		Bool ReadWString(WString& value)
		{
			uint32 length;

			if (!ReadUInt32(length))
			{

				return False;
			}

			Array<WString::Char> buffer(
				length
			);

			if (!Read(&buffer[0], length * sizeof(WString::Char)))
			{
				PopLastRead();

				return False;
			}

			value.Assign(
				&buffer[0],
				length
			);

			return True;
		}

		template<typename T>
		inline Bool Read(T& value)
		{
			if constexpr (Is_Type<T, Bool>::Value)
			{
				if (!ReadBool(value))
				{

					return False;
				}

				return True;
			}
			else if constexpr (Is_Enum<T>::Value)
			{
				if (!ReadEnum(value))
				{

					return False;
				}

				return True;
			}
			else if constexpr (Is_Integer<T>::Value)
			{
				if constexpr (Is_Type<T, int8>::Value)
				{
					if (!ReadInt8(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int16>::Value)
				{
					if (!ReadInt16(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int32>::Value)
				{
					if (!ReadInt32(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int64>::Value)
				{
					if (!ReadInt64(value))
					{

						return False;
					}

					return True;
				}

				if constexpr (Is_Type<T, uint8>::Value)
				{
					if (!ReadUInt8(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint16>::Value)
				{
					if (!ReadUInt16(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint32>::Value)
				{
					if (!ReadUInt32(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint64>::Value)
				{
					if (!ReadUInt64(value))
					{

						return False;
					}

					return True;
				}
			}
			else if constexpr (Is_Decimal<T>::Value)
			{
				if constexpr (Is_Float<T>::Value)
				{
					if (!ReadFloat(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Double<T>::Value)
				{
					if (!ReadDouble(value))
					{

						return False;
					}

					return True;
				}
			}
		}
		inline Bool Read(String& value)
		{
			if (!ReadString(value))
			{

				return False;
			}

			return True;
		}
		Bool        Read(Void* lpBuffer, size_t size)
		{
			auto readPosition    = GetReadPosition();
			auto newReadPosition = readPosition + size;

			if (newReadPosition > GetCapacity())
			{

				return False;
			}

			memcpy(
				lpBuffer,
				&reinterpret_cast<const uint8*>(lpBuffer_R)[readPosition],
				size
			);

			bufferIndex_R = newReadPosition;

			bufferLastReadSize = size;

			return True;
		}

		Bool WriteBool(Bool value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(Bool)))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		Bool WriteEnum(T value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(T)))
			{

				return False;
			}

			return True;
		}

		Bool WriteInt8(int8 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(int8)))
			{

				return False;
			}

			return True;
		}
		Bool WriteInt16(int16 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(int16)))
			{

				return False;
			}

			return True;
		}
		Bool WriteInt32(int32 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(int32)))
			{

				return False;
			}

			return True;
		}
		Bool WriteInt64(int64 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(int64)))
			{

				return False;
			}

			return True;
		}

		Bool WriteUInt8(uint8 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(uint8)))
			{

				return False;
			}

			return True;
		}
		Bool WriteUInt16(uint16 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(uint16)))
			{

				return False;
			}

			return True;
		}
		Bool WriteUInt32(uint32 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(uint32)))
			{

				return False;
			}

			return True;
		}
		Bool WriteUInt64(uint64 value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(uint64)))
			{

				return False;
			}

			return True;
		}

		Bool WriteFloat(Float value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(Float)))
			{

				return False;
			}

			return True;
		}
		Bool WriteDouble(Double value)
		{
			value = ToEndian(
				value
			);

			if (!Write(&value, sizeof(Double)))
			{

				return False;
			}

			return True;
		}

		Bool WriteString(const String& value)
		{
			auto length = value.GetLength();

			if constexpr (!Is_Type<size_t, uint32>::Value)
			{
				if (length > Integer<uint32>::Maximum)
				{
					length = Integer<uint32>::Maximum;
				}
			}

			if (!WriteUInt32(static_cast<uint32>(length)))
			{

				return False;
			}

			if (!Write(value.GetCString(), length))
			{
				PopLastWrite();

				return False;
			}

			return True;
		}
		Bool WriteWString(const WString& value)
		{
			auto length = value.GetLength();

			if constexpr (!Is_Type<size_t, uint32>::Value)
			{
				if (length > Integer<uint32>::Maximum)
				{
					length = Integer<uint32>::Maximum;
				}
			}

			if (!WriteUInt32(static_cast<uint32>(length)))
			{

				return False;
			}

			if (!Write(value.GetCString(), length * sizeof(WString::Char)))
			{
				PopLastWrite();

				return False;
			}

			return True;
		}

		template<typename T>
		inline Bool Write(T value)
		{
			if constexpr (Is_Type<T, Bool>::Value)
			{
				if (!WriteBool(value))
				{

					return False;
				}

				return True;
			}
			else if constexpr (Is_Enum<T>::Value)
			{
				if (!WriteEnum(value))
				{

					return False;
				}

				return True;
			}
			else if constexpr (Is_Integer<T>::Value)
			{
				if constexpr (Is_Type<T, int8>::Value)
				{
					if (!WriteInt8(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int16>::Value)
				{
					if (!WriteInt16(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int32>::Value)
				{
					if (!WriteInt32(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, int64>::Value)
				{
					if (!WriteInt64(value))
					{

						return False;
					}

					return True;
				}

				if constexpr (Is_Type<T, uint8>::Value)
				{
					if (!WriteUInt8(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint16>::Value)
				{
					if (!WriteUInt16(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint32>::Value)
				{
					if (!WriteUInt32(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Type<T, uint64>::Value)
				{
					if (!WriteUInt64(value))
					{

						return False;
					}

					return True;
				}
			}
			else if constexpr (Is_Decimal<T>::Value)
			{
				if constexpr (Is_Float<T>::Value)
				{
					if (!WriteFloat(value))
					{

						return False;
					}

					return True;
				}
				else if constexpr (Is_Double<T>::Value)
				{
					if (!WriteDouble(value))
					{

						return False;
					}

					return True;
				}
			}
		}
		inline Bool Write(const String& value)
		{
			if (!WriteString(value))
			{

				return False;
			}

			return True;
		}
		Bool        Write(const Void* lpBuffer, size_t size)
		{
			if (IsReadOnly())
			{

				return False;
			}

			auto writePosition    = GetWritePosition();
			auto newWritePosition = writePosition + size;

			if (newWritePosition > GetCapacity())
			{

				return False;
			}

			memcpy(
				&reinterpret_cast<uint8*>(lpBuffer_W)[writePosition],
				lpBuffer,
				size
			);

			bufferIndex_W = newWritePosition;

			bufferLastWriteSize = size;

			return True;
		}

		ByteBuffer& operator = (ByteBuffer&& byteBuffer)
		{
			buffer = Move(
				byteBuffer.buffer
			);

			lpBuffer_R = byteBuffer.lpBuffer_R;
			byteBuffer.lpBuffer_R = nullptr;

			lpBuffer_W = byteBuffer.lpBuffer_W;
			byteBuffer.lpBuffer_W = nullptr;

			bufferIndex_R = byteBuffer.bufferIndex_R;
			byteBuffer.bufferIndex_R = 0;

			bufferIndex_W = byteBuffer.bufferIndex_W;
			byteBuffer.bufferIndex_W = 0;

			bufferCapacity = byteBuffer.bufferCapacity;
			byteBuffer.bufferCapacity = 0;

			bufferLastReadSize = byteBuffer.bufferLastReadSize;
			byteBuffer.bufferLastReadSize = 0;

			bufferLastWriteSize = byteBuffer.bufferLastWriteSize;
			byteBuffer.bufferLastWriteSize = 0;

			return *this;
		}
		ByteBuffer& operator = (const ByteBuffer& byteBuffer)
		{
			buffer = byteBuffer.buffer;
			lpBuffer_R = byteBuffer.IsSelfHosted() ? &buffer[0] : byteBuffer.lpBuffer_R;
			lpBuffer_W = !byteBuffer.IsReadOnly() ? (byteBuffer.IsSelfHosted() ? &buffer[0] : byteBuffer.lpBuffer_W) : nullptr;
			bufferIndex_R = byteBuffer.bufferIndex_R;
			bufferIndex_W = byteBuffer.bufferIndex_W;
			bufferCapacity = byteBuffer.bufferCapacity;
			bufferLastReadSize = byteBuffer.bufferLastReadSize;
			bufferLastWriteSize = byteBuffer.bufferLastWriteSize;

			return *this;
		}

		Bool operator == (const ByteBuffer& byteBuffer) const
		{
			size_t capacity;

			if ((capacity = GetCapacity()) != byteBuffer.GetCapacity())
			{

				return False;
			}

			if (!memcmp(GetBuffer(), byteBuffer.GetBuffer(), capacity))
			{

				return False;
			}
			
			return True;
		}
		Bool operator != (const ByteBuffer& byteBuffer) const
		{
			if (operator==(byteBuffer))
			{

				return False;
			}

			return True;
		}
	};
}
