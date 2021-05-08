#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	#include <linux/i2c.h>
	#include <linux/i2c-dev.h>
#endif

namespace AL::GPIO
{
	typedef uint8 I2CAddress;
	typedef uint8 I2CRegister;

	class I2CDevice
	{
		bool isSlave = false;

#if defined(AL_PLATFORM_LINUX)
		typedef int HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = -1;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef HANDLE HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = INVALID_HANDLE_VALUE;
#else
		typedef void* HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = nullptr;
#endif

		String     name;
		HFILE      hFile;
		I2CAddress address;

		I2CDevice(const I2CDevice&) = delete;

		I2CDevice(HFILE hFile, String&& name, I2CAddress address)
			: name(
				Move(name)
			),
			hFile(
				hFile
			),
			address(
				address
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		static void Open(I2CDevice& device, String&& name, I2CAddress address)
		{
#if defined(AL_PLATFORM_LINUX)
			int fd;

			if ((fd = open(name.GetCString(), O_RDWR)) == -1)
			{

				throw Exceptions::SystemException(
					"open"
				);
			}

			if (ioctl(fd, I2C_SLAVE, address) == -1)
			{
				close(fd);

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			device = I2CDevice(
				fd,
				Move(name),
				address
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		static void Open(I2CDevice& device, const String& name, I2CAddress address)
		{
			Open(
				device,
				String(name),
				address
			);
		}

		I2CDevice()
			: hFile(
				FILE_HANDLE_NULL
			)
		{
		}

		I2CDevice(I2CDevice&& device)
			: isSlave(
				device.isSlave
			),
			name(
				Move(device.name)
			),
			hFile(
				device.hFile
			),
			address(
				device.address
			)
		{
			device.isSlave = false;

			device.hFile = FILE_HANDLE_NULL;
		}

		virtual ~I2CDevice()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hFile != FILE_HANDLE_NULL;
		}

		bool IsSlave() const
		{
			return isSlave;
		}

		auto& GetName() const
		{
			return name;
		}

		auto GetAddress() const
		{
			return address;
		}

		// @throw AL::Exceptions::Exception
		void Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			for (size_t i = 0; i < size; i += I2C_SMBUS_BLOCK_MAX)
			{
				i2c_smbus_data data;
				data.block[0] = static_cast<typename Remove_Modifiers<decltype(i2c_smbus_data::block[0])>::Type>(
					((size - i) > I2C_SMBUS_BLOCK_MAX) ? I2C_SMBUS_BLOCK_MAX : (size - i)
				);

				i2c_smbus_ioctl_data request;
				request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
				request.read_write = I2C_SMBUS_READ;
				request.data = &data;
				request.command = 0;

				if (ioctl(hFile, I2C_SMBUS, &request) == -1)
				{

					throw Exceptions::SystemException(
						"ioctl"
					);
				}

				memcpy(
					&data.block[1],
					&reinterpret_cast<uint8*>(lpBuffer)[i],
					data.block[0]
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void Read(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			for (size_t i = 0; i < size; i += I2C_SMBUS_BLOCK_MAX)
			{
				i2c_smbus_data data;
				data.block[0] = static_cast<typename Remove_Modifiers<decltype(i2c_smbus_data::block[0])>::Type>(
					((size - i) > I2C_SMBUS_BLOCK_MAX) ? I2C_SMBUS_BLOCK_MAX : (size - i)
				);

				i2c_smbus_ioctl_data request;
				request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
				request.read_write = I2C_SMBUS_READ;
				request.data = &data;
				request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
					reg
				);

				if (ioctl(hFile, I2C_SMBUS, &request) == -1)
				{

					throw Exceptions::SystemException(
						"ioctl"
					);
				}

				memcpy(
					&data.block[1],
					&reinterpret_cast<uint8*>(lpBuffer)[i],
					data.block[0]
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void ReadInt8(int8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = static_cast<int8>(
				result.byte
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadInt8(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				ReadInt8(
					reinterpret_cast<int8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void ReadUInt8(uint8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = result.byte;
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadUInt8(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				ReadUInt8(
					reinterpret_cast<uint8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void ReadInt16(int16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;
			result.block[0] = sizeof(int16);

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_I2C_BLOCK_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = *reinterpret_cast<const int16*>(
				&result.block[0]
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadInt16(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				ReadInt16(
					reinterpret_cast<int16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void ReadUInt16(uint16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;
			result.block[0] = sizeof(uint16);

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_I2C_BLOCK_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = *reinterpret_cast<const uint16*>(
				&result.block[0]
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadUInt16(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				ReadUInt16(
					reinterpret_cast<uint16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void ReadInt8(I2CRegister reg, int8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = static_cast<int8>(
				result.byte
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadInt8(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				ReadInt8(
					reg,
					reinterpret_cast<int8*>(lpBuffer)[i]
				);
			}
		}
		
		// @throw AL::Exceptions::Exception
		void ReadUInt8(I2CRegister reg, uint8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = result.byte;
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadUInt8(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				ReadUInt8(
					reg,
					reinterpret_cast<uint8*>(lpBuffer)[i]
				);
			}
		}
		
		// @throw AL::Exceptions::Exception
		void ReadInt16(I2CRegister reg, int16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_WORD_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = static_cast<int16>(
				result.word
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadInt16(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				ReadInt16(
					reg,
					reinterpret_cast<int16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void ReadUInt16(I2CRegister reg, uint16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data result;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_WORD_DATA;
			request.read_write = I2C_SMBUS_READ;
			request.data = &result;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}

			value = result.word;
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadUInt16(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				ReadUInt16(
					reg,
					reinterpret_cast<uint16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			for (size_t i = 0; i < size; i += I2C_SMBUS_BLOCK_MAX)
			{
				i2c_smbus_data data;

				data.block[0] = static_cast<typename Remove_Modifiers<decltype(i2c_smbus_data::block[0])>::Type>(
					((size - i) > I2C_SMBUS_BLOCK_MAX) ? I2C_SMBUS_BLOCK_MAX : (size - i)
				);

				memcpy(
					&data.block[1],
					&reinterpret_cast<const uint8*>(lpBuffer)[i],
					data.block[0]
				);

				i2c_smbus_ioctl_data request;
				request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
				request.read_write = I2C_SMBUS_WRITE;
				request.data = &data;
				request.command = 0;

				if (ioctl(hFile, I2C_SMBUS, &request) == -1)
				{

					throw Exceptions::SystemException(
						"ioctl"
					);
				}
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void Write(I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			for (size_t i = 0; i < size; i += I2C_SMBUS_BLOCK_MAX)
			{
				i2c_smbus_data data;

				data.block[0] = static_cast<typename Remove_Modifiers<decltype(i2c_smbus_data::block[0])>::Type>(
					((size - i) > I2C_SMBUS_BLOCK_MAX) ? I2C_SMBUS_BLOCK_MAX : (size - i)
				);

				memcpy(
					&data.block[1],
					&reinterpret_cast<const uint8*>(lpBuffer)[i],
					data.block[0]
				);

				i2c_smbus_ioctl_data request;
				request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
				request.read_write = I2C_SMBUS_WRITE;
				request.data = &data;
				request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
					reg
				);

				if (ioctl(hFile, I2C_SMBUS, &request) == -1)
				{

					throw Exceptions::SystemException(
						"ioctl"
					);
				}
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void WriteInt8(int8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.byte = static_cast<uint8>(
				value
			);

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteInt8(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				WriteInt8(
					reinterpret_cast<const int8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt8(uint8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.byte = value;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt8(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				WriteUInt8(
					reinterpret_cast<const uint8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteInt16(int16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.block[0] = sizeof(int16);
			memcpy(&data.block[1], &value, sizeof(int16));

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteInt16(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				WriteInt16(
					reinterpret_cast<const int16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt16(uint16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.block[0] = sizeof(uint16);
			memcpy(&data.block[1], &value, sizeof(uint16));

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_I2C_BLOCK_BROKEN;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = 0;

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt16(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				WriteUInt16(
					reinterpret_cast<const uint16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteInt8(I2CRegister reg, int8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.byte = static_cast<uint8>(
				value
			);

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE_DATA;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteInt8(I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				WriteInt8(
					reg,
					reinterpret_cast<const int8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt8(I2CRegister reg, uint8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.byte = value;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_BYTE_DATA;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt8(I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; ++i)
			{
				WriteUInt8(
					reg,
					reinterpret_cast<const uint8*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteInt16(I2CRegister reg, uint16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.word = static_cast<uint16>(
				value
			);

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_WORD_DATA;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteInt16(I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				WriteInt16(
					reg,
					reinterpret_cast<const int16*>(lpBuffer)[i]
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt16(I2CRegister reg, uint16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_smbus_data data;
			data.word = value;

			i2c_smbus_ioctl_data request;
			request.size = I2C_SMBUS_WORD_DATA;
			request.read_write = I2C_SMBUS_WRITE;
			request.data = &data;
			request.command = static_cast<decltype(i2c_smbus_ioctl_data::command)>(
				reg
			);

			if (ioctl(hFile, I2C_SMBUS, &request) == -1)
			{

				throw Exceptions::SystemException(
					"ioctl"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt16(I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

			for (size_t i = 0; i < size; i += 2)
			{
				WriteUInt16(
					reg,
					reinterpret_cast<const uint16*>(lpBuffer)[i]
				);
			}
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				close(hFile);
				hFile = FILE_HANDLE_NULL;
#endif
			}
		}

		I2CDevice& operator = (I2CDevice&& device)
		{
			Close();

			isSlave = device.isSlave;
			device.isSlave = false;

			name = Move(
				device.name
			);

			hFile = device.hFile;
			device.hFile = FILE_HANDLE_NULL;

			address = device.address;

			return *this;
		}
	};
}
