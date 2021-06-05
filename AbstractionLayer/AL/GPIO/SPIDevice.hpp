#pragma once
#include "AL/Common.hpp"

#include "Pin.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/ioctl.h>

	#if __has_include(<linux/spi/spidev.h>)
		#include <linux/spi/spidev.h>

		#define AL_DEPENDENCY_SPIDEV
	#endif
#endif

namespace AL::GPIO
{
	enum class SPIModes : uint8
	{
#if defined(AL_DEPENDENCY_SPIDEV)
		Zero  = SPI_MODE_0,
		One   = SPI_MODE_1,
		Two   = SPI_MODE_2,
		Three = SPI_MODE_3
#else
		Zero  = 0,
		One   = 1,
		Two   = 2,
		Three = 3
#endif
	};

	typedef uint8 SPIBusId;

	typedef uint32 SPISpeed;

	typedef uint8 SPIBitCount;

	typedef uint8 SPIDeviceId;

	enum class SPICSModes : uint8
	{
		ActiveLow,
		ActiveHigh
	};

	class SPIDevice
	{
#if defined(AL_PLATFORM_LINUX)
		typedef int HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = -1;
#else
		typedef void* HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = nullptr;
#endif

		HFILE hFile;

		Pin csPin;
		SPICSModes csPinMode;

		SPIBusId busId;

		SPIModes deviceMode;
		SPIDeviceId deviceId;
		SPISpeed deviceSpeed;
		SPIBitCount deviceBitCount;

		bool isCsChangeEnabled = false;

		SPIDevice(const SPIDevice&) = delete;

