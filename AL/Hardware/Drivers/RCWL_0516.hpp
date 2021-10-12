#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/GPIO.hpp"

namespace AL::Hardware::Drivers
{
	class RCWL_0516
		: public IDriver<Void, Bool, Void>
	{
		Bool isOpen = False;

		GPIO pin;

	public:
		RCWL_0516(RCWL_0516&& rcwl)
			: isOpen(
				rcwl.isOpen
			),
			pin(
				Move(rcwl.pin)
			)
		{
			rcwl.isOpen = False;
		}

		RCWL_0516(GPIOBus bus, GPIOPin pin)
			: pin(
				bus,
				pin
			)
		{
		}

		virtual ~RCWL_0516()
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

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"RCWL_0516 already open"
			);

			try
			{
				pin.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening pin"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				pin.Close();

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"RCWL_0516 not open"
			);

			GPIOPinValues value;

			try
			{
				pin.Read(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading pin"
				);
			}

			data = value == GPIOPinValues::High;
		}

		RCWL_0516& operator = (RCWL_0516&& rcwl)
		{
			Close();

			isOpen = rcwl.isOpen;
			rcwl.isOpen = False;

			pin = Move(
				rcwl.pin
			);

			return *this;
		}

		Bool operator == (const RCWL_0516& rcwl) const
		{
			if (pin != rcwl.pin)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const RCWL_0516& rcwl) const
		{
			if (operator==(rcwl))
			{

				return False;
			}

			return True;
		}
	};
}
