#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	extern "C"
	{
		#include <i2c/smbus.h>

		#include <linux/i2c-dev.h>
	}
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
		void ReadInt8(I2CRegister reg, int8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int result;

			if ((result = i2c_smbus_read_byte_data(hFile, static_cast<__u8>(reg))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_read_byte_data",
					result
				);
			}

			value = static_cast<int8>(
				result
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadInt16(I2CRegister reg, int16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int result;

			if ((result = i2c_smbus_read_word_data(hFile, static_cast<__u8>(reg))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_read_word_data",
					result
				);
			}

			value = static_cast<int16>(
				result
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void ReadUInt8(I2CRegister reg, uint8& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int result;

			if ((result = i2c_smbus_read_byte_data(hFile, static_cast<__u8>(reg))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_read_byte_data",
					result
				);
			}

			value = static_cast<uint8>(
				result
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void ReadUInt16(I2CRegister reg, uint16& value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int result;

			if ((result = i2c_smbus_read_word_data(hFile, static_cast<__u8>(reg))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_read_word_data",
					result
				);
			}

			value = static_cast<uint16>(
				result
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void Read(I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int errorCode;

			if ((errorCode = i2c_smbus_read_i2c_block_data(hFile, static_cast<__u8>(reg), static_cast<__u8>((size <= 255) ? size : 255), reinterpret_cast<__u8*>(lpBuffer))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_read_i2c_block_data",
					errorCode
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		
		// @throw AL::Exceptions::Exception
		void WriteInt8(I2CRegister reg, int8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int errorCode;

			if ((errorCode = i2c_smbus_write_byte_data(hFile, static_cast<__u8>(reg), static_cast<__u8>(value))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_write_byte_data",
					errorCode
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteInt16(I2CRegister reg, uint16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int errorCode;

			if ((errorCode = i2c_smbus_write_word_data(hFile, static_cast<__u8>(reg), static_cast<__u16>(value))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_write_word_data",
					errorCode
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void WriteUInt8(I2CRegister reg, uint8 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int errorCode;

			if ((errorCode = i2c_smbus_write_byte_data(hFile, static_cast<__u8>(reg), static_cast<__u8>(value))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_write_byte_data",
					errorCode
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void WriteUInt16(I2CRegister reg, uint16 value)
		{
			AL_ASSERT(IsOpen(), "I2CDevice not open");

#if defined(AL_PLATFORM_LINUX)
			int errorCode;

			if ((errorCode = i2c_smbus_write_word_data(hFile, static_cast<__u8>(reg), static_cast<__u16>(value))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_write_word_data",
					errorCode
				);
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
			int errorCode;

			if ((errorCode = i2c_smbus_write_i2c_block_data(hFile, static_cast<__u8>(reg), static_cast<__u8>((size <= 255) ? size : 255), reinterpret_cast<const __u8*>(lpBuffer))) < 0)
			{

				throw Exceptions::SystemException(
					"i2c_smbus_write_i2c_block_data",
					errorCode
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
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
