#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "Drivers/Pico/SPI.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#include "AL/FileSystem/Path.hpp"

	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/ioctl.h>

	#if !AL_HAS_INCLUDE(<linux/spi/spidev.h>)
		#error Missing linux/spi/spidev.h
	#endif

	#include <linux/spi/spidev.h>
#else
	#error Platform not supported
#endif

namespace AL::Hardware
{
	typedef uint32 SPISpeed;

	enum class SPIModes : uint8
	{
		Zero, One, Two, Three
	};

	struct SPITransaction
	{
		Void*       lpRX       = nullptr;
		const Void* lpTX       = nullptr;
		size_t      BufferSize = 0;
	};

	class SPIDevice
	{
		Bool               isOpen = False;

#if defined(AL_PLATFORM_PICO)
		Drivers::Pico::SPI spi;
#elif defined(AL_PLATFORM_LINUX)
		int                fd;
		FileSystem::Path   path;
#endif

		SPIModes           mode;
		SPISpeed           speed;
		uint8              bitCount;

	public:
		SPIDevice(SPIDevice&& device)
			: isOpen(
				device.isOpen
			),
#if defined(AL_PLATFORM_PICO)
			spi(
				Move(device.spi)
			),
#elif defined(AL_PLATFORM_LINUX)
			fd(
				device.fd
			),
			path(
				Move(device.path)
			),
#endif
			mode(
				device.mode
			),
			speed(
				device.speed
			),
			bitCount(
				device.bitCount
			)
		{
			device.isOpen = false;
		}

#if defined(AL_PLATFORM_PICO)
		SPIDevice(::spi_inst_t* spi, GPIOPin miso, GPIOPin mosi, GPIOPin sclk, GPIOPin cs, SPIModes mode, SPISpeed speed, uint8 bitCount)
			: spi(
				spi,
				miso,
				mosi,
				sclk,
				cs,
				[mode]()
				{
					switch (mode)
					{
						case SPIModes::Zero:
							return Drivers::Pico::SPIModes::Zero;
							
						case SPIModes::One:
							return Drivers::Pico::SPIModes::One;
							
						case SPIModes::Two:
							return Drivers::Pico::SPIModes::Two;
							
						case SPIModes::Three:
							return Drivers::Pico::SPIModes::Three;
					}

					throw NotImplementedException();
				}(),
				speed,
				bitCount
			),
			mode(
				mode
			),
			speed(
				speed
			),
			bitCount(
				bitCount
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		SPIDevice(FileSystem::Path&& path, SPIModes mode, SPISpeed speed, uint8 bitCount)
			: path(
				Move(path)
			),
			mode(
				mode
			),
			speed(
				speed
			),
			bitCount(
				bitCount
			)
		{
		}
		SPIDevice(const FileSystem::Path& path, SPIModes mode, SPISpeed speed, uint8 bitCount)
			: SPIDevice(
				FileSystem::Path(path),
				mode,
				speed,
				bitCount
			)
		{
		}
#endif

		virtual ~SPIDevice()
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

#if defined(AL_PLATFORM_PICO)
		auto GetHandle() const
		{
			return spi.GetSPI();
		}

		auto GetCS() const
		{
			return spi.GetCS();
		}

		auto GetMISO() const
		{
			return spi.GetMISO();
		}

		auto GetMOSI() const
		{
			return spi.GetMOSI();
		}

		auto GetSCLK() const
		{
			return spi.GetSCLK();
		}
#elif defined(AL_PLATFORM_LINUX)
		auto& GetPath() const
		{
			return path;
		}
#endif

		auto GetMode() const
		{
			return mode;
		}

		auto GetSpeed() const
		{
			return speed;
		}

		auto GetBitCount() const
		{
			return bitCount;
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"SPIDevice already open"
			);

#if defined(AL_PLATFORM_PICO)
			spi.Open();
#elif defined(AL_PLATFORM_LINUX)
			if ((fd = ::open(GetPath().GetString().GetCString(), O_RDWR)) == -1)
			{

				throw OS::SystemException(
					"open"
				);
			}

			__u8 _mode;

			switch (GetMode())
			{
				case SPIModes::Zero:
					_mode = SPI_MODE_0;
					break;
					
				case SPIModes::One:
					_mode = SPI_MODE_1;
					break;
					
				case SPIModes::Two:
					_mode = SPI_MODE_2;
					break;
					
				case SPIModes::Three:
					_mode = SPI_MODE_3;
					break;
			}

			if (::ioctl(fd, SPI_IOC_WR_MODE, &_mode) == -1)
			{
				::close(
					fd
				);

				throw Exception(
					OS::SystemException(
						"ioctl"
					),
					"Error setting mode"
				);
			}

			auto _bitCount = static_cast<__u8>(
				GetBitCount()
			);

			if ((::ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &_bitCount) == -1) ||
				(::ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &_bitCount) == -1))
			{
				::close(
					fd
				);

				throw Exception(
					OS::SystemException(
						"ioctl"
					),
					"Error setting bit count"
				);
			}

			auto _speed = static_cast<__u32>(
				GetSpeed()
			);

