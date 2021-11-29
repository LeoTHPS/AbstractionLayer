#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/ioctl.h>

	#if __has_include(<linux/spi/spidev.h>)
		#define AL_DEPENDENCY_SPIDEV

		#include <linux/spi/spidev.h>
	#endif
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
		Bool     isOpen = False;
		Bool     isCSChangeEnabled = False;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
		int      fd;
	#endif
#endif

		String   path;
		SPIModes mode;
		SPISpeed speed;
		size_t   bitCount;

	public:
		SPIDevice(SPIDevice&& device)
			: isOpen(
				device.isOpen
			),
			isCSChangeEnabled(
				device.isCSChangeEnabled
			),
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			fd(
				device.fd
			),
	#endif
#endif
			path(
				Move(device.path)
			),
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

		SPIDevice(String&& path, SPIModes mode, SPISpeed speed, size_t bitCount)
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
		SPIDevice(const String& path, SPIModes mode, SPISpeed speed, size_t bitCount)
			: SPIDevice(
				String(path),
				mode,
				speed,
				bitCount
			)
		{
		}

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

		Bool IsCSChangeEnabled() const
		{
			return isCSChangeEnabled;
		}

		auto& GetPath() const
		{
			return path;
		}

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

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			if ((fd = ::open(GetPath().GetCString(), O_RDWR)) == -1)
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
			throw DependencyMissingException(
				"spidev"
			);
	#endif
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
				::close(
					fd
				);
	#endif
#endif

				isOpen = False;
			}
		}

		// Set True to deselect CS pin between each transfer
		Void SetCSChange(Bool set = True)
		{
			isCSChangeEnabled = set;
		}

		// @throw AL::Exception
		template<typename T>
		Void Read(T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);
			
			Read(
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

			ReadWrite(
				lpBuffer,
				nullptr,
				size
			);
		}

		// @throw AL::Exception
		template<typename T>
		Void Write(const T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);
			
			Write(
				&value,
				sizeof(T)
			);
		}
		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

			ReadWrite(
				nullptr,
				lpBuffer,
				size
			);
		}

		// @throw AL::Exception
		template<typename T>
		Void ReadWrite(T& rx, const T& tx)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);
			
			ReadWrite(
				&rx,
				&tx,
				sizeof(T)
			);
		}
		// @throw AL::Exception
		Void ReadWrite(Void* lpRX, const Void* lpTX, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"SPIDevice not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			::spi_ioc_transfer transfer = { 0 };
			transfer.len           = static_cast<decltype(transfer.len)>(size);
			transfer.rx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::rx_buf)>(lpRX);
			transfer.tx_buf        = reinterpret_cast<decltype(::spi_ioc_transfer::tx_buf)>(lpTX);
			transfer.speed_hz      = static_cast<decltype(transfer.speed_hz)>(GetSpeed());
			transfer.cs_change     = IsCSChangeEnabled() ? 1 : 0;
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

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
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
				lpTransfers->cs_change     = IsCSChangeEnabled() ?                                   1 : 0;
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
#else
			throw NotImplementedException();
#endif
		}

		SPIDevice& operator = (SPIDevice&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = False;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			fd = device.fd;
	#endif
#endif

			path = Move(
				device.path
			);

			mode = device.mode;
			speed = device.speed;
			bitCount = device.bitCount;

			return *this;
		}

		Bool operator == (const SPIDevice& device) const
		{
			if (IsOpen() ^ device.IsOpen())
			{

				return False;
			}

			if (IsOpen() && device.IsOpen())
			{
				if (GetPath() != device.GetPath())
				{

					return False;
				}

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
