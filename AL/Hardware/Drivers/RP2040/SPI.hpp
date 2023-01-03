#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include <hardware/spi.h>

namespace AL::Hardware::Drivers::RP2040
{
	enum class SPIModes : uint8
	{
		Zero, One, Two, Three
	};

	class SPI
	{
		Bool         isOpen  = False;
		Bool         isSlave = False;

		::spi_inst*  spi;
		mutable GPIO cs;
		GPIOPin      miso;
		GPIOPin      mosi;
		GPIOPin      sclk;
		SPIModes     mode;
		uint32       baud;
		uint8        bitCount;

		SPI(const SPI&) = delete;

	public:
		typedef SPIModes Modes;

		SPI(SPI&& spi)
			: isOpen(
				spi.isOpen
			),
			spi(
				spi.spi
			),
			cs(
				Move(spi.cs)
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
			mode(
				spi.mode
			),
			baud(
				spi.baud
			),
			bitCount(
				spi.bitCount
			)
		{
			spi.isOpen  = False;
		}

		SPI(::spi_inst* spi, GPIOPin miso, GPIOPin mosi, GPIOPin sclk, GPIOPin cs, Modes mode, uint32 baud, uint8 bitCount)
			: spi(
				spi
			),
			cs(
				cs,
				GPIOPinDirections::Output,
				GPIOPinValues::High
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
			mode(
				mode
			),
			baud(
				baud
			),
			bitCount(
				bitCount
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
			return cs.GetPin();
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

		auto GetMode() const
		{
			return mode;
		}

		auto GetBaud() const
		{
			return baud;
		}

		auto GetBitCount() const
		{
			return bitCount;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"SPI already open"
			);

			baud = ::spi_init(
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

			cs.Open();

			::spi_set_format(
				GetSPI(),
				GetBitCount(),
				GetClockPolarityFromMode(GetMode()),
				GetClockPhaseFromMode(GetMode()),
				::SPI_MSB_FIRST
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
				cs.Close();

				::spi_deinit(
					GetSPI()
				);

				isOpen = False;
			}
		}

		Void Read(Void* lpBuffer, size_t size, Bool changeCS = True) const
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			if (changeCS)
			{
				cs.Write(
					GPIOPinValues::Low
				);
			}

			::spi_read_blocking(
				GetSPI(),
				0,
				reinterpret_cast<::uint8_t*>(lpBuffer),
				size
			);

			if (changeCS)
			{
				cs.Write(
					GPIOPinValues::High
				);
			}
		}

		Void Write(const Void* lpBuffer, size_t size, Bool changeCS = True)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			if (changeCS)
			{

				SetCSActive();
			}

			::spi_write_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint8_t*>(lpBuffer),
				size
			);

			if (changeCS)
			{

				SetCSActive(False);
			}
		}

		Void WriteRead(const Void* lpTX, Void* lpRX, size_t size, Bool changeCS = True)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			if (changeCS)
			{

				SetCSActive();
			}

			::spi_write_read_blocking(
				GetSPI(),
				reinterpret_cast<const ::uint8_t*>(lpTX),
				reinterpret_cast<::uint8_t*>(lpRX),
				size
			);

			if (changeCS)
			{

				SetCSActive(False);
			}
		}

		Void SetCSActive(Bool set = True)
		{
			AL_ASSERT(
				IsOpen(),
				"SPI not open"
			);

			cs.Write(
				set ? GPIOPinValues::Low : GPIOPinValues::High
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
			cs        = Move(spi.cs);
			miso      = spi.miso;
			mosi      = spi.mosi;
			sclk      = spi.sclk;
			mode      = spi.mode;
			baud      = spi.baud;
			bitCount  = spi.bitCount;

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

	private:
		static Void GetClockEdgeFromMode(Modes value)
		{
			switch (value)
			{
				case SPIModes::Zero:
					return ;

				case SPIModes::One:
					return ;

				case SPIModes::Two:
					return ;

				case SPIModes::Three:
					return ;
			}

			throw NotImplementedException();
		}

		static ::spi_cpha_t GetClockPhaseFromMode(Modes value)
		{
			switch (value)
			{
				case SPIModes::Zero:
					return ::SPI_CPHA_0;

				case SPIModes::One:
					return ::SPI_CPHA_1;

				case SPIModes::Two:
					return ::SPI_CPHA_0;

				case SPIModes::Three:
					return ::SPI_CPHA_1;
			}

			throw NotImplementedException();
		}

		static ::spi_cpol_t GetClockPolarityFromMode(Modes value)
		{
			switch (value)
			{
				case SPIModes::Zero:
					return ::SPI_CPOL_0;

				case SPIModes::One:
					return ::SPI_CPOL_0;

				case SPIModes::Two:
					return ::SPI_CPOL_1;

				case SPIModes::Three:
					return ::SPI_CPOL_1;
			}

			throw NotImplementedException();
		}
	};
}
