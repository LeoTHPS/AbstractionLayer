#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/Pin.hpp"

namespace AL::GPIO::Devices
{
	class RCWL_0516
		: public Device<bool>
	{
		Pin pin;
		DeviceId deviceId;
		PinNumber pinNumber;

	public:
		RCWL_0516(RCWL_0516&& rcwl_0516)
			: Device(
				Move(rcwl_0516)
			),
			deviceId(
				rcwl_0516.deviceId
			),
			pinNumber(
				rcwl_0516.pinNumber
			)
		{
			rcwl_0516.deviceId = 0;
			rcwl_0516.pinNumber = 0;
		}

		RCWL_0516(DeviceId deviceId, PinNumber pinNumber)
			: deviceId(
				deviceId
			),
			pinNumber(
				pinNumber
			)
		{
		}

		auto& operator = (RCWL_0516&& rcwl_0516)
		{
			Device::operator=(
				Move(rcwl_0516)
			);

			pin = Move(
				rcwl_0516.pin
			);

			deviceId = rcwl_0516.deviceId;
			rcwl_0516.deviceId = 0;

			pinNumber = rcwl_0516.pinNumber;
			rcwl_0516.pinNumber = 0;

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			Pin::Export(
				pin,
				deviceId,
				pinNumber,
				PinDirection::In
			);
		}

		virtual void OnClose() override
		{
			pin.Unexport();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnRead(bool& value, DeviceChannel /*channel*/) override
		{
			value = (pin.Read() == 1);
		}
	};
}
