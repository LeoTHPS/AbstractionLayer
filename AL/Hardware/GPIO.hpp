#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Timer.hpp"
#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "Pico/GPIO.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#if AL_HAS_INCLUDE(<wiringPi.h>)
		#define AL_DEPENDENCY_WIRINGPI

		#include <wiringPi.h>
	#elif AL_HAS_INCLUDE(<gpiod.h>)
		#define AL_DEPENDENCY_GPIOD

		#include <gpiod.h>
	#else
		#warning: AL::Hardware::GPIO will be using sysfs - wiringPi/gpiod are recommended

		#include <fcntl.h>
		#include <unistd.h>

		#include <sys/stat.h>
		#include <sys/ioctl.h>
	#endif
#else
	#error Platform not supported
#endif

namespace AL::Hardware
{
	typedef uint8 GPIOBus;

	// BCM format
	typedef uint8 GPIOPin;

	typedef Bool GPIOPinValue;

	enum class GPIOPinValues : GPIOPinValue
	{
		Low  = False,
		High = True
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
		Bool                  isOpen = False;

#if defined(AL_PLATFORM_PICO)
		Pico::GPIO            gpio;
#elif defined(AL_PLATFORM_LINUX)
		GPIOBus               bus;
		GPIOPin               pin;
		mutable GPIOPinValues value;
		GPIOPinDirections     direction;

	#if defined(AL_DEPENDENCY_GPIOD)
		::gpiod_chip*         lpChip;
		::gpiod_line*         lpLine;
	#elif defined(AL_DEPENDENCY_WIRINGPI)

	#else

	#endif
#endif

		GPIO(const GPIO&) = delete;

	public:
		GPIO(GPIO&& gpio)
			: isOpen(
				gpio.isOpen
			)
#if defined(AL_PLATFORM_PICO)
			,
			gpio(
				Move(gpio.gpio)
			)
#elif defined(AL_PLATFORM_LINUX)
			,
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

#if defined(AL_PLATFORM_PICO)
		GPIO(GPIOPin pin, GPIOPinDirections direction = GPIOPinDirections::In, GPIOPinValues value = GPIOPinValues::Low)
			: gpio(
				pin,
				(direction == GPIOPinDirections::In) ? Pico::GPIOPinDirections::Input : Pico::GPIOPinDirections::Output,
				(value == GPIOPinValues::Low) ? Pico::GPIOPinValues::Low : Pico::GPIOPinValues::High
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		GPIO(GPIOBus bus, GPIOPin pin, GPIOPinDirections direction = GPIOPinDirections::In, GPIOPinValues value = GPIOPinValues::Low)
			: bus(
				bus
			),
			pin(
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
#endif

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

#if defined(AL_PLATFORM_PICO)
		auto Get() const
		{
			return (gpio.Get() == Pico::GPIOPinValues::Low) ? GPIOPinValues::Low : GPIOPinValues::High;
		}

		auto GetPin() const
		{
			return static_cast<GPIOPin>(gpio.GetPin());
		}

		auto GetDirection() const
		{
			return (gpio.GetDirection() == Pico::GPIOPinDirections::Input) ? GPIOPinDirections::In : GPIOPinDirections::Out;
		}
#elif defined(AL_PLATFORM_LINUX)
		auto Get() const
		{
			return value;
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
#endif

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"GPIO already open"
			);

#if defined(AL_PLATFORM_PICO)
			gpio.Open();
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
				gpio.Close();
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
		Void Read(GPIOPinValue& value) const
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			Pico::GPIOPinValues _value;

			gpio.Read(
				_value
			);

			value = _value == Pico::GPIOPinValues::High;
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

			this->value = value ? GPIOPinValues::High : GPIOPinValues::Low;
#else
			throw NotImplementedException();
#endif
		}
		// @throw AL::Exception
		Void Read(GPIOPinValues& value) const
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
			gpio.Write(
				value ? Pico::GPIOPinValues::High : Pico::GPIOPinValues::Low
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

			this->value = value ? GPIOPinValues::High : GPIOPinValues::Low;
#else
			throw NotImplementedException();
#endif
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
		Void SetPullUp()
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			gpio.SetPullUp();
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void SetPullDown()
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			gpio.SetPullDown();
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
#else
			throw NotImplementedException();
#endif
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
					gpio.SetDirection(Pico::GPIOPinDirections::Input);
					break;

				case GPIOPinDirections::Out:
					gpio.SetDirection(Pico::GPIOPinDirections::Output);
					gpio.Write(value ? Pico::GPIOPinValues::High : Pico::GPIOPinValues::Low);
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

			if ((this->direction = direction) == GPIOPinDirections::Out)
			{

				this->value = value ? GPIOPinValues::High : GPIOPinValues::Low;
			}
#else
			throw NotImplementedException();
#endif
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
		// @return AL::False if time elapsed
		Bool WaitForEdge(GPIOPinEdges edge, TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"GPIO not open"
			);

#if defined(AL_PLATFORM_PICO)
			switch (edge)
			{
				case GPIOPinEdges::Both:
					return gpio.WaitForEdge(Pico::GPIOPinEdges::Both);

				case GPIOPinEdges::Rising:
					return gpio.WaitForEdge(Pico::GPIOPinEdges::Rising);

				case GPIOPinEdges::Falling:
					return gpio.WaitForEdge(Pico::GPIOPinEdges::Falling);

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

#if defined(AL_PLATFORM_PICO)
			this->gpio = Move(
				gpio.gpio
			);
#elif defined(AL_PLATFORM_LINUX)
			bus       = gpio.bus;
			pin       = gpio.pin;
			value     = gpio.value;
			direction = gpio.direction;

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

#if defined(AL_PLATFORM_PICO)
			if (this->gpio != gpio.gpio)
			{

				return False;
			}
#elif defined(AL_PLATFORM_LINUX)
			if (GetBus() != gpio.GetBus())
			{

				return False;
			}

			if (GetPin() != gpio.GetPin())
			{

				return False;
			}
#endif

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
			return (value == GPIOPinValues::High) ? True : False;
		}

		static constexpr GPIOPinValues PinValue_ToValues(GPIOPinValue value)
		{
			return (value == False) ? GPIOPinValues::Low : GPIOPinValues::High;
		}
	};
}
