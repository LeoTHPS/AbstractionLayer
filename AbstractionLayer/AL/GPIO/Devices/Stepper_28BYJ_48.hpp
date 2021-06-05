#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/Pin.hpp"

namespace AL::GPIO::Devices
{
	enum class Stepper_28BYJ_48_Phases
	{
		Phase_0,
		Phase_1,
		Phase_2,
		Phase_3,
		Phase_4,
		Phase_5,
		Phase_6,
		Phase_7,
		Phase_8
	};

	class Stepper_28BYJ_48
		: public Device<void, Stepper_28BYJ_48_Phases, Stepper_28BYJ_48_Phases>
	{
		DeviceId  deviceId;
		PinNumber in1;
		PinNumber in2;
		PinNumber in3;
		PinNumber in4;

		struct
		{
			Pin IN1;
			Pin IN2;
			Pin IN3;
			Pin IN4;
		} pins;

		Stepper_28BYJ_48_Phases phase;

	public:
		Stepper_28BYJ_48(Stepper_28BYJ_48&& stepper)
			: deviceId(
				stepper.deviceId
			),
			in1(
				stepper.in1
			),
			in2(
				stepper.in2
			),
			in3(
				stepper.in3
			),
			in4(
				stepper.in4
			),
			phase(
				stepper.phase
			)
		{
			pins.IN1 = Move(
				stepper.pins.IN1
			);
			pins.IN2 = Move(
				stepper.pins.IN2
			);
			pins.IN3 = Move(
				stepper.pins.IN3
			);
			pins.IN4 = Move(
				stepper.pins.IN4
			);
		}

		Stepper_28BYJ_48(DeviceId deviceId, PinNumber in1, PinNumber in2, PinNumber in3, PinNumber in4)
			: deviceId(
				deviceId
			),
			in1(
				in1
			),
			in2(
				in2
			),
			in3(
				in3
			),
			in4(
				in4
			),
			phase(
				Stepper_28BYJ_48_Phases::Phase_0
			)
		{
		}

		auto GetPhase() const
		{
			return phase;
		}

		// @throw AL::Exceptions::Exception
		void SetPhase(Stepper_28BYJ_48_Phases value)
		{
			Write(
				value
			);
		}

		// @throw AL::Exceptions::Exception
		void Rotate(uint64 steps, TimeSpan delay, bool forward = true)
		{
			if (steps > 0)
			{
				auto lpSleepOrSpin = (delay.ToMicroseconds() > 1000) ? &Sleep : &Spin;

				auto lpStepper_28BYJ_48_Phases_NextOrPrevious =
					forward ? &Stepper_28BYJ_48_Phases_Next : &Stepper_28BYJ_48_Phases_Previous;

				auto phase = lpStepper_28BYJ_48_Phases_NextOrPrevious(
					GetPhase()
				);

				SetPhase(
					phase
				);

				for (uint64 i = 1; i < steps; ++i)
				{
					lpSleepOrSpin(
						delay
					);

					phase = lpStepper_28BYJ_48_Phases_NextOrPrevious(
						GetPhase()
					);

					SetPhase(
						phase
					);
				}
			}
		}

		auto& operator = (Stepper_28BYJ_48&& stepper)
		{
			Device::operator=(
				Move(stepper)
			);

			deviceId = stepper.deviceId;

			in1 = stepper.in1;
			in2 = stepper.in2;
			in3 = stepper.in3;
			in4 = stepper.in4;

			pins.IN1 = Move(
				stepper.pins.IN1
			);
			pins.IN2 = Move(
				stepper.pins.IN2
			);
			pins.IN3 = Move(
				stepper.pins.IN3
			);
			pins.IN4 = Move(
				stepper.pins.IN4
			);

			phase = stepper.phase;

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			PinValues pinValues[4];

			if (!Stepper_28BYJ_48_Phases_GetPinValues(pinValues, GetPhase()))
			{

				throw Exceptions::Exception(
					"Unsupported phase"
				);
			}

			if (!Pin::Export(pins.IN1, deviceId, in1, PinDirection::Out, pinValues[0]))
			{

				throw Exceptions::Exception(
					"Pin %u on device %u is already exported",
					in1,
					deviceId
				);
			}

			if (!Pin::Export(pins.IN2, deviceId, in2, PinDirection::Out, pinValues[1]))
			{
				pins.IN1.Unexport();

				throw Exceptions::Exception(
					"Pin %u on device %u is already exported",
					in2,
					deviceId
				);
			}

			if (!Pin::Export(pins.IN3, deviceId, in3, PinDirection::Out, pinValues[2]))
			{
				pins.IN2.Unexport();
				pins.IN1.Unexport();

				throw Exceptions::Exception(
					"Pin %u on device %u is already exported",
					in3,
					deviceId
				);
			}

			if (!Pin::Export(pins.IN4, deviceId, in4, PinDirection::Out, pinValues[3]))
			{
				pins.IN3.Unexport();
				pins.IN2.Unexport();
				pins.IN1.Unexport();

				throw Exceptions::Exception(
					"Pin %u on device %u is already exported",
					in4,
					deviceId
				);
			}
		}

		virtual void OnClose() override
		{
			pins.IN4.Unexport();
			pins.IN3.Unexport();
			pins.IN2.Unexport();
			pins.IN1.Unexport();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnRead(DataR& value) override
		{
			value = GetPhase();
		}

		// @throw AL::Exceptions::Exception
		virtual void OnWrite(const DataW& value) override
		{
			PinValues pinValues[4];

			if (!Stepper_28BYJ_48_Phases_GetPinValues(pinValues, value))
			{

				throw Exceptions::Exception(
					"Unsupported phase"
				);
			}

			pins.IN1.Write(pinValues[0]);
			pins.IN2.Write(pinValues[1]);
			pins.IN3.Write(pinValues[2]);
			pins.IN4.Write(pinValues[3]);

			phase = value;
		}

	private:
		static Stepper_28BYJ_48_Phases Stepper_28BYJ_48_Phases_Next(Stepper_28BYJ_48_Phases phase)
		{
			if (phase == Stepper_28BYJ_48_Phases::Phase_8)
			{

				return Stepper_28BYJ_48_Phases::Phase_1;
			}

			typedef typename Get_Enum_Or_Integer_Base<Stepper_28BYJ_48_Phases>::Type Stepper_28BYJ_48_Phases_Base_Type;

			return static_cast<Stepper_28BYJ_48_Phases>(
				static_cast<Stepper_28BYJ_48_Phases_Base_Type>(phase) + 1
			);
		}
		
		static Stepper_28BYJ_48_Phases Stepper_28BYJ_48_Phases_Previous(Stepper_28BYJ_48_Phases phase)
		{
			switch (phase)
			{
				case Stepper_28BYJ_48_Phases::Phase_0:
				case Stepper_28BYJ_48_Phases::Phase_1:
					return Stepper_28BYJ_48_Phases::Phase_8;
			}

			typedef typename Get_Enum_Or_Integer_Base<Stepper_28BYJ_48_Phases>::Type Stepper_28BYJ_48_Phases_Base_Type;

			return static_cast<Stepper_28BYJ_48_Phases>(
				static_cast<Stepper_28BYJ_48_Phases_Base_Type>(phase) - 1
			);
		}

		static bool Stepper_28BYJ_48_Phases_GetPinValues(PinValues(&values)[4], Stepper_28BYJ_48_Phases phase)
		{
			switch (phase)
			{
				case Stepper_28BYJ_48_Phases::Phase_0:
					values[0] = PinValues::Low;
					values[1] = PinValues::Low;
					values[2] = PinValues::Low;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_1:
					values[0] = PinValues::High;
					values[1] = PinValues::Low;
					values[2] = PinValues::Low;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_2:
					values[0] = PinValues::High;
					values[1] = PinValues::High;
					values[2] = PinValues::Low;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_3:
					values[0] = PinValues::Low;
					values[1] = PinValues::High;
					values[2] = PinValues::Low;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_4:
					values[0] = PinValues::Low;
					values[1] = PinValues::High;
					values[2] = PinValues::High;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_5:
					values[0] = PinValues::Low;
					values[1] = PinValues::Low;
					values[2] = PinValues::High;
					values[3] = PinValues::Low;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_6:
					values[0] = PinValues::Low;
					values[1] = PinValues::Low;
					values[2] = PinValues::High;
					values[3] = PinValues::High;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_7:
					values[0] = PinValues::Low;
					values[1] = PinValues::Low;
					values[2] = PinValues::Low;
					values[3] = PinValues::High;
					return true;

				case Stepper_28BYJ_48_Phases::Phase_8:
					values[0] = PinValues::High;
					values[1] = PinValues::Low;
					values[2] = PinValues::Low;
					values[3] = PinValues::High;
					return true;
			}

			return false;
		}
	};
}
