#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include "AL/OS/SystemException.hpp"

#include <hardware/i2c.h>

namespace AL::Hardware::Pico
{
	typedef uint8 I2CAddress;

	class I2C
	{
		Bool        isOpen  = False;
		Bool        isSlave = False;

		::i2c_inst* i2c;
		GPIOPin     scl;
		GPIOPin     sda;
		uint32      baud;
		I2CAddress  address;

		I2C(const I2C&) = delete;

	public:
		typedef I2CAddress Address;

		I2C(I2C&& i2c)
			: isOpen(
				i2c.isOpen
			),
			isSlave(
				i2c.isSlave
			),
			i2c(
				i2c.i2c
			),
			scl(
				i2c.scl
			),
			sda(
				i2c.sda
			),
			baud(
				i2c.baud
			),
			address(
				i2c.address
			)
		{
			i2c.isOpen  = False;
			i2c.isSlave = False;
		}

		I2C(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud)
			: i2c(
				i2c
			),
			scl(
				scl
			),
			sda(
				sda
			),
			baud(
				baud
			)
		{
		}

		virtual ~I2C()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsSlave() const
		{
			return isSlave;
		}

		auto GetI2C() const
		{
			return i2c;
		}

		auto GetSCL() const
		{
			return scl;
		}

		auto GetSDA() const
		{
			return sda;
		}

		auto GetBaud() const
		{
			return baud;
		}

		auto GetAddress() const
		{
			return address;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"I2C already open"
			);

			baud = ::i2c_init(
				GetI2C(),
				GetBaud()
			);

			::gpio_set_function(
				GetSDA(),
				::GPIO_FUNC_I2C
			);

			::gpio_set_function(
				GetSCL(),
				::GPIO_FUNC_I2C
			);

			::gpio_pull_up(
				GetSDA()
			);

			::gpio_pull_up(
				GetSCL()
			);

			if (IsSlave())
			{
				::i2c_set_slave_mode(
					GetI2C(),
					true,
					GetAddress()
				);
			}

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::i2c_deinit(
					GetI2C()
				);

				isOpen = False;
			}
		}

		Void Read(Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"I2C not open"
			);

			::i2c_read_raw_blocking(
				GetI2C(),
				reinterpret_cast<::uint8_t*>(lpBuffer),
				size
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size, Address address, Bool nostop = False) const
		{
			AL_ASSERT(
				IsOpen(),
				"I2C not open"
			);

			if (::i2c_read_blocking(GetI2C(), address, reinterpret_cast<::uint8_t*>(lpBuffer), size, nostop) == ::PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_read_blocking"
				);
			}
		}

		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"I2C not open"
			);

			::i2c_write_raw_blocking(
				GetI2C(),
				reinterpret_cast<const ::uint8_t*>(lpBuffer),
				size
			);
		}
		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size, Address address, Bool nostop = False)
		{
			AL_ASSERT(
				IsOpen(),
				"I2C not open"
			);

			if (::i2c_write_blocking(GetI2C(), address, reinterpret_cast<const ::uint8_t*>(lpBuffer), size, nostop) != ::PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_write_blocking"
				);
			}
		}

		Void SetSlaveMode(Bool set, uint8 address)
		{
			if (IsOpen())
			{
				::i2c_set_slave_mode(
					GetI2C(),
					set,
					address
				);
			}

			isSlave       = set;
			this->address = address;
		}

		I2C& operator = (I2C&& i2c)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = i2c.isOpen;
			i2c.isOpen = False;

			isSlave = i2c.isSlave;

			this->i2c  = i2c.i2c;
			scl        = i2c.scl;
			sda        = i2c.sda;
			baud       = i2c.baud;
			address    = i2c.address;

			return *this;
		}

		Bool operator == (const I2C& i2c) const
		{
			if (GetI2C() != i2c.GetI2C())
			{

				return False;
			}

			if (GetSCL() != i2c.GetSCL())
			{

				return False;
			}

			if (GetSDA() != i2c.GetSDA())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const I2C& i2c) const
		{
			if (operator==(i2c))
			{

				return False;
			}

			return True;
		}
	};
}
