#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include <hardware/gpio.h>

namespace AL::Hardware::Drivers::RP2040
{
	typedef uint8 GPIOPin;

	enum class GPIOPinEdges : uint8
	{
		Both,
		Rising,
		Falling
	};

	enum class GPIOPinValues : uint8
	{
		Low,
		High
	};

	enum class GPIOPinDirections : uint8
	{
		Input,
		Output
	};

	class GPIO
	{
		Bool                  isOpen = False;

		GPIOPin               pin;
		mutable GPIOPinValues value;
		GPIOPinDirections     direction;

		GPIO(const GPIO&) = delete;

	public:
		typedef GPIOPin           Pin;
		typedef GPIOPinEdges      PinEdges;
		typedef GPIOPinValues     PinValues;
		typedef GPIOPinDirections PinDirections;

		GPIO(GPIO&& gpio)
			: isOpen(
				gpio.isOpen
			),
			pin(
				gpio.pin
			),
			value(
				gpio.value
			),
			direction(
				gpio.direction
			)
		{
			gpio.isOpen = False;
		}

		GPIO(Pin pin, PinDirections direction = PinDirections::Input, PinValues value = PinValues::Low)
			: pin(
				pin
			),
			value(
				value
			),
			direction(
				direction
			)
		{
		}

		virtual ~GPIO()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsPulledUp() const
		{
			if (!::gpio_is_pulled_up(pin))
			{

				return False;
			}

			return True;
		}

		Bool IsPulledDown() const
		{
			if (!::gpio_is_pulled_down(pin))
			{

				return False;
			}

			return True;
		}

		auto Get() const
		{
			return value;
		}

		auto GetPin() const
		{
			return pin;
		}

		auto GetDirection() const
		{
			return direction;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"GPIO already open"
			);

			::gpio_init(
				GetPin()
			);

			switch (GetDirection())
			{
				case PinDirections::Input:
					::gpio_set_dir(GetPin(), false);
					break;

				case PinDirections::Output:
					::gpio_set_dir(GetPin(), true);
					::gpio_put(GetPin(), Get() == PinValues::High);
					break;
			}

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::gpio_deinit(
					GetPin()
				);

				isOpen = False;
			}
		}

		PinValues Read() const
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			return value = ::gpio_get(GetPin()) ? PinValues::High : PinValues::Low;
		}
		Void      Read(PinValues& value) const
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			this->value = value = ::gpio_get(GetPin()) ? PinValues::High : PinValues::Low;
		}

		Void Write(PinValues value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			::gpio_put(
				GetPin(),
				value == PinValues::High
			);

			this->value = value;
		}

		// @return AL::False on timeout
		Bool WaitForEdge(PinEdges value, TimeSpan maxWaitTime = TimeSpan::Infinite) const
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			OS::Timer timer;
			PinValues _value, __value = Read();

			switch (value)
			{
				case PinEdges::Both:
				{
					while ((_value = Read()) == __value)
					{
						if (timer.GetElapsed() >= maxWaitTime)
						{

							return False;
						}
					}
				}
				break;

				case PinEdges::Rising:
				{
					if (__value == PinValues::High)
					{
						while ((_value = Read()) == PinValues::High)
						{
							if (timer.GetElapsed() >= maxWaitTime)
							{

								return False;
							}
						}
					}

					while ((_value = Read()) == PinValues::Low)
					{
						if (timer.GetElapsed() >= maxWaitTime)
						{

							return False;
						}
					}
				}
				break;

				case PinEdges::Falling:
				{
					if (__value == PinValues::Low)
					{
						while ((_value = Read()) == PinValues::Low)
						{
							if (timer.GetElapsed() >= maxWaitTime)
							{

								return False;
							}
						}
					}

					while ((_value = Read()) == PinValues::High)
					{
						if (timer.GetElapsed() >= maxWaitTime)
						{

							return False;
						}
					}
				}
				break;
			}

			return True;
		}

		Void SetPull(Bool up, Bool down)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			::gpio_set_pulls(
				GetPin(),
				up,
				down
			);
		}

		Void SetPullUp(Bool set = True)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			::gpio_pull_up(
				GetPin()
			);
		}

		Void SetPullDown(Bool set = True)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			::gpio_pull_down(
				GetPin()
			);
		}

		Void SetDirection(PinDirections value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			::gpio_set_dir(
				GetPin(),
				value == PinDirections::Output
			);

			if ((direction = value) == PinDirections::Output)
			{
				::gpio_put(
					GetPin(),
					Get() == PinValues::High
				);
			}
		}

		GPIO& operator = (GPIO&& gpio)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = gpio.isOpen;
			gpio.isOpen = False;

			pin       = gpio.pin;
			value     = gpio.value;
			direction = gpio.direction;

			return *this;
		}

		Bool operator == (const GPIO& gpio) const
		{
			if (GetPin() != gpio.GetPin())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const GPIO& gpio) const
		{
			if (operator==(gpio))
			{

				return False;
			}

			return True;
		}
	};
}
