#pragma once
#include "AL/Common.hpp"

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
		void Write(const T& value);
		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void ReadRegister(I2CRegister reg, T& value);
		// @throw AL::Exceptions::Exception
		void ReadRegister(I2CRegister reg, void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void WriteRegister(I2CRegister reg, const T& value);
		// @throw AL::Exceptions::Exception
		void WriteRegister(I2CRegister reg, const void* lpBuffer, size_t size);

		auto& operator = (I2CDevice&& device)
		{
			lpBus = device.lpBus;
			device.lpBus = nullptr;

			address = device.address;

			return *this;
		}
	};
}
