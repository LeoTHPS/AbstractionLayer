#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "Pin.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <linux/spi/spidev.h>

namespace AL::GPIO
{
	enum class SPIModes : uint8
	{
		Zero  = SPI_MODE_0,
		One   = SPI_MODE_1,
		Two   = SPI_MODE_2,
		Three = SPI_MODE_3
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
		int fd;

		Pin csPin;
		SPICSModes csPinMode;

		SPIBusId busId;

		SPIModes deviceMode;
		SPIDeviceId deviceId;
		SPISpeed deviceSpeed;
		SPIBitCount deviceBitCount;

		SPIDevice(const SPIDevice&) = delete;

		SPIDevice(int fd, SPIBusId busId, SPIDeviceId deviceId, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: fd(
				fd
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

		SPIDevice(int fd, SPIBusId busId, Pin&& csPin, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: fd(
				fd
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

			if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitCount) == -1)
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIBitCount"
				);
			}

			if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
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
		}
		// @throw AL::Exceptions::Exception
		// @return false if CS is already exported
		static bool Open(SPIDevice& device, SPIBusId busId, DeviceId csPinDeviceId, PinNumber csPinNumber, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
		{
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

			if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitCount) == -1)
			{
				close(fd);

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"ioctl"
					),
					"Error setting SPIBitCount"
				);
			}

			if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
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

			return true;
		}

		SPIDevice()
			: SPIDevice(
				int(-1),
				SPIBusId(0),
				SPIDeviceId(0),
				SPIModes::Zero,
				SPISpeed(0),
				SPIBitCount(0)
			)
		{
		}

		SPIDevice(SPIDevice&& device)
			: fd(
				device.fd
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
			)
		{
			device.fd = -1;
		}

		virtual ~SPIDevice()
		{
			Close();
		}

		bool IsOpen() const
		{
			return fd != -1;
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

		// @throw AL::Exceptions::Exception
		template<typename T>
		void Read(T& value)
		{
			Read(
				&value,
				sizeof(T)
			);
		}

		// @throw AL::Exceptions::Exception
		void Read(void* lpBuffer, size_t size)
		{
			ReadWrite(
				lpBuffer,
				nullptr,
				size
			);
		}

		// @throw AL::Exceptions::Exception
		template<typename T>
		void Write(const T& value)
		{
			Write(
				&value,
				sizeof(T)
			);
		}

		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			ReadWrite(
				nullptr,
				lpBuffer,
				size
			);
		}

		// @throw AL::Exceptions::Exception
		template<typename T>
		void ReadWrite(T& rx, const T& tx)
		{
			ReadWrite(
				&rx,
				&tx,
				sizeof(T)
			);
		}

		// @throw AL::Exceptions::Exception
		void ReadWrite(void* lpReadBuffer, const void* lpWriteBuffer, size_t size)
		{
			spi_ioc_transfer transfer = { 0 };
			//transfer.cs_change = csPin.IsExported() ? 0 : 1;
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
		}

		void Close()
		{
			if (IsOpen())
			{
				close(fd);
				fd = -1;

				csPin.Unexport();
			}
		}

		SPIDevice& operator = (SPIDevice&& device)
		{
			Close();

			fd = device.fd;
			device.fd = -1;

			csPin = Move(device.csPin);
			csPinMode = device.csPinMode;

			busId = device.busId;
			
			deviceMode = device.deviceMode;
			deviceId = device.deviceId;
			deviceSpeed = device.deviceSpeed;
			deviceBitCount = device.deviceBitCount;

			return *this;
		}
	};
}
