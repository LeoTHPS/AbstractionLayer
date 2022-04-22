#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include <hardware/gpio.h>
#elif defined(AL_PLATFORM_LINUX)
	#if __has_include(<wiringPi.h>)
		#define AL_DEPENDENCY_WIRINGPI

		#include <wiringPi.h>
	#elif __has_include(<gpiod.h>)
		#define AL_DEPENDENCY_GPIOD

		#include <gpiod.h>
	#else
		#warning: AL::Hardware::GPIO will be using sysfs - wiringPi/gpiod are recommended

		#include <fcntl.h>
		#include <unistd.h>

		#include <sys/stat.h>
		#include <sys/ioctl.h>
	#endif
#endif

namespace AL::Hardware
{
	typedef uint8 GPIOBus;

	// BCM format
	typedef uint8 GPIOPin;

	typedef uint32 GPIOPinValue;

	enum class GPIOPinValues : GPIOPinValue
	{
		Low, High
	};

	enum class GPIOPinEdges
	{
		Both, Rising, Falling
	};

	enum class GPIOPinDirections
	{
		In, Out
	};

	class GPIO
	{
		Bool              isOpen = False;

		GPIOBus           bus;
		GPIOPin           pin;
		GPIOPinValue      value     = 0;
		GPIOPinDirections direction = GPIOPinDirections::Out;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
		::gpiod_chip*     lpChip;
		::gpiod_line*     lpLine;
	#elif defined(AL_DEPENDENCY_WIRINGPI)

	#else

	#endif
#endif