			if ((::ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) == -1) ||
				(::ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) == -1))
			{
				::close(
						fd
				);

				throw Exception(
					OS::SystemException(
						"ioctl"
					),
					"Error setting speed"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
				spi.Close();
#elif defined(AL_PLATFORM_LINUX)
				::close(
					fd
				);
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		template<typename T>
		Void Read(T& value, Bool changeCS = False) const
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

			Read(
				&value,
				sizeof(T),
				changeCS
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size, Bool changeCS = False) const
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			spi.Read(
				lpBuffer,
				size,
				changeCS
			);
#elif defined(AL_PLATFORM_LINUX)
			::spi_ioc_transfer transfer = { };
			transfer.len           = static_cast<decltype(transfer.len)>(size);
			transfer.rx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::rx_buf)>(lpBuffer);
			transfer.tx_buf        = nullptr;
			transfer.speed_hz      = static_cast<decltype(transfer.speed_hz)>(GetSpeed());
			transfer.cs_change     = changeCS ? 1 : 0;
			transfer.bits_per_word = static_cast<decltype(transfer.bits_per_word)>(GetBitCount());

			if (::ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) == -1)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		template<typename T>
		Void Write(const T& value, Bool changeCS = False)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);
			
			Write(
				&value,
				sizeof(T),
				changeCS
			);
		}
		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size, Bool changeCS = False)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

			ReadWrite(
				nullptr,
				lpBuffer,
				size,
				changeCS
			);
		}

		// @throw AL::Exception
		template<typename T>
		Void ReadWrite(T& rx, const T& tx, Bool changeCS = False)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);
			
			ReadWrite(
				&rx,
				&tx,
				sizeof(T),
				changeCS
			);
		}
		// @throw AL::Exception
		Void ReadWrite(Void* lpRX, const Void* lpTX, size_t size, Bool changeCS = False)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			if (lpRX && lpTX)
			{
				spi.WriteRead(
					lpTX,
					lpRX,
					size,
					changeCS
				);
			}
			else if (lpRX)
			{
				spi.Read(
					lpRX,
					size,
					changeCS
				);
			}
			else if (lpTX)
			{
				spi.Write(
					lpTX,
					size,
					changeCS
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			::spi_ioc_transfer transfer = { };
			transfer.len           = static_cast<decltype(transfer.len)>(size);
			transfer.rx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::rx_buf)>(lpRX);
			transfer.tx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::tx_buf)>(lpTX);
			transfer.speed_hz      = static_cast<decltype(transfer.speed_hz)>(GetSpeed());
			transfer.cs_change     = changeCS ? 1 : 0;
			transfer.bits_per_word = static_cast<decltype(transfer.bits_per_word)>(GetBitCount());

			if (::ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) == -1)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		template<size_t S>
		Void Execute(const SPITransaction(&transactions)[S])
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

			Execute(
				&transactions[0],
				S
			);
		}
		// @throw AL::Exception
		Void Execute(const SPITransaction* lpTransactions, size_t count)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			spi.SetCSActive();

			for (size_t i = 0; i < count; ++i, ++lpTransactions)
			{
				if (lpTransactions->lpRX && lpTransactions->lpTX)
				{
					spi.WriteRead(
						lpTransactions->lpTX,
						lpTransactions->lpRX,
						lpTransactions->BufferSize,
						False
					);
				}
				else if (lpTransactions->lpRX)
				{
					spi.Read(
						lpTransactions->lpRX,
						lpTransactions->BufferSize,
						False
					);
				}
				else if (lpTransactions->lpTX)
				{
					spi.Write(
						lpTransactions->lpTX,
						lpTransactions->BufferSize,
						False
					);
				}
			}

			spi.SetCSActive(
				False
			);
#elif defined(AL_PLATFORM_LINUX)
			Collections::Array<::spi_ioc_transfer> transfers(
				count
			);

			auto lpTransfers = &transfers[0];

			for (size_t i = 0; i < count; ++i, ++lpTransfers, ++lpTransactions)
			{
				memset(
					lpTransfers,
					0,
					sizeof(::spi_ioc_transfer)
				);

				lpTransfers->len           = static_cast<decltype(::spi_ioc_transfer::len)>          (lpTransactions->BufferSize);
				lpTransfers->rx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::rx_buf)>  (lpTransactions->lpRX);
				lpTransfers->tx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::tx_buf)>  (lpTransactions->lpTX);
				lpTransfers->speed_hz      = static_cast<decltype(::spi_ioc_transfer::speed_hz)>     (GetSpeed());
				lpTransfers->cs_change     = changeCS ?                                              1 : 0;
				lpTransfers->bits_per_word = static_cast<decltype(::spi_ioc_transfer::bits_per_word)>(GetBitCount());
			}

			if (::ioctl(fd, SPI_IOC_MESSAGE(count), &transfers[0]) == -1)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		SPIDevice& operator = (SPIDevice&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = False;

#if defined(AL_PLATFORM_PICO)
			spi = Move(
				device.spi
			);
#elif defined(AL_PLATFORM_LINUX)
			fd = device.fd;

			path = Move(
				device.path
			);
#endif

			mode     = device.mode;
			speed    = device.speed;
			bitCount = device.bitCount;

			return *this;
		}

		Bool operator == (const SPIDevice& device) const
		{
			if (IsOpen() != device.IsOpen())
			{

				return False;
			}

#if defined(AL_PLATFORM_PICO)
			if (spi != device.spi)
			{

				return False;
			}
#elif defined(AL_PLATFORM_LINUX)
			if (GetPath() != device.GetPath())
			{

				return False;
			}
#endif

			if (GetMode() != device.GetMode())
			{

				return False;
			}

			if (GetSpeed() != device.GetSpeed())
			{

				return False;
			}

			if (GetBitCount() != device.GetBitCount())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const SPIDevice& device) const
		{
			if (operator==(device))
			{

				return False;
			}

			return True;
		}
	};
}
