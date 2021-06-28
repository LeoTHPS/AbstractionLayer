#pragma once
#include "AL/Common.hpp"

#include "I2CDevice.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	extern "C"
	{
		#include <linux/i2c.h>
		#include <linux/i2c-dev.h>
	}
#endif

namespace AL::GPIO
{
	class I2CBus
	{
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

		I2CBus(const I2CBus&) = delete;

		I2CBus(HFILE hFile, String&& name)
			: name(
				Move(name)
			),
			hFile(
				hFile
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		static void Open(I2CBus& bus, String&& name)
		{
#if defined(AL_PLATFORM_LINUX)
			int fd;

			if ((fd = open(name.GetCString(), O_RDWR)) == -1)
			{

				throw Exceptions::SystemException(
					"open"
				);
			}

			bus = I2CBus(
				fd,
				Move(name)
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		static void Open(I2CBus& bus, const String& name)
		{
			Open(
				bus,
				String(name)
			);
		}

		I2CBus()
			: hFile(
				FILE_HANDLE_NULL
			)
		{
		}

		I2CBus(I2CBus&& bus)
			: name(
				Move(bus.name)
			),
			hFile(
				bus.hFile
			)
		{
			bus.hFile = FILE_HANDLE_NULL;
		}

		virtual ~I2CBus()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hFile != FILE_HANDLE_NULL;
		}

		auto& GetName() const
		{
			return name;
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Read(I2CAddress address, T& value)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

			Read(
				address,
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Read(I2CAddress address, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_msg i2c_buffer;
			i2c_buffer.flags = I2C_M_RD;
			i2c_buffer.addr = static_cast<__u16>(address);
			i2c_buffer.buf = reinterpret_cast<__u8*>(lpBuffer);
			i2c_buffer.len = static_cast<__u16>(size);

			i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = &i2c_buffer;
			i2c_data.nmsgs = 1;

			if (ioctl(hFile, I2C_RDWR, &i2c_data) < 0)
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
		template<typename T, size_t S = sizeof(T)>
		void Write(I2CAddress address, const T& value)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

			Write(
				address,
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Write(I2CAddress address, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_msg i2c_buffer;
			i2c_buffer.flags = 0;
			i2c_buffer.addr = static_cast<__u16>(address);
			i2c_buffer.buf = reinterpret_cast<__u8*>(const_cast<void*>(lpBuffer));
			i2c_buffer.len = static_cast<__u16>(size);

			i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = &i2c_buffer;
			i2c_data.nmsgs = 1;

			if (ioctl(hFile, I2C_RDWR, &i2c_data) < 0)
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
		template<typename T, size_t S = sizeof(T)>
		void ReadRegister(I2CAddress address, I2CRegister reg, T& value)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

			ReadRegister(
				address,
				reg,
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void ReadRegister(I2CAddress address, I2CRegister reg, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

#if defined(AL_PLATFORM_LINUX)
			i2c_msg i2c_buffers[2];
			
			// TX
			{
				i2c_buffers[0].flags = 0;
				i2c_buffers[0].addr = static_cast<__u16>(address);
				i2c_buffers[0].buf = reinterpret_cast<__u8*>(&reg);
				i2c_buffers[0].len = sizeof(I2CRegister);
			}

			// RX
			{
				i2c_buffers[1].flags = I2C_M_RD | I2C_M_NOSTART;
				i2c_buffers[1].addr = static_cast<__u16>(address);
				i2c_buffers[1].buf = reinterpret_cast<__u8*>(lpBuffer);
				i2c_buffers[1].len = static_cast<__u16>(size);
			}

			i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = i2c_buffers;
			i2c_data.nmsgs = 2;

			if (ioctl(hFile, I2C_RDWR, &i2c_data) < 0)
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
		template<typename T, size_t S = sizeof(T)>
		void WriteRegister(I2CAddress address, I2CRegister reg, const T& value)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");

			WriteRegister(
				address,
				reg,
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void WriteRegister(I2CAddress address, I2CRegister reg, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "I2CBus not open");
			
#if defined(AL_PLATFORM_LINUX)
			auto _lpBuffer = new uint8[sizeof(I2CRegister) + size];

			memcpy(
				&_lpBuffer[0],
				&reg,
				sizeof(I2CRegister)
			);

			memcpy(
				&_lpBuffer[sizeof(I2CRegister)],
				lpBuffer,
				size
			);

			try
			{
				Write(
					address,
					_lpBuffer,
					sizeof(I2CRegister) + size
				);
			}
			catch (Exceptions::Exception&)
			{
				delete[] _lpBuffer;

				throw;
			}

			delete[] _lpBuffer;
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

		I2CBus& operator = (I2CBus&& bus)
		{
			Close();

			name = Move(
				bus.name
			);

			hFile = bus.hFile;
			bus.hFile = FILE_HANDLE_NULL;

			return *this;
		}
	};
}

// @throw AL::Exceptions::Exception
template<typename T, AL::size_t S>
inline void AL::GPIO::I2CDevice::Read(T& value)
{
	GetBus().Read<T, S>(
		GetAddress(),
		value
	);
}
// @throw AL::Exceptions::Exception
inline void AL::GPIO::I2CDevice::Read(void* lpBuffer, size_t size)
{
	GetBus().Read(
		GetAddress(),
		lpBuffer,
		size
	);
}

// @throw AL::Exceptions::Exception
template<typename T, AL::size_t S>
inline void AL::GPIO::I2CDevice::Write(const T& value)
{
	GetBus().Write<T, S>(
		GetAddress(),
		value
	);
}
// @throw AL::Exceptions::Exception
inline void AL::GPIO::I2CDevice::Write(const void* lpBuffer, size_t size)
{
	GetBus().Write(
		GetAddress(),
		lpBuffer,
		size
	);
}

// @throw AL::Exceptions::Exception
template<typename T, AL::size_t S>
inline void AL::GPIO::I2CDevice::ReadRegister(I2CRegister reg, T& value)
{
	GetBus().ReadRegister<T, S>(
		GetAddress(),
		reg,
		value
	);
}
// @throw AL::Exceptions::Exception
inline void AL::GPIO::I2CDevice::ReadRegister(I2CRegister reg, void* lpBuffer, size_t size)
{
	GetBus().ReadRegister(
		GetAddress(),
		reg,
		lpBuffer,
		size
	);
}

// @throw AL::Exceptions::Exception
template<typename T, AL::size_t S>
inline void AL::GPIO::I2CDevice::WriteRegister(I2CRegister reg, const T& value)
{
	GetBus().WriteRegister<T, S>(
		GetAddress(),
		reg,
		value
	);
}
// @throw AL::Exceptions::Exception
inline void AL::GPIO::I2CDevice::WriteRegister(I2CRegister reg, const void* lpBuffer, size_t size)
{
	GetBus().WriteRegister(
		GetAddress(),
		reg,
		lpBuffer,
		size
	);
}
