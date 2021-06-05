#pragma once
#include "AL/Common.hpp"

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
	class I2CBus;

	typedef uint16 I2CAddress;
	typedef uint8  I2CRegister;

	class I2CDevice
	{
		I2CBus* lpBus;

		I2CAddress address;

		I2CDevice(const I2CDevice&) = delete;

	public:
		I2CDevice(I2CDevice&& device)
			: lpBus(
				device.lpBus
			),
			address(
				device.address
			)
		{
			device.lpBus = nullptr;
		}

		I2CDevice(I2CBus& bus, I2CAddress address)
			: lpBus(
				&bus
			),
			address(
				address
			)
		{
		}

		virtual ~I2CDevice()
		{
		}

		auto& GetBus()
		{
			return *lpBus;
		}
		auto& GetBus() const
		{
			return *lpBus;
		}

		auto GetAddress() const
		{
			return address;
		}
		
		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Read(T& value);
		// @throw AL::Exceptions::Exception
		void Read(void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Read(I2CRegister _register, T& value);
		// @throw AL::Exceptions::Exception
		void Read(I2CRegister _register, void* lpBuffer, size_t size);
		
		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Write(const T& value);
		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Write(I2CRegister _register, const T& value);
		// @throw AL::Exceptions::Exception
		void Write(I2CRegister _register, const void* lpBuffer, size_t size);

		auto& operator = (I2CDevice&& device)
		{
			lpBus = device.lpBus;
			device.lpBus = nullptr;

			address = device.address;

			return *this;
		}
	};
}
