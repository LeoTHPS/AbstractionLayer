#pragma once
#include "AL/Common.hpp"

#include "IDevice.hpp"

#include "AL/Hardware/GPIO.hpp"

namespace AL::Hardware::Drivers
{
	enum class Stepper_28BYJ_48_Phases
	{
		Phase_0 = 0b0000,
		Phase_1 = 0b1000,
		Phase_2 = 0b1100,
		Phase_3 = 0b0100,
		Phase_4 = 0b0110,
		Phase_5 = 0b0010,
		Phase_6 = 0b0011,
		Phase_7 = 0b0001,
		Phase_8 = 0b1001
	};

	class Stepper_28BYJ_48
		: public IDevice<Void, Stepper_28BYJ_48_Phases, Stepper_28BYJ_48_Phases>
	{
		Bool isOpen = False;

		GPIO in1;
		GPIO in2;
		GPIO in3;
		GPIO in4;

		Stepper_28BYJ_48_Phases phase;

	public:
		Stepper_28BYJ_48(Stepper_28BYJ_48&& stepper)
			: isOpen(
				stepper.isOpen
			),
			in1(
				Move(stepper.in1)
			),
			in2(
				Move(stepper.in2)
			),
			in3(
				Move(stepper.in3)
			),
			in4(
				Move(stepper.in4)
			),
			phase(
				stepper.phase
			)
		{
			stepper.isOpen = False;
		}

		Stepper_28BYJ_48(GPIOBus bus, GPIOPin in1, GPIOPin in2, GPIOPin in3, GPIOPin in4)
			: in1(
				bus,
				in1
			),
			in2(
				bus,
				in1
			),
			in3(
				bus,
				in1
			),
			in4(
				bus,
				in1
			)
		{
		}

		virtual ~Stepper_28BYJ_48()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
		}

		auto GetPhase() const
		{
			return phase;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"Stepper_28BYJ_48 already open"
			);

