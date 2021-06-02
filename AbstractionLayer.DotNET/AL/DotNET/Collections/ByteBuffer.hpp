#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Collections/ByteBuffer.hpp>

namespace AL::DotNET::Collections
{
	public ref class ByteBuffer
	{
		Endians endian;

		AL::Collections::ByteBuffer<AL::Endians::Big>* const lpBuffer_BigEndian;
		AL::Collections::ByteBuffer<AL::Endians::Little>* const lpBuffer_LittleEndian;

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
			: endian(
				endian
			),
			lpBuffer_BigEndian(
				(endian == Endians::Big) ? new AL::Collections::ByteBuffer<AL::Endians::Big>(capacity) : nullptr
			),
			lpBuffer_LittleEndian(
				(endian == Endians::Little) ? new AL::Collections::ByteBuffer<AL::Endians::Little>(capacity) : nullptr
			)
		{
		}

		ByteBuffer(Endians endian, array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
			: ByteBuffer(
				endian,
				count
			)
		{
			switch (endian)
			{
				case Endians::Big:
					Marshal::Copy(buffer, &(*lpBuffer_BigEndian)[0], offset, count);
					lpBuffer_BigEndian->SetWritePosition(count);
					break;

				case Endians::Little:
					Marshal::Copy(buffer, &(*lpBuffer_LittleEndian)[0], offset, count);
					lpBuffer_LittleEndian->SetWritePosition(count);
					break;
			}
		}

		virtual ~ByteBuffer()
		{
			switch (GetEndian())
			{
				case Endians::Big:
					delete lpBuffer_BigEndian;
					break;

				case Endians::Little:
					delete lpBuffer_LittleEndian;
					break;
			}
		}

		::System::UInt32 GetSize()
		{
			switch (GetEndian())
			{
				case Endians::Big:
					return ::System::UInt32(lpBuffer_BigEndian->GetSize());

				case Endians::Little:
					return ::System::UInt32(lpBuffer_LittleEndian->GetSize());
			}

			return 0;
		}

		Endians GetEndian()
		{
			return endian;
		}

		::System::UInt32 GetCapacity()
		{
			switch (GetEndian())
			{
				case Endians::Big:
					return ::System::UInt32(lpBuffer_BigEndian->GetCapacity());

				case Endians::Little:
					return ::System::UInt32(lpBuffer_LittleEndian->GetCapacity());
			}

			return 0;
		}

		::System::UInt32 GetReadPosition()
		{
			switch (GetEndian())
			{
				case Endians::Big:
					return ::System::UInt32(lpBuffer_BigEndian->GetReadPosition());

				case Endians::Little:
					return ::System::UInt32(lpBuffer_LittleEndian->GetReadPosition());
			}

			return 0;
		}

		::System::UInt32 GetWritePosition()
		{
			switch (GetEndian())
			{
				case Endians::Big:
					return ::System::UInt32(lpBuffer_BigEndian->GetWritePosition());

				case Endians::Little:
					return ::System::UInt32(lpBuffer_LittleEndian->GetWritePosition());
			}

			return 0;
		}

		void SetCapacity(::System::UInt32 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->SetCapacity(static_cast<size_t>(value));
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->SetCapacity(static_cast<size_t>(value));
					break;
			}
		}

		void SetReadPosition(::System::UInt32 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->SetReadPosition(static_cast<size_t>(value));
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->SetReadPosition(static_cast<size_t>(value));
					break;
			}
		}

