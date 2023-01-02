#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/GPIO.hpp"

namespace AL::Hardware::Drivers
{
	struct CYT1062Frame
	{
		int8 Delta;
		Bool Button;
	};

	class CYT1062
		: public IDriver<Void, CYT1062Frame, Void>
	{
		Bool                  isOpen = False;

		GPIO                  dt;
		GPIO                  sw;
		GPIO                  clk;
		mutable GPIOPinValues clkValue = GPIOPinValues::Low;

	public:
		typedef CYT1062Frame Frame;

		CYT1062(CYT1062&& cyt1062)
			: isOpen(
				cyt1062.isOpen
			),
			dt(
				Move(cyt1062.dt)
			),
			sw(
				Move(cyt1062.sw)
			),
			clk(
				Move(cyt1062.clk)
			),
			clkValue(
				cyt1062.clkValue
			)
		{
			cyt1062.isOpen   = False;
			cyt1062.clkValue = GPIOPinValues::Low;
		}

		CYT1062(GPIOBus clkBus, GPIOPin clk, GPIOBus dtBus, GPIOPin dt, GPIOBus swBus, GPIOPin sw)
			: dt(
				dtBus, dt
			),
			sw(
				swBus,
				sw
			),
			clk(
				clkBus,
				clk
			)
		{
		}

		virtual ~CYT1062()
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
				"CYT1062 already open"
			);

			{
				try
				{
					dt.Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening DT [Bus: %u, Pin: %u]",
						dt.GetBus(),
						dt.GetPin()
					);
				}

				try
				{
					dt.SetDirection(
						GPIOPinDirections::In
					);

					dt.SetPullUp();
				}
				catch (Exception& exception)
				{
					dt.Close();

					throw Exception(
						Move(exception),
						"Error configuring DT [Bus: %u, Pin: %u]",
						dt.GetBus(),
						dt.GetPin()
					);
				}
			}

			{
				try
				{
					sw.Open();
				}
				catch (Exception& exception)
				{
					dt.Close();

					throw Exception(
						Move(exception),
						"Error opening SW [Bus: %u, Pin: %u]",
						sw.GetBus(),
						sw.GetPin()
					);
				}

				try
				{
					sw.SetDirection(
						GPIOPinDirections::In
					);

					sw.SetPullUp();
				}
				catch (Exception& exception)
				{
					sw.Close();
					dt.Close();

					throw Exception(
						Move(exception),
						"Error configuring SW [Bus: %u, Pin: %u]",
						sw.GetBus(),
						sw.GetPin()
					);
				}
			}

			{
				try
				{
					clk.Open();
				}
				catch (Exception& exception)
				{
					sw.Close();
					dt.Close();

					throw Exception(
						Move(exception),
						"Error opening CLK [Bus: %u, Pin: %u]",
						clk.GetBus(),
						clk.GetPin()
					);
				}

				try
				{
					clk.SetDirection(
						GPIOPinDirections::In
					);

					clk.SetPullUp();
				}
				catch (Exception& exception)
				{
					clk.Close();
					sw.Close();
					dt.Close();

					throw Exception(
						Move(exception),
						"Error configuring CLK [Bus: %u, Pin: %u]",
						clk.GetBus(),
						clk.GetPin()
					);
				}
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				dt.Close();
				sw.Close();
				clk.Close();

				clkValue = GPIOPinValues::Low;

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"CYT1062 not open"
			);

			data.Delta = 0;

			GPIOPinValues sw, dt, clk;

			if (ReadCLK(clk))
			{
				ReadDT(
					dt
				);

				data.Delta = (dt == clk) ? 1 : -1;
			}

			ReadSW(
				sw
			);

			data.Button = sw == GPIOPinValues::Low;
		}

		CYT1062& operator = (CYT1062&& cyt1062)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = cyt1062.isOpen;
			cyt1062.isOpen = False;

			dt  = Move(cyt1062.dt);
			sw  = Move(cyt1062.sw);
			clk = Move(cyt1062.clk);

			clkValue = cyt1062.clkValue;
			cyt1062.clkValue = GPIOPinValues::Low;

			return *this;
		}

		Bool operator == (const CYT1062& cyt1062) const
		{
			if (dt != cyt1062.dt)
			{

				return False;
			}

			if (sw != cyt1062.sw)
			{

				return False;
			}

			if (clk != cyt1062.clk)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const CYT1062& cyt1062) const
		{
			if (operator==(cyt1062))
			{

				return False;
			}

			return True;
		}

	private:
		Void ReadSW(GPIOPinValues& value) const
		{
			try
			{
				sw.Read(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading SW [Bus: %u, Pin: %u]",
					sw.GetBus(),
					sw.GetPin()
				);
			}
		}
		Void ReadDT(GPIOPinValues& value) const
		{
			try
			{
				dt.Read(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading DT [Bus: %u, Pin: %u]",
					dt.GetBus(),
					dt.GetPin()
				);
			}
		}
		Bool ReadCLK(GPIOPinValues& value) const
		{
			auto clkValue = this->clkValue;

			ReadCLKOnce(
				this->clkValue
			);

			if ((this->clkValue == clkValue) || (this->clkValue == GPIOPinValues::Low))
			{

				return False;
			}

			value = this->clkValue;

			return True;
		}
		Void ReadCLKOnce(GPIOPinValues& value) const
		{
			try
			{
				clk.Read(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading CLK [Bus: %u, Pin: %u]",
					clk.GetBus(),
					clk.GetPin()
				);
			}
		}
	};
}