	public:
		GPIO(GPIO&& gpio)
			: isOpen(
				gpio.isOpen
			),
			bus(
				gpio.bus
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
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			,
			lpChip(
				gpio.lpChip
			),
			lpLine(
				gpio.lpLine
			)
	#elif defined(AL_DEPENDENCY_WIRINGPI)

	#else

	#endif
#endif
		{
			gpio.isOpen = False;
		}

		GPIO(GPIOBus bus, GPIOPin pin)
			: bus(
				bus
			),
			pin(
				pin
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

		auto GetBus() const
		{
			return bus;
		}

		auto GetPin() const
		{
			return pin;
		}

		auto GetDirection() const
		{
			return direction;
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"GPIO already open"
			);

#if defined(AL_PLATFORM_PICO)
			if (GetBus() != 0)
			{

				throw Exception(
					"Bus not found"
				);
			}

			::gpio_init(
				GetPin()
			);

			switch (GetDirection())
			{
				case GPIOPinDirections::In:
					SetDirection(GPIOPinDirections::In);
					break;

				case GPIOPinDirections::Out:
					SetDirection(GPIOPinDirections::Out, value);
					break;
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if ((lpChip = ::gpiod_chip_open_by_number(static_cast<unsigned int>(GetBus()))) == nullptr)
			{

				throw Exception(
					"GPIO bus #%u not found",
					GetBus()
				);
			}

			if ((lpLine = ::gpiod_chip_get_line(lpChip, static_cast<unsigned int>(GetPin()))) == nullptr)
			{
				::gpiod_chip_close(
					lpChip
				);

				throw Exception(
					"GPIO pin #%u not found on bus #%u",
					GetPin(),
					GetBus()
				);
			}

			switch (GetDirection())
			{
				case GPIOPinDirections::In:
					SetDirection(GPIOPinDirections::In, 0);
					break;

				case GPIOPinDirections::Out:
					SetDirection(GPIOPinDirections::Out, value);
					break;
			}
	#elif defined(AL_DEPENDENCY_WIRINGPI)
			::wiringPiSetupGpio();

			switch (GetDirection())
			{
				case GPIOPinDirections::In:
					SetDirection(GPIOPinDirections::In, 0);
					break;

				case GPIOPinDirections::Out:
					SetDirection(GPIOPinDirections::Out, value);
					break;
			}
	#else
			// TODO: implement
			throw NotImplementedException();
	#endif
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
				::gpio_set_dir(
					GetPin(),
					true
				);

				::gpio_put(
					GetPin(),
					false
				);
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
				::gpiod_line_release(
					lpLine
				);

				::gpiod_chip_close(
					lpChip
				);
	#elif defined(AL_DEPENDENCY_WIRINGPI)
				// TODO: implement
	#else
				// TODO: implement
	#endif
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void Read(GPIOPinValue& value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			value = ::gpio_get(GetPin()) ? 1 : 0;
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if ((value = ::gpiod_line_get_value(lpLine)) == GPIOPinValue(-1))
			{

				throw OS::SystemException(
					"gpiod_line_get_value"
				);
			}
	#elif defined(AL_DEPENDENCY_WIRINGPI)
			value = static_cast<GPIOPinValue>(
				::digitalRead(
					static_cast<int>(GetPin())
				)
			);
	#else
			// TODO: implement
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif

			this->value = value;
		}
		// @throw AL::Exception
		Void Read(GPIOPinValues& value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			GPIOPinValue _value;

			Read(
				_value
			);

			value = PinValue_ToValues(
				_value
			);
		}

		// @throw AL::Exception
		Void Write(GPIOPinValue value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			AL_ASSERT(
				GetDirection() == GPIOPinDirections::Out,
				"Invalid GPIO direction"
			);

#if defined(AL_PLATFORM_PICO)
			::gpio_put(
				GetPin(),
				value != 0
			);
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if (::gpiod_line_set_value(lpLine, static_cast<int>(value)) == -1)
			{

				throw OS::SystemException(
					"gpiod_line_set_value"
				);
			}
	#elif defined(AL_DEPENDENCY_WIRINGPI)
			::digitalWrite(
				static_cast<int>(GetPin()),
				static_cast<int>(value)
			);
	#else
			// TODO: implement
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif

			this->value = value;
		}
		// @throw AL::Exception
		Void Write(GPIOPinValues value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			auto _value = PinValues_ToValue(
				value
			);

			Write(
				_value
			);
		}

		// @throw AL::Exception
		Void SetDirection(GPIOPinDirections direction)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			SetDirection(
				direction,
				GPIOPinValues::Low
			);
		}
		// @throw AL::Exception
		Void SetDirection(GPIOPinDirections direction, GPIOPinValue value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			switch (direction)
			{
				case GPIOPinDirections::In:
					::gpio_set_dir(GetPin(), false);
					break;

				case GPIOPinDirections::Out:
					::gpio_set_dir(GetPin(), true);
					::gpio_put(GetPin(), value != 0);
					break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			// TODO: remove this when libgpiod2 is released
			{
				::gpiod_line_release(
					lpLine
				);

				if ((lpLine = ::gpiod_chip_get_line(lpChip, static_cast<unsigned int>(GetPin()))) == nullptr)
				{

					throw OS::SystemException(
						"gpiod_chip_get_line"
					);
				}
			}

			switch (direction)
			{
				case GPIOPinDirections::In:
					if (::gpiod_line_request_input(lpLine, nullptr) == -1)
					{

						throw OS::SystemException(
							"gpiod_line_request_input"
						);
					}
					break;

				case GPIOPinDirections::Out:
					if (::gpiod_line_request_output(lpLine, nullptr, static_cast<int>(value)) == -1)
					{

						throw OS::SystemException(
							"gpiod_line_request_output"
						);
					}
					break;

				default:
					throw NotImplementedException();
			}
	#elif defined(AL_DEPENDENCY_WIRINGPI)
			switch (direction)
			{
				case GPIOPinDirections::In:
					::pinMode(static_cast<int>(GetPin()), INPUT);
					break;

				case GPIOPinDirections::Out:
					::pinMode(static_cast<int>(GetPin()), OUTPUT);
					::digitalWrite(static_cast<int>(GetPin()), static_cast<int>(value));
					break;

				default:
					throw NotImplementedException();
			}
	#else
			// TODO: implement
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif

			if ((this->direction = direction) == GPIOPinDirections::Out)
			{

				this->value = value;
			}
		}
		// @throw AL::Exception
		Void SetDirection(GPIOPinDirections direction, GPIOPinValues value)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

			auto _value = PinValues_ToValue(
				value
			);

			SetDirection(
				direction,
				_value
			);
		}

		// @throw AL::Exception
		// @return False if time elapsed
		Bool WaitForEdge(GPIOPinEdges edge, TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			// TODO: remove this when libgpiod2 is released
			{
				::gpiod_line_release(
					lpLine
				);

				if ((lpLine = ::gpiod_chip_get_line(lpChip, static_cast<unsigned int>(GetPin()))) == nullptr)
				{

					throw OS::SystemException(
						"gpiod_chip_get_line"
					);
				}
			}

			int expectedEventType = 0;

			switch (edge)
			{
				case GPIOPinEdges::Both:
				{
					if (::gpiod_line_request_both_edges_events(lpLine, nullptr) == -1)
					{

						throw OS::SystemException(
							"gpiod_line_request_both_edges_events"
						);
					}
				}
				break;

				case GPIOPinEdges::Rising:
				{
					expectedEventType = GPIOD_LINE_EVENT_RISING_EDGE;

					if (::gpiod_line_request_rising_edge_events(lpLine, nullptr) == -1)
					{

						throw OS::SystemException(
							"gpiod_line_request_rising_edge_events"
						);
					}
				}
				break;

				case GPIOPinEdges::Falling:
				{
					expectedEventType = GPIOD_LINE_EVENT_FALLING_EDGE;

					if (::gpiod_line_request_falling_edge_events(lpLine, nullptr) == -1)
					{

						throw OS::SystemException(
							"gpiod_line_request_falling_edge_events"
						);
					}
				}
				break;

				default:
					throw NotImplementedException();
			}

			::timespec timeout;
			timeout.tv_sec = maxWaitTime.ToSeconds();
			timeout.tv_nsec = (maxWaitTime.ToMicroseconds() - TimeSpan::FromSeconds(timeout.tv_sec).ToMicroseconds()) * 1000;

			switch (::gpiod_line_event_wait(lpLine, &timeout))
			{
				case -1:
					throw OS::SystemException(
						"gpiod_line_event_wait"
					);

				case 0:
					return False;
			}

			::gpiod_line_event event;

			do
			{
				if (::gpiod_line_event_read(lpLine, &event) == -1)
				{

					throw OS::SystemException(
						"gpiod_line_event_read"
					);
				}
			} while (expectedEventType && (event.event_type != expectedEventType));
	#elif defined(AL_DEPENDENCY_WIRINGPI)
			// TODO: implement
			throw NotImplementedException();
	#else
			// TODO: implement
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif

			return True;
		}

		GPIO& operator = (GPIO&& gpio)
		{
			Close();

			isOpen = gpio.isOpen;
			gpio.isOpen = False;

			bus = gpio.bus;
			pin = gpio.pin;
			value = gpio.value;
			direction = gpio.direction;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			lpChip = gpio.lpChip;
			gpio.lpChip = nullptr;

			lpLine = gpio.lpLine;
			gpio.lpLine = nullptr;
	#elif defined(AL_DEPENDENCY_WIRINGPI)

	#else

	#endif
#endif

			return *this;
		}

		Bool operator == (const GPIO& gpio) const
		{
			if (IsOpen() != gpio.IsOpen())
			{

				return False;
			}

			if (GetBus() != gpio.GetBus())
			{

				return False;
			}

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

	private:
		static constexpr GPIOPinValue PinValues_ToValue(GPIOPinValues value)
		{
			return (value == GPIOPinValues::Low) ? 0 : 1;
		}

		static constexpr GPIOPinValues PinValue_ToValues(GPIOPinValue value)
		{
			return (value == 0) ? GPIOPinValues::Low : GPIOPinValues::High;
		}
	};
}