		void SetWritePosition(::System::UInt32 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->SetWritePosition(static_cast<size_t>(value));
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->SetWritePosition(static_cast<size_t>(value));
					break;
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

			switch (Marshal::SizeOf<T>())
			{
				case 1:
				{
					::System::Byte _value;

					if (PeekUInt8(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 2:
				{
					::System::UInt16 _value;

					if (PeekUInt16(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 4:
				{
					::System::UInt32 _value;

					if (PeekUInt32(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 8:
				{
					::System::UInt64 _value;

					if (PeekUInt64(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool PeekBool([::System::Runtime::InteropServices::OutAttribute] ::System::Boolean% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					bool _value;

					if (lpBuffer_BigEndian->PeekBool(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					bool _value;

					if (lpBuffer_LittleEndian->PeekBool(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool PeekInt8([::System::Runtime::InteropServices::OutAttribute] ::System::SByte% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int8 _value;

					if (lpBuffer_BigEndian->PeekInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int8 _value;

					if (lpBuffer_LittleEndian->PeekInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekInt16([::System::Runtime::InteropServices::OutAttribute] ::System::Int16% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int16 _value;

					if (lpBuffer_BigEndian->PeekInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int16 _value;

					if (lpBuffer_LittleEndian->PeekInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekInt32([::System::Runtime::InteropServices::OutAttribute] ::System::Int32% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int32 _value;

					if (lpBuffer_BigEndian->PeekInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int32 _value;

					if (lpBuffer_LittleEndian->PeekInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekInt64([::System::Runtime::InteropServices::OutAttribute] ::System::Int64% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int64 _value;

					if (lpBuffer_BigEndian->PeekInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int64 _value;

					if (lpBuffer_LittleEndian->PeekInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool PeekUInt8([::System::Runtime::InteropServices::OutAttribute] ::System::Byte% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint8 _value;

					if (lpBuffer_BigEndian->PeekUInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint8 _value;

					if (lpBuffer_LittleEndian->PeekUInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekUInt16([::System::Runtime::InteropServices::OutAttribute] ::System::UInt16% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint16 _value;

					if (lpBuffer_BigEndian->PeekUInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint16 _value;

					if (lpBuffer_LittleEndian->PeekUInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekUInt32([::System::Runtime::InteropServices::OutAttribute] ::System::UInt32% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint32 _value;

					if (lpBuffer_BigEndian->PeekUInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint32 _value;

					if (lpBuffer_LittleEndian->PeekUInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekUInt64([::System::Runtime::InteropServices::OutAttribute] ::System::UInt64% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint64 _value;

					if (lpBuffer_BigEndian->PeekUInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint64 _value;

					if (lpBuffer_LittleEndian->PeekUInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		
		bool PeekFloat([::System::Runtime::InteropServices::OutAttribute] ::System::Single% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					float _value;

					if (lpBuffer_BigEndian->PeekFloat(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					float _value;

					if (lpBuffer_LittleEndian->PeekFloat(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool PeekDouble([::System::Runtime::InteropServices::OutAttribute] ::System::Double% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					double _value;

					if (lpBuffer_BigEndian->PeekDouble(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					double _value;

					if (lpBuffer_LittleEndian->PeekDouble(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool PeekString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			String nativeString;

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (!lpBuffer_BigEndian->PeekString(nativeString))
					{

						return false;
					}
				}
				break;

				case Endians::Little:
				{
					if (!lpBuffer_LittleEndian->PeekString(nativeString))
					{

						return false;
					}
				}
				break;
			}

			value = Marshal::ToString(
				nativeString
			);

			return true;
		}
		bool PeekWString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			WString nativeString;

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (!lpBuffer_BigEndian->PeekWString(nativeString))
					{

						return false;
					}
				}
				break;

				case Endians::Little:
				{
					if (!lpBuffer_LittleEndian->PeekWString(nativeString))
					{

						return false;
					}
				}
				break;
			}

			value = Marshal::ToString(
				nativeString
			);

			return true;
		}

		bool Peek([::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% buffer, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (lpBuffer_BigEndian->Peek(&_buffer[0], _buffer.GetCapacity()))
					{
						buffer = gcnew array<::System::Byte>(
							count
						);

						Marshal::Copy(
							&_buffer[0],
							buffer,
							0,
							count
						);

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					if (lpBuffer_LittleEndian->Peek(&_buffer[0], _buffer.GetCapacity()))
					{
						buffer = gcnew array<::System::Byte>(
							count
						);

						Marshal::Copy(
							&_buffer[0],
							buffer,
							0,
							count
						);

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool Peek([::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% buffer, ::System::UInt32 count, ::System::UInt32 index)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (lpBuffer_BigEndian->Peek(&_buffer[0], _buffer.GetCapacity(), index))
					{
						buffer = gcnew array<::System::Byte>(
							count
						);

						Marshal::Copy(
							&_buffer[0],
							buffer,
							0,
							count
						);

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					if (lpBuffer_LittleEndian->Peek(&_buffer[0], _buffer.GetCapacity(), index))
					{
						buffer = gcnew array<::System::Byte>(
							count
						);

						Marshal::Copy(
							&_buffer[0],
							buffer,
							0,
							count
						);

						return true;
					}
				}
				break;
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

			switch (Marshal::SizeOf<T>())
			{
				case 1:
				{
					::System::Byte _value;

					if (ReadUInt8(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 2:
				{
					::System::UInt16 _value;

					if (ReadUInt16(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 4:
				{
					::System::UInt32 _value;

					if (ReadUInt32(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;

				case 8:
				{
					::System::UInt64 _value;

					if (ReadUInt64(_value))
					{
						value = T(
							_value
						);

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool ReadBool([::System::Runtime::InteropServices::OutAttribute] ::System::Boolean% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					bool _value;

					if (lpBuffer_BigEndian->ReadBool(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					bool _value;

					if (lpBuffer_LittleEndian->ReadBool(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool ReadInt8([::System::Runtime::InteropServices::OutAttribute] ::System::SByte% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int8 _value;

					if (lpBuffer_BigEndian->ReadInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int8 _value;

					if (lpBuffer_LittleEndian->ReadInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadInt16([::System::Runtime::InteropServices::OutAttribute] ::System::Int16% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int16 _value;

					if (lpBuffer_BigEndian->ReadInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int16 _value;

					if (lpBuffer_LittleEndian->ReadInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadInt32([::System::Runtime::InteropServices::OutAttribute] ::System::Int32% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int32 _value;

					if (lpBuffer_BigEndian->ReadInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int32 _value;

					if (lpBuffer_LittleEndian->ReadInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadInt64([::System::Runtime::InteropServices::OutAttribute] ::System::Int64% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					int64 _value;

					if (lpBuffer_BigEndian->ReadInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					int64 _value;

					if (lpBuffer_LittleEndian->ReadInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool ReadUInt8([::System::Runtime::InteropServices::OutAttribute] ::System::Byte% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint8 _value;

					if (lpBuffer_BigEndian->ReadUInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint8 _value;

					if (lpBuffer_LittleEndian->ReadUInt8(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadUInt16([::System::Runtime::InteropServices::OutAttribute] ::System::UInt16% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint16 _value;

					if (lpBuffer_BigEndian->ReadUInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint16 _value;

					if (lpBuffer_LittleEndian->ReadUInt16(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadUInt32([::System::Runtime::InteropServices::OutAttribute] ::System::UInt32% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint32 _value;

					if (lpBuffer_BigEndian->ReadUInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint32 _value;

					if (lpBuffer_LittleEndian->ReadUInt32(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadUInt64([::System::Runtime::InteropServices::OutAttribute] ::System::UInt64% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					uint64 _value;

					if (lpBuffer_BigEndian->ReadUInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					uint64 _value;

					if (lpBuffer_LittleEndian->ReadUInt64(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		
		bool ReadFloat([::System::Runtime::InteropServices::OutAttribute] ::System::Single% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					float _value;

					if (lpBuffer_BigEndian->ReadFloat(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					float _value;

					if (lpBuffer_LittleEndian->ReadFloat(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}
		bool ReadDouble([::System::Runtime::InteropServices::OutAttribute] ::System::Double% value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
				{
					double _value;

					if (lpBuffer_BigEndian->ReadDouble(_value))
					{
						value = _value;

						return true;
					}
				}
				break;

				case Endians::Little:
				{
					double _value;

					if (lpBuffer_LittleEndian->ReadDouble(_value))
					{
						value = _value;

						return true;
					}
				}
				break;
			}

			return false;
		}

		bool ReadString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			String nativeString;

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (!lpBuffer_BigEndian->ReadString(nativeString))
					{

						return false;
					}
				}
				break;

				case Endians::Little:
				{
					if (!lpBuffer_LittleEndian->ReadString(nativeString))
					{

						return false;
					}
				}
				break;
			}

			value = Marshal::ToString(
				nativeString
			);

			return true;
		}
		bool ReadWString([::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			WString nativeString;

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (!lpBuffer_BigEndian->ReadWString(nativeString))
					{

						return false;
					}
				}
				break;

				case Endians::Little:
				{
					if (!lpBuffer_LittleEndian->ReadWString(nativeString))
					{

						return false;
					}
				}
				break;
			}

			value = Marshal::ToString(
				nativeString
			);

			return true;
		}

		bool Read([::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% buffer, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			switch (GetEndian())
			{
				case Endians::Big:
				{
					if (!lpBuffer_BigEndian->Read(&_buffer[0], _buffer.GetCapacity()))
					{

						return false;
					}
				}
				break;

				case Endians::Little:
				{
					if (!lpBuffer_LittleEndian->Read(&_buffer[0], _buffer.GetCapacity()))
					{

						return false;
					}
				}
				break;
			}

			buffer = gcnew array<::System::Byte>(
				count
			);

			Marshal::Copy(
				&_buffer[0],
				buffer,
				0,
				count
			);

			return true;
		}

		generic<typename T>
		void WriteEnum(T value)
		{
			if (!T::typeid->IsEnum)
			{

				throw gcnew ::System::ArgumentException(
					"T is not an Enum"
				);
			}

			switch (Marshal::SizeOf<T>())
			{
				case 1:
					WriteUInt8(static_cast<::System::Byte>(::System::Convert::ToByte(value)));
					break;
				
				case 2:
					WriteUInt16(static_cast<::System::UInt16>(::System::Convert::ToUInt16(value)));
					break;
				
				case 4:
					WriteUInt32(static_cast<::System::UInt32>(::System::Convert::ToUInt32(value)));
					break;
				
				case 8:
					WriteUInt64(static_cast<::System::UInt64>(::System::Convert::ToUInt64(value)));
					break;
			}
		}

		void WriteBool(::System::Boolean value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteBool(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteBool(value);
					break;
			}
		}

		void WriteInt8(::System::SByte value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteInt8(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteInt8(value);
					break;
			}
		}
		void WriteInt16(::System::Int16 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteInt16(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteInt16(value);
					break;
			}
		}
		void WriteInt32(::System::Int32 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteInt32(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteInt32(value);
					break;
			}
		}
		void WriteInt64(::System::Int64 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteInt64(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteInt64(value);
					break;
			}
		}
		
		void WriteUInt8(::System::Byte value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteUInt8(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteUInt8(value);
					break;
			}
		}
		void WriteUInt16(::System::UInt16 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteUInt16(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteUInt16(value);
					break;
			}
		}
		void WriteUInt32(::System::UInt32 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteUInt32(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteUInt32(value);
					break;
			}
		}
		void WriteUInt64(::System::UInt64 value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteUInt64(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteUInt64(value);
					break;
			}
		}

		void WriteFloat(::System::Single value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteFloat(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteFloat(value);
					break;
			}
		}
		void WriteDouble(::System::Double value)
		{
			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteDouble(value);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteDouble(value);
					break;
			}
		}

		void WriteString(::System::String^ value)
		{
			auto nativeString = Marshal::ToNativeString(
				value
			);

			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteString(nativeString);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteString(nativeString);
					break;
			}
		}
		void WriteWString(::System::String^ value)
		{
			auto nativeString = Marshal::ToNativeWString(
				value
			);

			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->WriteWString(nativeString);
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->WriteWString(nativeString);
					break;
			}
		}

		void Write(array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			Marshal::Copy(
				buffer,
				&_buffer[0],
				offset,
				count
			);

			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer_BigEndian->Write(&_buffer[0], _buffer.GetCapacity());
					break;

				case Endians::Little:
					lpBuffer_LittleEndian->Write(&_buffer[0], _buffer.GetCapacity());
					break;
			}
		}

		virtual array<::System::Byte>^ ToArray()
		{
			auto _array = gcnew array<::System::Byte>(
				GetSize()
			);

			const uint8* lpBuffer = nullptr;

			switch (GetEndian())
			{
				case Endians::Big:
					lpBuffer = &(*lpBuffer_BigEndian)[0];
					break;

				case Endians::Little:
					lpBuffer = &(*lpBuffer_LittleEndian)[0];
					break;
			}

			Marshal::Copy(
				lpBuffer,
				_array,
				0,
				static_cast<::System::UInt32>(_array->Length)
			);

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

			auto valueBuffer = Marshal::ToArray(
				value
			);

			::System::Array::Reverse(
				valueBuffer
			);

			result = Marshal::FromArray<T>(
				valueBuffer,
				0
			);
		}
		
		generic<typename T>
		static void FromEndian(Endians endian, [::System::Runtime::InteropServices::OutAttribute] T% result, T value)
		{
			if (endian == Endians::Machine)
			{
				result = value;

				return;
			}

			auto valueBuffer = Marshal::ToArray(
				value
			);

			::System::Array::Reverse(
				valueBuffer
			);

			result = Marshal::FromArray<T>(
				valueBuffer,
				0
			);
		}
		
		static bool operator == (ByteBuffer^ buffer1, ByteBuffer^ buffer2)
		{
			if (buffer1->GetEndian() != buffer2->GetEndian())
			{

				return false;
			}

			switch (buffer1->GetEndian())
			{
				case Endians::Big:
					return *buffer1->lpBuffer_BigEndian == *buffer2->lpBuffer_BigEndian;

				case Endians::Little:
					return *buffer1->lpBuffer_LittleEndian == *buffer2->lpBuffer_LittleEndian;
			}

			return false;
		}
		static bool operator != (ByteBuffer^ buffer1, ByteBuffer^ buffer2)
		{
			return !operator==(
				buffer1,
				buffer2
			);
		}
	};
}
