#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/SPIDevice.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class SPIDevice
		: public Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>
	{
		struct DeviceInfo
		{
			SPIBusId    SPI_BusId;
			SPIDeviceId SPI_DeviceId;
			SPIModes    SPI_Mode;
			SPISpeed    SPI_Speed;
			SPIBitCount SPI_BitCount;

			DeviceId    SPI_CS_DeviceId;
			PinNumber   SPI_CS_PinNumber;
			SPICSModes  SPI_CS_Mode;

			DeviceInfo(SPIBusId busId, SPIDeviceId deviceId, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
				: SPI_BusId(
					busId
				),
				SPI_DeviceId(
					deviceId
				),
				SPI_Mode(
					mode
				),
				SPI_Speed(
					speed
				),
				SPI_BitCount(
					bitCount
				)
			{
			}

			DeviceInfo(SPIBusId busId, DeviceId csPinDeviceId, PinNumber csPinNumber, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
				: SPI_BusId(
					busId
				),
				SPI_Mode(
					mode
				),
				SPI_Speed(
					speed
				),
				SPI_BitCount(
					bitCount
				),
				SPI_CS_DeviceId(
					csPinDeviceId
				),
				SPI_CS_PinNumber(
					csPinNumber
				),
				SPI_CS_Mode(
					csPinMode
				)
			{
			}
		};

		GPIO::SPIDevice device;
		DeviceInfo deviceInfo;

		bool useCustomCS;

	public:
		SPIDevice(SPIDevice&& spiDevice)
			: Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>(
				Move(spiDevice)
			),
			device(
				Move(spiDevice.device)
			),
			deviceInfo(
				Move(spiDevice.deviceInfo)
			),
			useCustomCS(
				spiDevice.useCustomCS
			)
		{
			spiDevice.useCustomCS = false;
		}

		SPIDevice(SPIBusId busId, SPIDeviceId deviceId, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: deviceInfo(
				busId,
				deviceId,
				mode,
				speed,
				bitCount
			),
			useCustomCS(
				false
			)
		{
		}

		SPIDevice(SPIBusId busId, DeviceId csPinDeviceId, PinNumber csPinNumber, SPICSModes csPinMode, SPIModes mode, SPISpeed speed, SPIBitCount bitCount)
			: deviceInfo(
				busId,
				csPinDeviceId,
				csPinNumber,
				csPinMode,
				mode,
				speed,
				bitCount
			),
			useCustomCS(
				true
			)
		{
		}

		// Set true to deselect CS pin between each transfer
		// This is only supported when using native CS pins, not GPIO
		void SetCSChange(bool set = true)
		{
			device.SetCSChange(
				set
			);
		}

		auto& operator = (SPIDevice&& spiDevice)
		{
			Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>::operator=(
				Move(spiDevice)
			);

			device = Move(
				spiDevice.device
			);

			deviceInfo = Move(
				spiDevice.deviceInfo
			);

			useCustomCS = spiDevice.useCustomCS;
			spiDevice.useCustomCS = false;

			return *this;
		}

	protected:
		auto& GetDevice()
		{
			return device;
		}
		auto& GetDevice() const
		{
			return device;
		}

		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			if (!useCustomCS)
			{
				GPIO::SPIDevice::Open(
					device,
					deviceInfo.SPI_BusId,
					deviceInfo.SPI_DeviceId,
					deviceInfo.SPI_Mode,
					deviceInfo.SPI_Speed,
					deviceInfo.SPI_BitCount
				);
			}
			else
			{
				GPIO::SPIDevice::Open(
					device,
					deviceInfo.SPI_BusId,
					deviceInfo.SPI_CS_DeviceId,
					deviceInfo.SPI_CS_PinNumber,
					deviceInfo.SPI_CS_Mode,
					deviceInfo.SPI_Mode,
					deviceInfo.SPI_Speed,
					deviceInfo.SPI_BitCount
				);
			}
		}

		virtual void OnClose() override
		{
			device.Close();
		}
	};
}
