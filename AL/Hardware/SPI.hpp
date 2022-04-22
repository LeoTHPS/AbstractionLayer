#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "GPIO.hpp"

	#include <hardware/spi.h>
#elif defined(AL_PLATFORM_LINUX)
	#include "AL/FileSystem/Path.hpp"

	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/ioctl.h>

	#if __has_include(<linux/spi/spidev.h>)
		#define AL_DEPENDENCY_SPIDEV

		#include <linux/spi/spidev.h>
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/FileSystem/Path.hpp"
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
		Bool             isOpen = False;
		Bool             isCSChangeEnabled = False;

#if defined(AL_PLATFORM_PICO)
		::spi_inst_t*    spi;

		GPIOPin          cs;
		GPIOPin          miso;
		GPIOPin          mosi;
		GPIOPin          sclk;
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
		int              fd;
	#endif
#endif

#if !defined(AL_PLATFORM_PICO)
		FileSystem::Path path;
#endif
		SPIModes         mode;
		SPISpeed         speed;
		size_t           bitCount;

	public:
		SPIDevice(SPIDevice&& device)
			: isOpen(
				device.isOpen
			),
			isCSChangeEnabled(
				device.isCSChangeEnabled
			),
#if defined(AL_PLATFORM_PICO)
			cs(
				device.cs
			),
			miso(
				device.miso
			),
			mosi(
				device.mosi
			),
			sclk(
				device.sclk
			),
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			fd(
				device.fd
			),
	#endif
#endif
#if !defined(AL_PLATFORM_PICO)
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
		SPIDevice(::spi_inst_t* spi, GPIOPin miso, GPIOPin mosi, GPIOPin sclk, GPIOPin cs, SPIModes mode, SPISpeed speed, size_t bitCount)
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
#else
		SPIDevice(FileSystem::Path&& path, SPIModes mode, SPISpeed speed, size_t bitCount)
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
		SPIDevice(const FileSystem::Path& path, SPIModes mode, SPISpeed speed, size_t bitCount)
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

		Bool IsCSChangeEnabled() const
		{
			return isCSChangeEnabled;
		}

#if defined(AL_PLATFORM_PICO)
		auto GetHandle() const
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
#else
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
			::spi_init(
				GetHandle(),
				GetSpeed()
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

			::gpio_init(
				GetCS()
			);

			::gpio_set_dir(
				GetCS(),
				true
			);

			::gpio_put(
				GetCS(),
				true
			);

			::spi_set_format(
				GetHandle(),
				8,
				GetClockPolarityFromMode(GetMode()),
				GetClockPhaseFromMode(GetMode()),
				::SPI_MSB_FIRST
			);
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
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
#if defined(AL_PLATFORM_PICO)
				::spi_deinit(
					GetHandle()
				);
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO)
			::gpio_put(
				GetCS(),
				false
			);

			if (lpRX && lpTX)
			{
				::spi_write_read_blocking(
					GetHandle(),
					reinterpret_cast<const ::uint8_t*>(lpTX),
					reinterpret_cast<::uint8_t*>(lpRX),
					size
				);
			}
			else if (lpRX)
			{
				::spi_read_blocking(
					GetHandle(),
					0,
					reinterpret_cast<::uint8_t*>(lpRX),
					size
				);
			}
			else if (lpTX)
			{
				::spi_write_blocking(
					GetHandle(),
					reinterpret_cast<const ::uint8_t*>(lpTX),
					size
				);
			}

			::gpio_put(
				GetCS(),
				true
			);
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			::spi_ioc_transfer transfer = { };
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

#if defined(AL_PLATFORM_PICO)
			::gpio_put(
				GetCS(),
				false
			);

			for (size_t i = 0; i < count; ++i, ++lpTransactions)
			{
				if (lpTransactions->lpRX && lpTransactions->lpTX)
				{
					::spi_write_read_blocking(
						GetHandle(),
						reinterpret_cast<const ::uint8_t*>(lpTransactions->lpTX),
						reinterpret_cast<::uint8_t*>(lpTransactions->lpRX),
						lpTransactions->BufferSize
					);
				}
				else if (lpTransactions->lpRX)
				{
					::spi_read_blocking(
						GetHandle(),
						0,
						reinterpret_cast<::uint8_t*>(lpTransactions->lpRX),
						lpTransactions->BufferSize
					);
				}
				else if (lpTransactions->lpTX)
				{
					::spi_write_blocking(
						GetHandle(),
						reinterpret_cast<const ::uint8_t*>(lpTransactions->lpTX),
						lpTransactions->BufferSize
					);
				}
			}

			::gpio_put(
				GetCS(),
				true
			);
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO)
			spi = device.spi;
			device.spi = nullptr;

			cs = device.cs;
			miso = device.miso;
			mosi = device.mosi;
			sclk = device.sclk;
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			fd = device.fd;
	#endif
#endif

#if !defined(AL_PLATFORM_PICO)
			path = Move(
				device.path
			);
#endif

			mode = device.mode;
			speed = device.speed;
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
			if (GetCS() != device.GetCS())
			{

				return False;
			}

			if (GetMISO() != device.GetMISO())
			{

				return False;
			}

			if (GetMOSI() != device.GetMOSI())
			{

				return False;
			}

			if (GetSCLK() != device.GetSCLK())
			{

				return False;
			}

			if (GetHandle() != device.GetHandle())
			{

				return False;
			}
#else
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

	private:
#if defined(AL_PLATFORM_PICO)
		Void GetClockEdgeFromMode(SPIModes value)
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

		::spi_cpha_t GetClockPhaseFromMode(SPIModes value)
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

		::spi_cpol_t GetClockPolarityFromMode(SPIModes value)
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
#endif
	};
}
