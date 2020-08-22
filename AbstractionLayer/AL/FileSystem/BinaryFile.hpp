#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

namespace AL::FileSystem
{
	class BinaryFile
		: public File
	{
		Endians endian = Endians::Machine;

	public:
		using File::File;

		auto GetEndian() const
		{
			return endian;
		}

		void SetEndian(Endians value)
		{
			endian = value;
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Open(BitMask<FileOpenModes> mode)
		{
			mode.Add(
				FileOpenModes::Binary
			);

			return File::Open(
				mode
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadInt8(int8& value)
		{
			if (!Read(&value, sizeof(int8)))
			{

				return false;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadInt16(int16& value)
		{
			if (!Read(&value, sizeof(int16)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadInt32(int32& value)
		{
			if (!Read(&value, sizeof(int32)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadInt64(int64& value)
		{
			if (!Read(&value, sizeof(int64)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadUInt8(uint8& value)
		{
			if (!Read(&value, sizeof(uint8)))
			{

				return false;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadUInt16(uint16& value)
		{
			if (!Read(&value, sizeof(uint16)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadUInt32(uint32& value)
		{
			if (!Read(&value, sizeof(uint32)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadUInt64(uint64& value)
		{
			if (!Read(&value, sizeof(uint64)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadFloat(float& value)
		{
			if (!Read(&value, sizeof(float)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadDouble(double& value)
		{
			if (!Read(&value, sizeof(double)))
			{

				return false;
			}

			value = FlipEndian(
				value
			);

			return true;
		}
		
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadString(String& value)
		{
			uint32 length;

			if (!Read(&length, sizeof(uint32)))
			{

				return false;
			}

			length = FlipEndian(
				length
			);

			value.SetSize(
				length + 1
			);

			if (!Read(&value[0], length))
			{

				return false;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadWString(WString& value)
		{
			uint32 length;

			if (!Read(&length, sizeof(uint32)))
			{

				return false;
			}

			length = FlipEndian(
				length
			);

			value.SetSize(
				length + 1
			);

			if (!Read(&value[0], length))
			{

				return false;
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		void WriteInt8(int8 value)
		{
			Write(
				&value,
				sizeof(int8)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteInt16(int16 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(int16)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteInt32(int32 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(int32)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteInt64(int64 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(int64)
			);
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt8(uint8 value)
		{
			Write(
				&value,
				sizeof(int8)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt16(uint16 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(uint16)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt32(uint32 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(uint32)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt64(uint64 value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(uint64)
			);
		}

		// @throw AL::Exceptions::Exception
		void WriteFloat(float value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(float)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteDouble(double value)
		{
			value = FlipEndian(
				value
			);

			Write(
				&value,
				sizeof(double)
			);
		}

		// @throw AL::Exceptions::Exception
		void WriteString(const String& value)
		{
			auto length = static_cast<uint32>(
				value.GetLength()
			);

			auto _length = FlipEndian(
				length
			);

			Write(
				&_length,
				sizeof(uint32)
			);

			Write(
				value.GetCString(),
				length * sizeof(typename String::Char)
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteWString(const WString& value)
		{
			auto length = static_cast<uint32>(
				value.GetLength()
			);

			auto _length = FlipEndian(
				length
			);

			Write(
				&_length,
				sizeof(uint32)
			);

			Write(
				value.GetCString(),
				length * sizeof(typename WString::Char)
			);
		}

	private:
		template<typename T>
		constexpr T FlipEndian(T value) const
		{
			return (endian != Endians::Machine) ? BitConverter::Flip<T>(value) : value;
		}
	};
}
