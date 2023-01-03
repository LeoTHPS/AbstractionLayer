#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include <hardware/spi.h>

namespace AL::Hardware::Drivers::RP2040
{
	class SPI
	{
		Bool        isOpen  = False;
		Bool        isSlave = False;

		::spi_inst* spi;
		GPIOPin     cs;
		GPIOPin     miso;
		GPIOPin     mosi;
		GPIOPin     sclk;
		uint32      baud;

		SPI(const SPI&) = delete;

	public:
		typedef I2CAddress Address;

		SPI(SPI&& spi)
			: isOpen(
				spi.isOpen
			),
			spi(
				spi.spi
			),
			cs(
				spi.cs
			),
			miso(
				spi.miso
			),
			mosi(
				spi.mosi
			),
			sclk(
				spi.sclk
			),
			baud(
				spi.baud
			)
		{
			spi.isOpen  = False;
		}

		SPI(::spi_inst* spi, GPIOPin miso, GPIOPin mosi, GPIOPin sclk, GPIOPin cs, uint32 baud)
			: spi(
				spi
			),
			cs(
				cs
			),
			miso(
				miso
			),
			mosi(
				mosi
			),
			sclk(
				sclk
			),
			baud(
				baud
			)
		{
		}

		virtual ~SPI()
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

		auto GetSPI() const
		{
			return spi;
		}

		auto GetCS() const
		{
			return cs;
		}

		auto GetMISO() const
		{
			return miso;
		}

		auto GetMOSI() const
		{
			return mosi;
		}

		auto GetSCLK() const
		{
			return sclk;
		}

		auto GetBaud() const
		{
			return baud;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"SPI already open"
			);

			::spi_init(
				GetSPI(),
				GetBaud()
			);

			::gpio_set_function(
				GetMISO(),
				::GPIO_FUNC_SPI
			);

			::gpio_set_function(
				GetSCLK(),
				::GPIO_FUNC_SPI
			);

			::gpio_set_function(
				GetMOSI(),
				::GPIO_FUNC_SPI
			);

			::gpio_set_function(
				GetCS(),
				::GPIO_FUNC_SPI
			);

			if (IsSlave())
			{
				::spi_set_slave(
					GetSPI(),
					true
				);
			}

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::spi_deinit(
					GetSPI()
				);

				isOpen = False;
			}
		}

		Void Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_read_blocking(
				GetSPI(),
				0,
				reinterpret_cast<::uint8_t*>(lpBuffer),
				size
			);
		}
		Void Read16(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_read16_blocking(
				GetSPI(),
				0,
				reinterpret_cast<::uint16_t*>(lpBuffer),
				size
			);
		}

		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_write_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint8_t*>(lpBuffer),
				size
			);
		}
		Void Write16(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_write16_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint16_t*>(lpBuffer),
				size
			);
		}

		Void WriteRead(const Void* lpTX, Void* lpRX, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_write_read_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint8_t*>(lpTX),
				reinterpret_cast<::uint8_t*>(lpRX),
				size
			);
		}
		Void WriteRead16(const Void* lpTX, Void* lpRX, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			::spi_write16_read16_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint16_t*>(lpTX),
				reinterpret_cast<::uint16_t*>(lpRX),
				size
			);
		}

		Void SetSlaveMode(Bool set = True)
		{
			if (IsOpen())
			{
				::spi_set_slave(
					GetSPI(),
					set
				);
			}

			isSlave = set;
		}

		SPI& operator = (SPI&& spi)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = spi.isOpen;
			spi.isOpen = False;

			isSlave = spi.isSlave;

			this->spi = spi.spi;
			cs        = spi.cs;
			miso      = spi.miso;
			mosi      = spi.mosi;
			sclk      = spi.sclk;
			baud      = spi.baud;

			return *this;
		}

		Bool operator == (const SPI& spi) const
		{
			if (GetSPI() != spi.GetSPI())
			{

				return False;
			}

			if (GetCS() != spi.GetCS())
			{

				return False;
			}

			if (GetMISO() != spi.GetMISO())
			{

				return False;
			}

			if (GetMOSI() != spi.GetMOSI())
			{

				return False;
			}

			if (GetSCLK() != spi.GetSCLK())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const SPI& spi) const
		{
			if (operator==(spi))
			{

				return False;
			}

			return True;
		}
	};
}
