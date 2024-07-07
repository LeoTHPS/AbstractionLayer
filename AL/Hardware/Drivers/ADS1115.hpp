#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/I2C.hpp"
#include "AL/Hardware/GPIO.hpp" // GPIOPinValues

#include "AL/OS/Console.hpp"

namespace AL::Hardware::Drivers
{
	enum class ADS1115Gains : uint8
	{
		// FS = +6.144V
		// This parameter expresses the full-scale range of the ADC scaling.
		// In no event should more than VDD + 0.3V be applied to this device.
		FS_2_3,
		// FS = +4.096V
		// This parameter expresses the full-scale range of the ADC scaling.
		// In no event should more than VDD + 0.3V be applied to this device.
		FS_1,
		// FS = +2.048V
		FS_2,
		// FS = +1.024V
		FS_4,
		// FS = +0.512V
		FS_8,
		// FS = +0.256V
		FS_16,

		// FS = +2.048V
		Default = FS_2
	};

	enum class ADS1115DataRates : uint8
	{
		SPS_8,
		SPS_16,
		SPS_32,
		SPS_64,
		SPS_128,
		SPS_250,
		SPS_475,
		SPS_860,

		Default = SPS_128
	};

	class ADS1115
		: public IDriver<uint8, int16, Void>
	{
		enum class _Registers : uint8
		{
			Config     = 0b00000001,
			Lo_Thresh  = 0b00000010,
			Hi_Thresh  = 0b00000011,
			Conversion = 0b00000000
		};

		enum _CONFIG_REGISTER : uint16
		{
			_CONFIG_REGISTER_OPERATIONAL_STATUS_MASK                        = 0b1 << 15,
			_CONFIG_REGISTER_OPERATIONAL_STATUS_READ_CONVERSION_COMPLETE    = 0b1 << 15,
			_CONFIG_REGISTER_OPERATIONAL_STATUS_READ_CONVERSION_IN_PROGRESS = 0b0 << 15,
			_CONFIG_REGISTER_OPERATIONAL_STATUS_WRITE_BEGIN_CONVERSION      = 0b1 << 15,

			_CONFIG_REGISTER_INPUT_MULTIPLEXER_MASK                         = 0b111 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN0_AIN_GND             = 0b100 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN1_AIN_GND             = 0b101 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN2_AIN_GND             = 0b110 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN3_AIN_GND             = 0b111 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN0_AIN_AIN1            = 0b000 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN0_AIN_AIN3            = 0b001 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN1_AIN_AIN3            = 0b010 << 12,
			_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN2_AIN_AIN3            = 0b011 << 12,

			_CONFIG_REGISTER_PGA_CONFIG_MASK                                = 0b111 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_6_144                            = 0b000 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_4_096                            = 0b001 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_2_048                            = 0b010 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_1_024                            = 0b011 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_0_512                            = 0b100 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_0_256                            = 0b101 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_0_256_2                          = 0b110 << 9,
			_CONFIG_REGISTER_PGA_CONFIG_FS_0_256_3                          = 0b111 << 9,

			_CONFIG_REGISTER_OPERATING_MODE_MASK                            = 0b1 << 8,
			_CONFIG_REGISTER_OPERATING_MODE_CONTINUOUS_CONVERSION_MODE      = 0b0 << 8,
			_CONFIG_REGISTER_OPERATING_MODE_POWER_DOWN_SINGLE_SHOT_MODE     = 0b1 << 8,

			_CONFIG_REGISTER_DATA_RATE_MASK                                 = 0b111 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_8                                = 0b000 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_16                               = 0b001 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_32                               = 0b010 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_64                               = 0b011 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_128                              = 0b100 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_250                              = 0b101 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_475                              = 0b110 << 5,
			_CONFIG_REGISTER_DATA_RATE_SPS_860                              = 0b111 << 5,

			_CONFIG_REGISTER_COMPARATOR_MODE_MASK                           = 0b1 << 4,
			_CONFIG_REGISTER_COMPARATOR_MODE_TRADITIONAL                    = 0b0 << 4,
			_CONFIG_REGISTER_COMPARATOR_MODE_WINDOW_COMPARATOR              = 0b1 << 4,

			_CONFIG_REGISTER_COMPARATOR_POLARITY_MASK                       = 0b1 << 3,
			_CONFIG_REGISTER_COMPARATOR_POLARITY_ACTIVE_LOW                 = 0b0 << 3,
			_CONFIG_REGISTER_COMPARATOR_POLARITY_ACTIVE_HIGH                = 0b1 << 3,

			_CONFIG_REGISTER_COMPARATOR_LATCHING_MASK                       = 0b1 << 2,
			_CONFIG_REGISTER_COMPARATOR_LATCHING_COMPARATOR                 = 0b1 << 2,
			_CONFIG_REGISTER_COMPARATOR_LATCHING_NON_LATCHING_COMPARATOR    = 0b0 << 2,

			_CONFIG_REGISTER_COMPARATOR_QUEUE_MASK                          = 0b11,
			_CONFIG_REGISTER_COMPARATOR_QUEUE_DISABLE_COMPARATOR            = 0b11,
			_CONFIG_REGISTER_COMPARATOR_QUEUE_ASSERT_AFTER_ONE_CONVERSION   = 0b00,
			_CONFIG_REGISTER_COMPARATOR_QUEUE_ASSERT_AFTER_TWO_CONVERSIONS  = 0b01,
			_CONFIG_REGISTER_COMPARATOR_QUEUE_ASSERT_AFTER_FOUR_CONVERSIONS = 0b10
		};

		Bool             isOpen         = False;
		Bool             isBusAllocated = False;

		I2CBus*          lpBus;
		I2CDevice        device;

		ADS1115Gains     gain      = ADS1115Gains::Default;
		ADS1115DataRates dataRate  = ADS1115DataRates::Default;
		GPIOPinValues    alertMode = GPIOPinValues::Low;

	public:
		typedef ADS1115Gains           Gains;
		typedef ADS1115DataRates       DataRates;

		static constexpr ReadData      DATA_MAX              = 0xFFFF;

		static constexpr uint32        DEFAULT_SPEED         = 400000;

		static constexpr Channel       CHANNEL_FIRST         = 0;
		static constexpr Channel       CHANNEL_LAST          = 3;
		static constexpr Channel       CHANNEL_COUNT         = 4;

		inline static const I2CAddress DEVICE_ADDRESS_VDD    = 0b1001001;
		inline static const I2CAddress DEVICE_ADDRESS_SDA    = 0b1001010;
		inline static const I2CAddress DEVICE_ADDRESS_SCL    = 0b1001011;
		inline static const I2CAddress DEVICE_ADDRESS_GROUND = 0b1001000;

		static constexpr Float CalculateVoltage(Float vdd, ReadData value)
		{
			return vdd * (static_cast<Float>(value) / DATA_MAX);
		}

		ADS1115(ADS1115&& ads1115)
			: isOpen(
				ads1115.isOpen
			),
			isBusAllocated(
				ads1115.isBusAllocated
			),
			lpBus(
				ads1115.lpBus
			),
			device(
				Move(ads1115.device)
			),
			gain(
				ads1115.gain
			),
			dataRate(
				ads1115.dataRate
			)
		{
			ads1115.isOpen = False;
			ads1115.isBusAllocated = False;
		}

		ADS1115(I2CBus& bus, I2CAddress address = DEVICE_ADDRESS_GROUND)
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
		}

#if defined(AL_PLATFORM_PICO)
		ADS1115(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS_GROUND)
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
		}