		SPIDevice(HFILE hFile, SPIBusId busId, SPIDeviceId deviceId, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: hFile(
				hFile
			),
			busId(
				busId
			),
			deviceMode(
				mode
			),
			deviceId(
				deviceId
			),
			deviceSpeed(
				speed
			),
			deviceBitCount(
				bitCount
			)
		{
		}

		SPIDevice(HFILE hFile, SPIBusId busId, Pin&& csPin, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: hFile(
				hFile
			),
			csPin(
				Move(csPin)
			),
			csPinMode(
				csPinMode
			),
			busId(
				busId
			),
			deviceMode(
				mode
			),
			deviceId(
				0
			),
			deviceSpeed(
				speed
			),
			deviceBitCount(
				bitCount
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		static void Open(SPIDevice& device, SPIBusId busId, SPIDeviceId deviceId, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			int fd;
			char path[255];
			sprintf(path, "/dev/spidev%u.%u", busId, deviceId);

			if ((fd = open(path, O_RDWR)) == -1)
			{

				throw Exceptions::SystemException(
					"open"
				);
			}

			if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1)
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIMode"
				);
			}

			if ((ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitCount) == -1) ||
				(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitCount) == -1))
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIBitCount"
				);
			}

			if ((ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) ||
				(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1))
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPISpeed"
				);
			}

			device = SPIDevice(
				fd,
				busId,
				deviceId,
				mode,
				speed,
				bitCount
			);
	#else
			throw Exceptions::DependencyMissingException(
				"spidev"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		// @return false if CS is already exported
		static bool Open(SPIDevice& device, SPIBusId busId, DeviceId csPinDeviceId, PinNumber csPinNumber, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			Pin csPin;

			if (!Pin::Export(csPin, csPinDeviceId, csPinNumber, PinDirection::Out, (csPinMode == SPICSModes::ActiveLow) ? PinValues::High : PinValues::Low))
			{

				return false;
			}

			int fd;
			char path[255];
			sprintf(path, "/dev/spidev%u.0", busId);

			if ((fd = open(path, O_RDWR)) == -1)
			{

				throw Exceptions::SystemException(
					"open"
				);
			}

			uint8 _mode = static_cast<uint8>(mode) | SPI_NO_CS;

			if (ioctl(fd, SPI_IOC_WR_MODE, &_mode) == -1)
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIMode"
				);
			}

			if ((ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitCount) == -1) ||
				(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitCount) == -1))
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIBitCount"
				);
			}

			if ((ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) ||
				(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1))
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPISpeed"
				);
			}

			device = SPIDevice(
				fd,
				busId,
				Move(csPin),
				csPinMode,
				mode,
				speed,
				bitCount
			);
	#else
			throw Exceptions::DependencyMissingException(
				"spidev"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif

			return true;
		}

		SPIDevice()
			: SPIDevice(
				FILE_HANDLE_NULL,
				SPIBusId(0),
				SPIDeviceId(0),
				SPIModes::Zero,
				SPISpeed(0),
				SPIBitCount(0)
			)
		{
		}

		SPIDevice(SPIDevice&& device)
			: hFile(
				device.hFile
			),
			csPin(
				Move(device.csPin)
			),
			csPinMode(
				device.csPinMode
			),
			busId(
				device.busId
			),
			deviceMode(
				device.deviceMode
			),
			deviceId(
				device.deviceId
			),
			deviceSpeed(
				device.deviceSpeed
			),
			deviceBitCount(
				device.deviceBitCount
			),
			isCsChangeEnabled(
				device.isCsChangeEnabled
			)
		{
			device.hFile = FILE_HANDLE_NULL;
			device.isCsChangeEnabled = false;
		}

		virtual ~SPIDevice()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hFile != FILE_HANDLE_NULL;
		}

		bool IsCSChangeEnabled() const
		{
			return isCsChangeEnabled;
		}

		auto GetId() const
		{
			return deviceId;
		}

		auto GetBusId() const
		{
			return busId;
		}

		auto GetMode() const
		{
			return deviceMode;
		}

		auto GetSpeed() const
		{
			return deviceSpeed;
		}

		auto GetBitCount() const
		{
			return deviceBitCount;
		}

		// Set true to deselect CS pin between each transfer
		// This is only supported when using native CS pins, not GPIO
		void SetCSChange(bool set = true)
		{
			isCsChangeEnabled = set;
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Read(T& value)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

			Read(
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

			ReadWrite(
				lpBuffer,
				nullptr,
				size
			);
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Write(const T& value)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

			Write(
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

			ReadWrite(
				nullptr,
				lpBuffer,
				size
			);
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void ReadWrite(T& rx, const T& tx)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

			ReadWrite(
				&rx,
				&tx,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void ReadWrite(void* lpReadBuffer, const void* lpWriteBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "SPIDevice not open");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
			spi_ioc_transfer transfer = { 0 };
			transfer.cs_change = IsCSChangeEnabled() ? 1 : 0;
			transfer.len = static_cast<decltype(transfer.len)>(size);
			transfer.speed_hz = static_cast<decltype(transfer.speed_hz)>(GetSpeed());
			transfer.bits_per_word = static_cast<decltype(transfer.bits_per_word)>(GetBitCount());
			transfer.rx_buf = reinterpret_cast<decltype(spi_ioc_transfer::rx_buf)>(lpReadBuffer);
			transfer.tx_buf = reinterpret_cast<decltype(spi_ioc_transfer::tx_buf)>(lpWriteBuffer);

			if (csPin.IsExported())
			{

				csPin.Write(
					(csPinMode == SPICSModes::ActiveLow) ? PinValues::Low : PinValues::High
				);
			}
			
			if (ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) == -1)
			{
				if (csPin.IsExported())
				{

					csPin.Write(
						(csPinMode == SPICSModes::ActiveLow) ? PinValues::High : PinValues::Low
					);
				}

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error operating on spi_ioc_transfer"
				);
			}

			if (csPin.IsExported())
			{

				csPin.Write(
					(csPinMode == SPICSModes::ActiveLow) ? PinValues::High : PinValues::Low
				);
			}
	#else
			throw Exceptions::DependencyMissingException(
				"spidev"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_SPIDEV)
				close(hFile);
				hFile = FILE_HANDLE_NULL;

				csPin.Unexport();
	#endif
#endif
			}
		}

		SPIDevice& operator = (SPIDevice&& device)
		{
			Close();

			hFile = device.hFile;
			device.hFile = FILE_HANDLE_NULL;

			csPin = Move(device.csPin);
			csPinMode = device.csPinMode;

			busId = device.busId;
			
			deviceMode = device.deviceMode;
			deviceId = device.deviceId;
			deviceSpeed = device.deviceSpeed;
			deviceBitCount = device.deviceBitCount;

			isCsChangeEnabled = device.isCsChangeEnabled;
			device.isCsChangeEnabled = false;

			return *this;
		}
	};
}