			try
			{
				in1.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening IN1"
				);
			}

			try
			{
				in2.Open();
			}
			catch (Exception& exception)
			{
				in1.Close();

				throw Exception(
					Move(exception),
					"Error opening IN2"
				);
			}

			try
			{
				in3.Open();
			}
			catch (Exception& exception)
			{
				in2.Close();
				in1.Close();

				throw Exception(
					Move(exception),
					"Error opening IN3"
				);
			}

			try
			{
				in4.Open();
			}
			catch (Exception& exception)
			{
				in3.Close();
				in2.Close();
				in1.Close();

				throw Exception(
					Move(exception),
					"Error opening IN4"
				);
			}

			try
			{
				Write(
					GetPhase()
				);
			}
			catch (Exception& exception)
			{
				in4.Close();
				in3.Close();
				in2.Close();
				in1.Close();

				throw Exception(
					Move(exception),
					"Error initializing phase"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				in1.Close();
				in2.Close();
				in3.Close();
				in4.Close();

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void SetPhase(Stepper_28BYJ_48_Phases value)
		{
			AL_ASSERT(
				IsOpen(),
				"Stepper_28BYJ_48 not open"
			);

			Write(
				value
			);
		}

		// @throw AL::Exception
		virtual Void Rotate(uint64 steps, TimeSpan delay, Bool forward = True)
		{
			AL_ASSERT(
				IsOpen(),
				"Stepper_28BYJ_48 not open"
			);

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

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"Stepper_28BYJ_48 not open"
			);
			
			GPIOPinValue pinValues[4];

			try
			{
				in1.Read(
					pinValues[0]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading IN1"
				);
			}

			try
			{
				in2.Read(
					pinValues[1]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading IN2"
				);
			}

			try
			{
				in3.Read(
					pinValues[3]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading IN3"
				);
			}

			try
			{
				in4.Read(
					pinValues[3]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading IN4"
				);
			}

			Stepper_28BYJ_48_Phases_Decode(
				phase,
				pinValues
			);

			data = GetPhase();;
		}

		// @throw AL::Exception
		virtual Void Write(const WriteData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"Stepper_28BYJ_48 not open"
			);

			GPIOPinValue pinValues[4];

			Stepper_28BYJ_48_Phases_GetPinValues(
				pinValues,
				data
			);

			try
			{
				in1.Write(
					pinValues[0]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing IN1"
				);
			}

			try
			{
				in2.Write(
					pinValues[1]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing IN2"
				);
			}

			try
			{
				in3.Write(
					pinValues[2]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing IN3"
				);
			}

			try
			{
				in4.Write(
					pinValues[3]
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing IN4"
				);
			}

			phase = data;
		}

		Stepper_28BYJ_48& operator = (Stepper_28BYJ_48&& stepper)
		{
			Close();

			isOpen = stepper.isOpen;
			stepper.isOpen = False;

			in1 = Move(
				stepper.in1
			);

			in2 = Move(
				stepper.in2
			);

			in3 = Move(
				stepper.in3
			);

			in4 = Move(
				stepper.in4
			);

			phase = stepper.phase;

			return *this;
		}

		Bool operator == (const Stepper_28BYJ_48& stepper) const
		{
			if (in1 != stepper.in1)
			{

				return False;
			}

			if (in2 != stepper.in2)
			{

				return False;
			}

			if (in3 != stepper.in3)
			{

				return False;
			}

			if (in4 != stepper.in4)
			{

				return False;
			}

			if (phase != stepper.phase)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Stepper_28BYJ_48& stepper) const
		{
			if (operator==(stepper))
			{

				return False;
			}

			return True;
		}

	private:
		static Stepper_28BYJ_48_Phases Stepper_28BYJ_48_Phases_Next(Stepper_28BYJ_48_Phases phase)
		{
			switch (phase)
			{
				case Stepper_28BYJ_48_Phases::Phase_0:
					return Stepper_28BYJ_48_Phases::Phase_1;

				case Stepper_28BYJ_48_Phases::Phase_1:
					return Stepper_28BYJ_48_Phases::Phase_2;

				case Stepper_28BYJ_48_Phases::Phase_2:
					return Stepper_28BYJ_48_Phases::Phase_3;

				case Stepper_28BYJ_48_Phases::Phase_3:
					return Stepper_28BYJ_48_Phases::Phase_4;

				case Stepper_28BYJ_48_Phases::Phase_4:
					return Stepper_28BYJ_48_Phases::Phase_5;

				case Stepper_28BYJ_48_Phases::Phase_5:
					return Stepper_28BYJ_48_Phases::Phase_6;

				case Stepper_28BYJ_48_Phases::Phase_6:
					return Stepper_28BYJ_48_Phases::Phase_7;

				case Stepper_28BYJ_48_Phases::Phase_7:
					return Stepper_28BYJ_48_Phases::Phase_8;

				case Stepper_28BYJ_48_Phases::Phase_8:
					return Stepper_28BYJ_48_Phases::Phase_1;
			}

			return Stepper_28BYJ_48_Phases::Phase_1;
		}

		static Stepper_28BYJ_48_Phases Stepper_28BYJ_48_Phases_Previous(Stepper_28BYJ_48_Phases phase)
		{
			switch (phase)
			{
				case Stepper_28BYJ_48_Phases::Phase_0:
				case Stepper_28BYJ_48_Phases::Phase_1:
					return Stepper_28BYJ_48_Phases::Phase_8;

				case Stepper_28BYJ_48_Phases::Phase_2:
					return Stepper_28BYJ_48_Phases::Phase_1;

				case Stepper_28BYJ_48_Phases::Phase_3:
					return Stepper_28BYJ_48_Phases::Phase_2;

				case Stepper_28BYJ_48_Phases::Phase_4:
					return Stepper_28BYJ_48_Phases::Phase_3;

				case Stepper_28BYJ_48_Phases::Phase_5:
					return Stepper_28BYJ_48_Phases::Phase_4;

				case Stepper_28BYJ_48_Phases::Phase_6:
					return Stepper_28BYJ_48_Phases::Phase_5;

				case Stepper_28BYJ_48_Phases::Phase_7:
					return Stepper_28BYJ_48_Phases::Phase_6;

				case Stepper_28BYJ_48_Phases::Phase_8:
					return Stepper_28BYJ_48_Phases::Phase_7;
			}

			return Stepper_28BYJ_48_Phases::Phase_8;
		}

		static constexpr Void Stepper_28BYJ_48_Phases_Decode(Stepper_28BYJ_48_Phases& phase, const GPIOPinValue(&values)[4])
		{
			typedef typename Get_Enum_Or_Integer_Base<Stepper_28BYJ_48_Phases>::Type Stepper_28BYJ_48_Phases_Base;

			reinterpret_cast<Stepper_28BYJ_48_Phases_Base&>(phase) |= values[0];
			reinterpret_cast<Stepper_28BYJ_48_Phases_Base&>(phase) |= values[1] << 1;
			reinterpret_cast<Stepper_28BYJ_48_Phases_Base&>(phase) |= values[1] << 2;
			reinterpret_cast<Stepper_28BYJ_48_Phases_Base&>(phase) |= values[1] << 3;
		}

		static constexpr Void Stepper_28BYJ_48_Phases_GetPinValues(GPIOPinValue(&values)[4], Stepper_28BYJ_48_Phases phase)
		{
			typedef typename Get_Enum_Or_Integer_Base<Stepper_28BYJ_48_Phases>::Type Stepper_28BYJ_48_Phases_Base;

			values[0] = static_cast<Stepper_28BYJ_48_Phases_Base>(phase) & 1;
			values[1] = (static_cast<Stepper_28BYJ_48_Phases_Base>(phase) >> 1) & 1;
			values[2] = (static_cast<Stepper_28BYJ_48_Phases_Base>(phase) >> 2) & 1;
			values[3] = (static_cast<Stepper_28BYJ_48_Phases_Base>(phase) >> 3) & 1;
		}
	};
}