#elif defined(AL_PLATFORM_LINUX)
		ADS1115(FileSystem::Path&& path, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS_GROUND)
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
		}
		ADS1115(const FileSystem::Path& path, uint32 baud = DEFAULT_SPEED, I2CAddress address = DEVICE_ADDRESS_GROUND)
			: ADS1115(
				FileSystem::Path(path),
				baud,
				address
			)
		{
		}
#endif

		virtual ~ADS1115()
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

		auto GetGain() const
		{
			return gain;
		}

		auto GetDataRate() const
		{
			return dataRate;
		}

		auto GetAlertMode() const
		{
			return alertMode;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"ADS1115 already open"
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
		Void SetGain(Gains value)
		{
			gain = value;
		}

		// @throw AL::Exception
		Void SetDataRate(DataRates value)
		{
			dataRate = value;
		}

		// @throw AL::Exception
		Void SetAlertMode(GPIOPinValues value)
		{
			alertMode = value;
		}

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"ADS1115 not open"
			);

			try
			{
				Device_BeginConversion(
					channel
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error beginning conversion"
				);
			}

			try
			{
				while (Device_IsConversionInProgress())
				{
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error checking if conversion is still in progress"
				);
			}

			try
			{
				Device_ReadConversionResult(
					data
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading conversion result"
				);
			}
		}

		ADS1115& operator = (ADS1115&& ads1115)
		{
			Close();

			isOpen = ads1115.isOpen;
			ads1115.isOpen = False;

			device = Move(
				ads1115.device
			);

			gain     = ads1115.gain;
			dataRate = ads1115.dataRate;

			return *this;
		}

		Bool operator == (const ADS1115& ads1115) const
		{
			if (device != ads1115.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ADS1115& ads1115) const
		{
			if (operator==(ads1115))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		Void Device_ReadRegister16(_Registers reg, uint16& value)
		{
			try
			{
				device.WriteRead(
					reg,
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading I2CDevice"
				);
			}

			value = BitConverter::FromBigEndian(
				value
			);
		}

		// @throw AL::Exception
		Void Device_WriteRegister16(_Registers reg, uint16 value)
		{
			uint8 buffer[] =
			{
				static_cast<uint8>(reg),
				static_cast<uint8>(value >> 8),
				static_cast<uint8>(value & 0x00FF)
			};

			try
			{
				device.Write(
					buffer
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing I2CDevice"
				);
			}
		}

		// @throw AL::Exception
		Bool Device_IsConversionInProgress()
		{
			uint16 config;

			try
			{
				Device_ReadRegister16(
					_Registers::Config,
					config
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading config register"
				);
			}

			if ((config & _CONFIG_REGISTER_OPERATIONAL_STATUS_MASK) != _CONFIG_REGISTER_OPERATIONAL_STATUS_READ_CONVERSION_IN_PROGRESS)
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Void Device_BeginConversion(Channel channel)
		{
			static constexpr uint16 PGA_FS[] =
			{
				_CONFIG_REGISTER_PGA_CONFIG_FS_6_144,
				_CONFIG_REGISTER_PGA_CONFIG_FS_4_096,
				_CONFIG_REGISTER_PGA_CONFIG_FS_2_048,
				_CONFIG_REGISTER_PGA_CONFIG_FS_1_024,
				_CONFIG_REGISTER_PGA_CONFIG_FS_0_256
			};

			static constexpr uint16 DATA_RATES[] =
			{
				_CONFIG_REGISTER_DATA_RATE_SPS_8,
				_CONFIG_REGISTER_DATA_RATE_SPS_16,
				_CONFIG_REGISTER_DATA_RATE_SPS_32,
				_CONFIG_REGISTER_DATA_RATE_SPS_64,
				_CONFIG_REGISTER_DATA_RATE_SPS_128,
				_CONFIG_REGISTER_DATA_RATE_SPS_250,
				_CONFIG_REGISTER_DATA_RATE_SPS_475,
				_CONFIG_REGISTER_DATA_RATE_SPS_860
			};

			static constexpr uint16 INPUT_MUX_BY_CHANNEL[] =
			{
				_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN0_AIN_GND,
				_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN1_AIN_GND,
				_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN2_AIN_GND,
				_CONFIG_REGISTER_INPUT_MULTIPLEXER_AIN_AIN3_AIN_GND
			};

			uint16 config =
				  _CONFIG_REGISTER_OPERATIONAL_STATUS_WRITE_BEGIN_CONVERSION
				| INPUT_MUX_BY_CHANNEL[channel]
				| PGA_FS[static_cast<typename Get_Enum_Or_Integer_Base<Gains>::Type>(GetGain())]
				| _CONFIG_REGISTER_OPERATING_MODE_POWER_DOWN_SINGLE_SHOT_MODE
				| DATA_RATES[static_cast<typename Get_Enum_Or_Integer_Base<DataRates>::Type>(GetDataRate())]
				| _CONFIG_REGISTER_COMPARATOR_MODE_TRADITIONAL
				| ((GetAlertMode() == GPIOPinValues::Low) ? _CONFIG_REGISTER_COMPARATOR_POLARITY_ACTIVE_LOW : _CONFIG_REGISTER_COMPARATOR_POLARITY_ACTIVE_HIGH)
				| _CONFIG_REGISTER_COMPARATOR_LATCHING_NON_LATCHING_COMPARATOR
				| _CONFIG_REGISTER_COMPARATOR_QUEUE_ASSERT_AFTER_ONE_CONVERSION;

			try
			{
				Device_WriteRegister16(
					_Registers::Config,
					config
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing config register"
				);
			}

			try
			{
				Device_WriteRegister16(
					_Registers::Hi_Thresh,
					0x8000
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing hi_thresh register"
				);
			}

			try
			{
				Device_WriteRegister16(
					_Registers::Lo_Thresh,
					0x0000
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing lo_thresh register"
				);
			}
		}

		// @throw AL::Exception
		Void Device_ReadConversionResult(int16& value)
		{
			uint16 reg;

			try
			{
				Device_ReadRegister16(
					_Registers::Conversion,
					reg
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading conversion register"
				);
			}

			value = static_cast<int16>(
				reg
			);
		}
	};
}
