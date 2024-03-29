#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/I2C.hpp"

namespace AL::Hardware::Drivers
{
	struct BMP180Data
	{
		Double Altitude;
		Double Pressure;
		Double Temperature;
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
		: public IDriver<BMP180DataFlags, BMP180Data, Void>
	{
#pragma pack(push, 1)
		struct device_info
		{
			uint8 Id;
			uint8 Version;
		};

		struct calibration_data
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

		struct uncompensated_pressure_data
		{
			uint8 MSB;
			uint8 LSB;
			uint8 XLSB;
		};

		struct uncompensated_temperature_data
		{
			uint8 MSB;
			uint8 LSB;
		};
#pragma pack(pop)

		Bool             isOpen         = False;
		Bool             isBusAllocated = False;

		I2CBus*          lpBus;
		I2CDevice        device;

		Double           refPressure;
		calibration_data calibrationData;

	public:
		inline static const I2CAddress DEVICE_ADDRESS = 0x77;

		static constexpr uint32        DEFAULT_SPEED  = 3000000;

		BMP180(BMP180&& bmp180)
			: isOpen(
				bmp180.isOpen
			),
			isBusAllocated(
				bmp180.isBusAllocated
			),
			lpBus(
				bmp180.lpBus
			),
			device(
				Move(bmp180.device)
			),
			refPressure(
				bmp180.refPressure
			),
			calibrationData(
				Move(bmp180.calibrationData)
			)
		{
			bmp180.isOpen         = False;
			bmp180.isBusAllocated = False;
		}

		BMP180(I2CBus& bus, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				False
			),
			lpBus(
				&bus
			),
			device(
				bus,
				address
			)
		{
			SetReferencePressure_hPa(
				1013.25
			);
		}

#if defined(AL_PLATFORM_PICO)
		BMP180(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					i2c,
					scl,
					sda,
					baud
				)
			),
			device(
				*lpBus,
				address
			)
		{
			SetReferencePressure_hPa(
				1013.25
			);
		}
#elif defined(AL_PLATFORM_LINUX)
		BMP180(FileSystem::Path&& path, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					Move(path),
					baud
				)
			),
			device(
				*lpBus,
				address
			)
		{
			SetReferencePressure_hPa(
				1013.25
			);
		}
		BMP180(const FileSystem::Path& path, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS)
			: BMP180(
				FileSystem::Path(path),
				baud,
				address
			)
		{
		}
#endif

		virtual ~BMP180()
		{
			if (IsOpen())
			{

				Close();
			}

			if (isBusAllocated)
			{
				delete lpBus;

				isBusAllocated = False;
			}
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
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

		Void SetReferencePressure_Pa(Double value)
		{
			refPressure = value;
		}
		Void SetReferencePressure_hPa(Double value)
		{
			SetReferencePressure_Pa(
				value / 0.01
			);
		}
		Void SetReferencePressure_inHg(Double value)
		{
			SetReferencePressure_Pa(
				value / 0.0002953
			);
		}
		Void SetReferencePressure_mmHg(Double value)
		{
			SetReferencePressure_Pa(
				value / 0.00750062
			);
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"BMP180 already open"
			);

			if (isBusAllocated)
			{
				try
				{
					lpBus->Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening I2CBus"
					);
				}
			}

