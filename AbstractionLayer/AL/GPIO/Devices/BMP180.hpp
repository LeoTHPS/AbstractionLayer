#pragma once
#include "AL/Common.hpp"

#include "I2CDevice.hpp"

namespace AL::GPIO::Devices
{
	struct BMP180Data
	{
		double Altitude;
		double Pressure;
		double Temperature;
	};

	enum class BMP180DataFlags : uint16
	{
		OSS_0         = 0x1,
		OSS_1         = 0x2,
		OSS_2         = 0x4,
		OSS_3         = 0x8,

		Pressure_Pa   = 0x10,
		Pressure_hPa  = 0x20,
		Pressure_inHg = 0x40,
		Pressure_mmHg = 0x80,

		Temperature_C = 0x100,
		Temperature_F = 0x200,

		Altitude_M    = 0x1000,
		Altitude_FT   = 0x2000,
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(BMP180DataFlags);

	class BMP180
		: public I2CDevice<BMP180DataFlags, BMP180Data, void>
	{
#pragma pack(push, 1)
		struct bmp_180_device_info
		{
			uint8 Id;
			uint8 Version;
		};

		struct bmp_180_calibration_data
		{
			int16  AC1;
			int16  AC2;
			int16  AC3;
			uint16 AC4;
			uint16 AC5;
			uint16 AC6;
			int16  B1;
			int16  B2;
			int16  MB;
			int16  MC;
			int16  MD;
		};

		struct bmp_180_uncompensated_pressure_data
		{
			uint8 MSB;
			uint8 LSB;
			uint8 XLSB;
		};

		struct bmp_180_uncompensated_temperature_data
		{
			uint8 MSB;
			uint8 LSB;
		};
#pragma pack(pop)

		double refPressure;

	public:
		BMP180(BMP180&& bmp180)
			: I2CDevice(
				Move(bmp180)
			)
		{
		}

		BMP180(GPIO::I2CBus& bus, I2CAddress address = 0x77)
			: I2CDevice(
				bus,
				address
			)
		{
			SetReferencePressure_hPa(
				1013.25
			);
		}

		auto GetReferencePressure_Pa() const
		{
			return refPressure;
		}
		auto GetReferencePressure_hPa() const
		{
			return GetReferencePressure_Pa() * 0.01;
		}
		auto GetReferencePressure_inHg() const
		{
			return GetReferencePressure_Pa() * 0.0002953;
		}
		auto GetReferencePressure_mmHg() const
		{
			return GetReferencePressure_Pa() * 0.00750062;
		}

		void SetReferencePressure_Pa(double value)
		{
			refPressure = value;
		}
		void SetReferencePressure_hPa(double value)
		{
			SetReferencePressure_Pa(
				value / 0.01
			);
		}
		void SetReferencePressure_inHg(double value)
		{
			SetReferencePressure_Pa(
				value / 0.0002953
			);
		}
		void SetReferencePressure_mmHg(double value)
		{
			SetReferencePressure_Pa(
				value / 0.00750062
			);
		}

		// @throw AL::Exceptions::Exception
		void ReadDeviceInfo(uint8& chipId, uint8& chipVersion)
		{
			bmp_180_device_info info;
			GetDevice().ReadRegister(0xD0, info);

			chipId = info.Id;
			chipVersion = info.Version;
		}

		auto& operator = (BMP180&& bmp180)
		{
			I2CDevice::operator=(
				Move(bmp180)
			);

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnRead(Channel channel, DataR& value) override
		{
			BitMask<BMP180DataFlags> flags(
				channel
			);

			auto oss = GetOssFromFlags(
				flags
			);

			// read factory calibration data
			bmp_180_calibration_data calibration_data;
			GetDevice().ReadRegister(0xAA, calibration_data);
			calibration_data.AC1 = BitConverter::FromBigEndian(calibration_data.AC1);
			calibration_data.AC2 = BitConverter::FromBigEndian(calibration_data.AC2);
			calibration_data.AC3 = BitConverter::FromBigEndian(calibration_data.AC3);
			calibration_data.AC4 = BitConverter::FromBigEndian(calibration_data.AC4);
			calibration_data.AC5 = BitConverter::FromBigEndian(calibration_data.AC5);
			calibration_data.AC6 = BitConverter::FromBigEndian(calibration_data.AC6);
			calibration_data.B1 = BitConverter::FromBigEndian(calibration_data.B1);
			calibration_data.B2 = BitConverter::FromBigEndian(calibration_data.B2);
			calibration_data.MB = BitConverter::FromBigEndian(calibration_data.MB);
			calibration_data.MC = BitConverter::FromBigEndian(calibration_data.MC);
			calibration_data.MD = BitConverter::FromBigEndian(calibration_data.MD);

			// tell device to begin sampling temperature
			GetDevice().WriteRegister<uint8>(0xF4, 0x2E);
			
			// wait
			Sleep(TimeSpan::FromMicroseconds(4500));

			// read uncompensated temperature
			bmp_180_uncompensated_temperature_data temperature_data;
			GetDevice().ReadRegister(0xF6, temperature_data);

			int32 temperature_uncompensated = (temperature_data.MSB << 8) + temperature_data.LSB;
			int32 temperature_x1 = ((temperature_uncompensated - calibration_data.AC6) * calibration_data.AC5) >> 15;
			int32 temperature_x2 = (calibration_data.MC << 11) / (temperature_x1 + calibration_data.MD);
			int32 temperature_b5 = temperature_x1 + temperature_x2;
			int32 temperature = (temperature_b5 + 8) >> 4;

			// tell device to begin sampling pressure
			GetDevice().WriteRegister<uint8>(0xF4, 0x34 + (oss << 6));
			
			// wait
			Sleep(!oss ? TimeSpan::FromMicroseconds(4500) : TimeSpan::FromMilliseconds((3 << oss) + 1));

			// read uncompensated pressure
			bmp_180_uncompensated_pressure_data pressure_data;
			GetDevice().ReadRegister(0xF6, pressure_data);

			int32 pressure_uncompensated = ((pressure_data.MSB << 16) + (pressure_data.LSB << 8) + pressure_data.XLSB) >> (8 - oss);
			int32 pressure_b6 = temperature_b5 - 4000;
			int32 pressure_x1 = (calibration_data.B2 * ((pressure_b6 * pressure_b6) >> 12)) >> 11;
			int32 pressure_x2 = (calibration_data.AC2 * pressure_b6) >> 11;
			int32 pressure_x3 = pressure_x1 + pressure_x2;
			int32 pressure_b3 = (((calibration_data.AC1 * 4 + pressure_x3) << oss) + 2) / 4;
			pressure_x1 = (calibration_data.AC3 * pressure_b6) >> 13;
			pressure_x2 = (calibration_data.B1 * ((pressure_b6 * pressure_b6) >> 12)) >> 16;
			pressure_x3 = ((pressure_x1 + pressure_x2) + 2) >> 2;
			uint32 pressure_b4 = (calibration_data.AC4 * static_cast<uint32>(pressure_x3 + 32768)) >> 15;
			uint32 pressure_b7 = static_cast<uint32>(pressure_uncompensated - pressure_b3) * (50000 >> oss);
			int32 pressure = (pressure_b7 < 0x80000000) ? ((pressure_b7 << 1) / pressure_b4) : ((pressure_b7 / pressure_b4) << 1);
			pressure_x1 = (pressure >> 8) * (pressure >> 8);
			pressure_x1 = (pressure_x1 * 3038) >> 16;
			pressure_x2 = (-7357 * pressure) >> 16;
			pressure += (pressure_x1 + pressure_x2 + 3791) >> 4;

			if (flags.IsSet(BMP180DataFlags::Pressure_Pa))
			{

				value.Pressure = pressure;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_hPa))
			{

				value.Pressure = pressure * 0.01;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_inHg))
			{

				value.Pressure = pressure * 0.0002953;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_mmHg))
			{

				value.Pressure = pressure * 0.00750062;
			}

			if (flags.IsSet(BMP180DataFlags::Temperature_C))
			{

				value.Temperature = temperature / 10.0;
			}
			else if (flags.IsSet(BMP180DataFlags::Temperature_F))
			{

				value.Temperature = ((temperature / 10.0) * 1.8) + 32.0;
			}

			if (flags.IsSet(BMP180DataFlags::Altitude_M))
			{

				value.Altitude = 44330.0 * (1.0 - Math::Pow(pressure / GetReferencePressure_Pa(), 0.1903));
			}
			else if (flags.IsSet(BMP180DataFlags::Altitude_FT))
			{

				value.Altitude = (44330.0 * (1.0 - Math::Pow(pressure / GetReferencePressure_Pa(), 0.1903))) * 3.281;
			}
		}

	private:
		static uint8 GetOssFromFlags(const BitMask<BMP180DataFlags>& flags)
		{
			uint8 oss = 0;

			if (flags.IsSet(BMP180DataFlags::OSS_1))
			{

				oss = 1;
			}

			if (flags.IsSet(BMP180DataFlags::OSS_2))
			{

				oss = 2;
			}

			if (flags.IsSet(BMP180DataFlags::OSS_3))
			{

				oss = 3;
			}

			return oss;
		}
	};
}
