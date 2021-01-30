#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/Pin.hpp"

namespace AL::GPIO::Devices
{
	class DHT22
		: public Device<void, uint16>
	{
		Pin pin;
		DeviceId deviceId;
		PinNumber pinNumber;

	public:
		DHT22(DHT22&& dht22)
			: Device(
				Move(dht22)
			),
			deviceId(
				dht22.deviceId
			),
			pinNumber(
				dht22.pinNumber
			)
		{
			dht22.deviceId = 0;
			dht22.pinNumber = 0;
		}

		DHT22(DeviceId deviceId, PinNumber pinNumber)
			: deviceId(
				deviceId
			),
			pinNumber(
				pinNumber
			)
		{
		}

		auto& operator = (DHT22&& dht22)
		{
			Device::operator=(
				Move(dht22)
			);

			pin = Move(
				dht22.pin
			);

			deviceId = dht22.deviceId;
			dht22.deviceId = 0;

			pinNumber = dht22.pinNumber;
			dht22.pinNumber = 0;

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			if (!Pin::Export(pin, deviceId, pinNumber, PinDirection::Out, PinValues::High))
			{

				throw Exceptions::Exception(
					"Pin [Bus: %u, GPIO: %u] is already exported",
					deviceId,
					pinNumber
				);
			}
		}

		virtual void OnClose() override
		{
			pin.Unexport();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnRead(uint16& value) override
		{
			throw AL::Exceptions::NotImplementedException();
		}
	};
}