			try
			{
				SoftReset();
			}
			catch (Exception& exception)
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				throw Exception(
					Move(exception),
					"Error soft resetting device"
				);
			}

			try
			{
				ReadCalibrationData(
					calibrationData
				);
			}
			catch (Exception& exception)
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				throw Exception(
					Move(exception),
					"Error reading calibration data"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Bool TestIO()
		{
			AL_ASSERT(
				IsOpen(),
				"BMP180 not open"
			);

			uint8 chipId;

			try
			{
				device.WriteRead(
					uint8(0xD0),
					chipId
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading chip id"
				);
			}

			return chipId == 0x55;
		}

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"BMP180 not open"
			);

			BitMask<BMP180DataFlags> flags(
				channel
			);

			auto oss = GetOssFromFlags(
				flags
			);

			int32 pressure, temperature, temperature_b5;
			MeasureAndReadTemperature(temperature, temperature_b5);
			MeasureAndReadPressure(pressure, oss, temperature_b5);

			if (flags.IsSet(BMP180DataFlags::Pressure_Pa))
			{

				data.Pressure = pressure;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_hPa))
			{

				data.Pressure = pressure * 0.01;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_inHg))
			{

				data.Pressure = pressure * 0.0002953;
			}
			else if (flags.IsSet(BMP180DataFlags::Pressure_mmHg))
			{

				data.Pressure = pressure * 0.00750062;
			}

			if (flags.IsSet(BMP180DataFlags::Temperature_C))
			{

				data.Temperature = temperature / 10.0;
			}
			else if (flags.IsSet(BMP180DataFlags::Temperature_F))
			{

				data.Temperature = ((temperature / 10.0) * 1.8) + 32.0;
			}

			if (flags.IsSet(BMP180DataFlags::Altitude_M))
			{

				data.Altitude = 44330.0 * (1.0 - Math::Pow(pressure / GetReferencePressure_Pa(), 0.1903));
			}
			else if (flags.IsSet(BMP180DataFlags::Altitude_FT))
			{

				data.Altitude = (44330.0 * (1.0 - Math::Pow(pressure / GetReferencePressure_Pa(), 0.1903))) * 3.281;
			}
		}

		// @throw AL::Exception
		virtual Void ReadDeviceInfo(uint8& chipId, uint8& chipVersion)
		{
			AL_ASSERT(
				IsOpen(),
				"BMP180 not open"
			);

			device_info info;

			try
			{
				device.WriteRead(
					uint8(0xD0),
					info
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading device info registers"
				);
			}

			chipId      = info.Id;
			chipVersion = info.Version;
		}

		BMP180& operator = (BMP180&& bmp180)
		{
			Close();

			isOpen = bmp180.isOpen;
			bmp180.isOpen = False;

			isBusAllocated = bmp180.isBusAllocated;
			bmp180.isBusAllocated = False;

			lpBus = bmp180.lpBus;

			device = Move(
				bmp180.device
			);

			refPressure = bmp180.refPressure;

			calibrationData = Move(
				bmp180.calibrationData
			);

			return *this;
		}

		Bool operator == (const BMP180& bmp180) const
		{
			if (device != bmp180.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const BMP180& bmp180) const
		{
			if (operator==(bmp180))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		Void SoftReset()
		{
			uint8 value[] =
			{
				0xAA,
				0xB6
			};

			try
			{
				device.Write(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing soft reset register"
				);
			}
		}

		// @throw AL::Exception
		Void ReadCalibrationData(calibration_data& value)
		{
			try
			{
				device.WriteRead(
					uint8(0xAA),
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading calibration register"
				);
			}

			for (size_t i = 0; i < sizeof(calibration_data); i += 2)
			{
				if (auto word = reinterpret_cast<const uint16*>(&value)[i / 2]; (word == 0x0000) || (word == 0xFFFF))
				{

					throw Exception(
						"Error communicating with device"
					);
				}
			}

			value.AC1 = BitConverter::FromBigEndian(value.AC1);
			value.AC2 = BitConverter::FromBigEndian(value.AC2);
			value.AC3 = BitConverter::FromBigEndian(value.AC3);
			value.AC4 = BitConverter::FromBigEndian(value.AC4);
			value.AC5 = BitConverter::FromBigEndian(value.AC5);
			value.AC6 = BitConverter::FromBigEndian(value.AC6);
			value.B1  = BitConverter::FromBigEndian(value.B1);
			value.B2  = BitConverter::FromBigEndian(value.B2);
			value.MB  = BitConverter::FromBigEndian(value.MB);
			value.MC  = BitConverter::FromBigEndian(value.MC);
			value.MD  = BitConverter::FromBigEndian(value.MD);
		}

		// @throw AL::Exception
		Void MeasureAndReadPressure(int32& value, uint8 oss, int32 temperature_b5)
		{
			{
				uint8 data[2] =
				{
					0xF4,
					static_cast<uint8>(0x34 + (oss << 6))
				};

				try
				{
					device.Write(
						data
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error triggering device"
					);
				}
			}

			Sleep(
				!oss ? TimeSpan::FromMicroseconds(4500) : TimeSpan::FromMilliseconds((3 << oss) + 1)
			);

			uncompensated_pressure_data pressure_data;

			try
			{
				device.WriteRead(
					uint8(0xF6),
					pressure_data
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading pressure data"
				);
			}

			int32 pressure_uncompensated = ((pressure_data.MSB << 16) + (pressure_data.LSB << 8) + pressure_data.XLSB) >> (8 - oss);
			int32 pressure_b6            = temperature_b5 - 4000;
			int32 pressure_x1            = (calibrationData.B2 * ((pressure_b6 * pressure_b6) >> 12)) >> 11;
			int32 pressure_x2            = (calibrationData.AC2 * pressure_b6) >> 11;
			int32 pressure_x3            = pressure_x1 + pressure_x2;
			int32 pressure_b3            = (((calibrationData.AC1 * 4 + pressure_x3) << oss) + 2) / 4;
			pressure_x1                  = (calibrationData.AC3 * pressure_b6) >> 13;
			pressure_x2                  = (calibrationData.B1 * ((pressure_b6 * pressure_b6) >> 12)) >> 16;
			pressure_x3                  = ((pressure_x1 + pressure_x2) + 2) >> 2;
			uint32 pressure_b4           = (calibrationData.AC4 * static_cast<uint32>(pressure_x3 + 32768)) >> 15;
			uint32 pressure_b7           = static_cast<uint32>(pressure_uncompensated - pressure_b3) * (50000 >> oss);
			int32 pressure               = (pressure_b7 < 0x80000000) ? ((pressure_b7 << 1) / pressure_b4) : ((pressure_b7 / pressure_b4) << 1);
			pressure_x1                  = (pressure >> 8) * (pressure >> 8);
			pressure_x1                  = (pressure_x1 * 3038) >> 16;
			pressure_x2                  = (-7357 * pressure) >> 16;
			value                        = pressure + ((pressure_x1 + pressure_x2 + 3791) >> 4);
		}

		// @throw AL::Exception
		Void MeasureAndReadTemperature(int32& temperature, int32& temperature_b5)
		{
			{
				uint8 data[2] =
				{
					0xF4,
					0x2E
				};

				try
				{
					device.Write(
						data
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error triggering device"
					);
				}
			}

			Sleep(
				TimeSpan::FromMicroseconds(4500)
			);

			uncompensated_temperature_data temperature_data;

			try
			{
				device.WriteRead(
					uint8(0xF6),
					temperature_data
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading temperature data"
				);
			}

			int32 temperature_uncompensated = (temperature_data.MSB << 8) + temperature_data.LSB;
			int32 temperature_x1            = ((temperature_uncompensated - calibrationData.AC6) * calibrationData.AC5) >> 15;
			int32 temperature_x2            = (calibrationData.MC << 11) / (temperature_x1 + calibrationData.MD);
			temperature_b5                  = temperature_x1 + temperature_x2;
			temperature                     = (temperature_b5 + 8) >> 4;
		}

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
