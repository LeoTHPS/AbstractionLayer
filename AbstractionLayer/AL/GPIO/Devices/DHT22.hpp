#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/Pin.hpp"

namespace AL::GPIO::Devices
{
	struct DHT22Data
	{
		double DewPoint;
		double Humidity;
		double Temperature;
	};

	enum class DHT22DataTypes : uint8
	{
		Metric,
		Imperial
	};

	class DHT22
		: public Device<DHT22DataTypes, DHT22Data, void>
	{
		Pin pin;
		DeviceId deviceId;
		PinNumber pinNumber;

	public:
		DHT22(DHT22&& dht22)
			: Device(
				Move(dht22)
			),
			pin(
				Move(dht22.pin)
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
		virtual void OnRead(Channel channel, DataR& value) override
		{
			pin.Write(PinValues::Low);
			Spin(TimeSpan::FromMilliseconds(18));

			pin.Write(PinValues::High);
			Spin(TimeSpan::FromMicroseconds(40));

			pin.SetDirection(PinDirection::In);

			pin.WaitForEdge(PinEdges::Rising);
			pin.WaitForEdge(PinEdges::Falling);

			uint64 data = 0;

			for (size_t i = 0; i < 40; ++i)
			{
				pin.WaitForEdge(PinEdges::Rising);

				OS::Timer timer;

				pin.WaitForEdge(PinEdges::Falling);

				bool bitIsSet = timer.GetElapsed() > TimeSpan::FromMicroseconds(40);

				data |= uint64((bitIsSet ? 1 : 0) << (64 - (24 + i)));
			}

			uint8 b0((data >> 32) & 0x00000000000000FF);
			uint8 b1((data >> 24) & 0x00000000000000FF);
			uint8 b2((data >> 16) & 0x00000000000000FF);
			uint8 b3((data >> 8)  & 0x00000000000000FF);
			uint8 b4(data         & 0x00000000000000FF);

			OS::Console::WriteLine(
				"data = %010llX, b0 = %02X, b1 = %02X, b2 = %02X, b3 = %02X, b4 = %02X",
				data,
				b0,
				b1,
				b2,
				b3,
				b4
			);

			if (false && (b0 + b1 + b2 + b3) != b4)
			{

				throw Exceptions::Exception(
					"Invalid checksum"
				);
			}

			value.DewPoint = 0;
			value.Humidity = ((b0 << 8) + b1) / 10.0;
			value.Temperature = (((b2 & 0x7F) << 8) + b3) / 10.0;

			if (b2 & 0x80)
			{

				value.Temperature = -value.Temperature;
			}
		}
	};
}
