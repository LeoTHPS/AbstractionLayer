#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::Collections
{
	public ref class ByteBuffer
	{
		Endians endian;

		array<::System::Byte>^ buffer;

		::System::UInt32 readPosition = 0;
		::System::UInt32 writePosition = 0;

	public:
		ByteBuffer()
			: ByteBuffer(
				Endians::Machine
			)
		{
		}

		explicit ByteBuffer(Endians endian)
			: ByteBuffer(
				endian,
				0
			)
		{
		}
		
		ByteBuffer(Endians endian, ::System::UInt32 capacity)
			: ByteBuffer(
				endian,
				gcnew array<::System::Byte>(
					capacity
				),
				0,
				capacity
			)
		{
		}

		ByteBuffer(Endians endian, array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
			: endian(
				endian
			),
			buffer(
				CreateBuffer(
					buffer,
					offset,
					count
				)
			)
		{
		}

		virtual ~ByteBuffer()
		{
		}

		auto GetSize()
		{
			return writePosition;
		}

		auto GetEndian()
		{
			return endian;
		}

		auto GetCapacity()
		{
			return static_cast<::System::UInt32>(
				buffer->Length
			);
		}

		auto GetReadPosition()
		{
			return readPosition;
		}

		auto GetWritePosition()
		{
			return writePosition;
		}

		void SetCapacity(::System::UInt32 value)
		{
			if (readPosition > value)
			{

				readPosition = value;
			}

			if (writePosition > value)
			{

				writePosition = value;
			}

			::System::Array::Resize(
				buffer,
				value
			);
		}

		void SetReadPosition(::System::UInt32 value)
		{
			if ((readPosition = value) > GetCapacity())
			{
				readPosition = GetCapacity();
			}
		}

		void SetWritePosition(::System::UInt32 value)
		{
			if ((writePosition = value) > GetCapacity())
			{
				writePosition = GetCapacity();
			}
		}

		generic<typename T>
		bool PeekEnum([::System::Runtime::InteropServices::OutAttribute] T% value)
		{
			if (!T::typeid->IsEnum)
			{

				throw gcnew ::System::ArgumentException(
					"T is not an Enum"
				);
			}

			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<T>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool PeekBool([::System::Runtime::InteropServices::OutAttribute] ::System::Boolean% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Boolean>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool PeekInt8([::System::Runtime::InteropServices::OutAttribute] ::System::SByte% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::SByte>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekInt16([::System::Runtime::InteropServices::OutAttribute] ::System::Int16% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int16>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekInt32([::System::Runtime::InteropServices::OutAttribute] ::System::Int32% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int32>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekInt64([::System::Runtime::InteropServices::OutAttribute] ::System::Int64% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int64>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool PeekUInt8([::System::Runtime::InteropServices::OutAttribute] ::System::Byte% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Byte>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekUInt16([::System::Runtime::InteropServices::OutAttribute] ::System::UInt16% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt16>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekUInt32([::System::Runtime::InteropServices::OutAttribute] ::System::UInt32% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt32>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekUInt64([::System::Runtime::InteropServices::OutAttribute] ::System::UInt64% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt64>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		
		bool PeekFloat([::System::Runtime::InteropServices::OutAttribute] ::System::Single% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Single>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool PeekDouble([::System::Runtime::InteropServices::OutAttribute] ::System::Double% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Double>()
			);

			if (Peek(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool PeekString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			return PeekString(
				value,
				::System::Text::Encoding::Default
			);
		}
		bool PeekString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value, ::System::Text::Encoding^ encoding)
		{
			::System::UInt32 length;

			if (PeekUInt32(length))
			{
				::System::UInt32 readPosition = GetReadPosition() + Marshal::SizeOf<::System::UInt32>();

				if (readPosition < GetSize())
				{
					value = encoding->GetString(
						buffer,
						readPosition,
						length
					);

					return true;
				}
			}

			return false;
		}

		bool Peek(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			return Peek(
				buffer,
				offset,
				count,
				GetReadPosition()
			);
		}
		bool Peek(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count, ::System::UInt32 index)
		{
			if ((index + count) <= GetCapacity())
			{
				::System::Buffer::BlockCopy(
					this->buffer,
					index,
					buffer,
					offset,
					count
				);

				return true;
			}

			return false;
		}

		generic<typename T>
		bool ReadEnum([::System::Runtime::InteropServices::OutAttribute] T% value)
		{
			if (!T::typeid->IsEnum)
			{

				throw gcnew ::System::ArgumentException(
					"T is not an Enum"
				);
			}

			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<T>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool ReadBool([::System::Runtime::InteropServices::OutAttribute] ::System::Boolean% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Boolean>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool ReadInt8([::System::Runtime::InteropServices::OutAttribute] ::System::SByte% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::SByte>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadInt16([::System::Runtime::InteropServices::OutAttribute] ::System::Int16% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int16>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadInt32([::System::Runtime::InteropServices::OutAttribute] ::System::Int32% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int32>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadInt64([::System::Runtime::InteropServices::OutAttribute] ::System::Int64% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Int64>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool ReadUInt8([::System::Runtime::InteropServices::OutAttribute] ::System::Byte% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Byte>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadUInt16([::System::Runtime::InteropServices::OutAttribute] ::System::UInt16% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt16>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadUInt32([::System::Runtime::InteropServices::OutAttribute] ::System::UInt32% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt32>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadUInt64([::System::Runtime::InteropServices::OutAttribute] ::System::UInt64% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::UInt64>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		
		bool ReadFloat([::System::Runtime::InteropServices::OutAttribute] ::System::Single% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Single>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}
		bool ReadDouble([::System::Runtime::InteropServices::OutAttribute] ::System::Double% value)
		{
			auto buffer = gcnew array<::System::Byte>(
				Marshal::SizeOf<::System::Double>()
			);

			if (Read(buffer, 0, buffer->Length))
			{
				FromEndian(
					GetEndian(),
					value,
					buffer
				);

				return true;
			}

			return false;
		}

		bool ReadString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			return ReadString(
				value,
				::System::Text::Encoding::Default
			);
		}
		bool ReadString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value, ::System::Text::Encoding^ encoding)
		{
			::System::UInt32 length;

			if (PeekUInt32(length))
			{
				::System::UInt32 readPosition = GetReadPosition() + Marshal::SizeOf<::System::UInt32>();

				if (readPosition < GetSize())
				{
					value = encoding->GetString(
						buffer,
						readPosition,
						length
					);

					auto stringSize = encoding->GetByteCount(
						value
					);

					SetReadPosition(
						readPosition + Marshal::SizeOf<::System::UInt32>() + stringSize
					);

					return true;
				}

				SetReadPosition(
					readPosition - Marshal::SizeOf<::System::UInt32>()
				);
			}

			return false;
		}

		bool Read(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			if (count && ((GetReadPosition() + count) <= GetCapacity()))
			{
				::System::Buffer::BlockCopy(
					this->buffer,
					readPosition,
					buffer,
					offset,
					count
				);

				readPosition += count;

				return true;
			}

			return count == 0;
		}

		generic<typename T>
		void WriteEnum(T value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}

		void WriteBool(::System::Boolean value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}

		void WriteInt8(::System::SByte value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteInt16(::System::Int16 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteInt32(::System::Int32 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteInt64(::System::Int64 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		
		void WriteUInt8(::System::Byte value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteUInt16(::System::UInt16 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteUInt32(::System::UInt32 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteUInt64(::System::UInt64 value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}

		void WriteFloat(::System::Single value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}
		void WriteDouble(::System::Double value)
		{
			array<::System::Byte>^ buffer;
			ToEndian(GetEndian(), buffer, value);
			Write(buffer, 0, buffer->Length);
		}

		void WriteString(::System::String^ value)
		{
			WriteString(
				value,
				::System::Text::Encoding::Default
			);
		}
		void WriteString(::System::String^ value, ::System::Text::Encoding^ encoding)
		{
			auto length = static_cast<::System::UInt32>(
				value->Length
			);

			WriteUInt32(
				length
			);

			auto buffer = encoding->GetBytes(
				value
			);
			
			Write(
				buffer,
				0,
				buffer->Length
			);
		}

		void Write(array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			if (count)
			{
				if ((writePosition += count) >= GetCapacity())
				{
					::System::Array::Resize(
						this->buffer,
						writePosition
					);
				}

				::System::Buffer::BlockCopy(
					buffer,
					offset,
					this->buffer,
					writePosition - count,
					count
				);
			}
		}

		virtual array<::System::Byte>^ ToArray()
		{
			auto _array = gcnew array<::System::Byte>(
				GetSize()
			);

			for (::System::UInt32 i = 0; i < GetSize(); ++i)
			{
				_array[i] = buffer[i];
			}

			return _array;
		}

		virtual ::System::String^ ToString() override
		{
			auto buffer = ToArray();

			auto string = gcnew ::System::String(
				""
			);

			for (::System::Int32 i = 0; i < buffer->Length; ++i)
			{
				string += buffer[i].ToString(
					"X2"
				);
			}

			return string;
		}

		generic<typename T>
		T ToEndian(T value)
		{
			ToEndian(
				GetEndian(),
				value,
				value
			);

			return value;
		}

		generic<typename T>
		T FromEndian(T value)
		{
			FromEndian(
				GetEndian(),
				value,
				value
			);

			return value;
		}

		generic<typename T>
		static void ToEndian(Endians endian, [::System::Runtime::InteropServices::OutAttribute] T% result, T value)
		{
			if (endian == Endians::Machine)
			{
				result = value;

				return;
			}

			array<::System::Byte>^ buffer;
			ToEndian(endian, buffer, value);

			result = Marshal::FromArray<T>(
				buffer,
				0
			);
		}
		generic<typename T>
		static void ToEndian(Endians endian, [::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% result, T value)
		{
			result = Marshal::ToArray<T>(
				value
			);

			if (endian != Endians::Machine)
			{
				::System::Array::Reverse(
					result
				);
			}
		}
		
		generic<typename T>
		static void FromEndian(Endians endian, [::System::Runtime::InteropServices::OutAttribute] T% result, T value)
		{
			if (endian == Endians::Machine)
			{
				result = value;

				return;
			}

			auto buffer = Marshal::ToArray(
				value
			);

			FromEndian(
				endian,
				result,
				buffer
			);
		}
		generic<typename T>
		static void FromEndian(Endians endian, [::System::Runtime::InteropServices::OutAttribute] T% result, array<::System::Byte>^ buffer)
		{
			if (endian != Endians::Machine)
			{

				::System::Array::Reverse(
					buffer
				);
			}

			result = Marshal::FromArray<T>(
				buffer,
				0
			);
		}
		
		static bool operator == (ByteBuffer^ buffer1, ByteBuffer^ buffer2)
		{
			return buffer1->buffer == buffer2->buffer;
		}
		static bool operator != (ByteBuffer^ buffer1, ByteBuffer^ buffer2)
		{
			return !operator==(
				buffer1,
				buffer2
			);
		}

		static array<::System::Byte>^ CreateBuffer(array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			if (buffer->Length != count)
			{
				auto _buffer = gcnew array<::System::Byte>(
					count
				);

				::System::Buffer::BlockCopy(
					buffer,
					offset,
					_buffer,
					0,
					count
				);

				return _buffer;
			}

			return buffer;
		}
	};
}
