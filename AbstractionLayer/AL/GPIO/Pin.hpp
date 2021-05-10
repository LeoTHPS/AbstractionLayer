#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <sys/stat.h>

	#if __has_include(<gpiod.h>)
		#include <gpiod.h>

		#define AL_DEPENDENCY_GPIOD
	#endif
#endif

namespace AL::GPIO
{
	typedef uint8 DeviceId;

	typedef uint32 PinValue;

	// BCM format
	typedef uint8 PinNumber;

	enum class PinEdges
	{
		Rising,
		Falling,
		Both
	};

	enum class PinValues : PinValue
	{
		Low  = 0,
		High = 1
	};

	enum class PinDirection
	{
		In,
		Out
	};

	class Pin
	{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
		gpiod_chip* lpChip;
		gpiod_line* lpLine;
	#endif
#endif

		PinNumber number;
		DeviceId deviceId;

		Pin(const Pin&) = delete;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
		Pin(gpiod_chip* lpChip, gpiod_line* lpLine, DeviceId deviceId, PinNumber number)
			: lpChip(
				lpChip
			),
			lpLine(
				lpLine
			),
			number(
				number
			),
			deviceId(
				deviceId
			)
		{
		}
	#endif
#endif

	public:
		// @throw AL::Exceptions::Exception
		// @return false if already exported
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number)
		{
#if defined(AL_PLATFORM_LINUX)
			{
				struct stat s;

				if (!stat(String::Format("/sys/class/gpio/gpio%u", number).GetCString(), &s) && S_ISDIR(s.st_mode))
				{

					return false;
				}
			}

	#if defined(AL_DEPENDENCY_GPIOD)
			if (auto lpChip = gpiod_chip_open_by_number(static_cast<unsigned int>(deviceId)))
			{
				if (auto lpLine = gpiod_chip_get_line(lpChip, static_cast<unsigned int>(number)))
				{
					pin = Pin(
						lpChip,
						lpLine,
						deviceId,
						number
					);

					return true;
				}

				gpiod_chip_close(
					lpChip
				);
			}

			throw Exceptions::Exception(
				"GPIO #%u on device #%u not found",
				number,
				deviceId
			);
	#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		// @return false if already exported
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number, PinDirection direction, PinValue value)
		{
			if (!Export(pin, deviceId, number))
			{

				return false;
			}

			try
			{
				pin.SetDirection(
					direction,
					value
				);
			}
			catch (Exceptions::Exception&)
			{
				pin.Unexport();

				throw;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if already exported
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number, PinDirection direction, PinValues value)
		{
			if (!Export(pin, deviceId, number))
			{

				return false;
			}

			try
			{
				pin.SetDirection(
					direction,
					value
				);
			}
			catch (Exceptions::Exception&)
			{
				pin.Unexport();

				throw;
			}

			return true;
		}

		Pin()
			:
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			lpChip(
				nullptr
			),
			lpLine(
				nullptr
			),
	#endif
#endif
			number(
				0
			),
			deviceId(
				0
			)
		{
		}

		Pin(Pin&& pin)
			:
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			lpChip(
				pin.lpChip
			),
			lpLine(
				pin.lpLine
			),
	#endif
#endif
			number(
				pin.number
			),
			deviceId(
				pin.deviceId
			)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			pin.lpChip = nullptr;
			pin.lpLine = nullptr;
	#endif
#endif

			pin.number = 0;
			pin.deviceId = 0;
		}

		virtual ~Pin()
		{
			Unexport();
		}

		bool IsExported() const
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			return lpLine != nullptr;
	#endif
#endif

			return false;
		}

		PinNumber GetNumber() const
		{
			return number;
		}

		DeviceId GetDeviceId() const
		{
			return deviceId;
		}

		PinDirection GetDirection() const
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_DEPENDENCY_GPIOD)
			return static_cast<PinDirection>(
				gpiod_line_direction(
					lpLine
				)
			);
#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		void Read(PinValue& value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if ((value = gpiod_line_get_value(lpLine)) == PinValue(-1))
			{

				throw Exceptions::SystemException(
					"gpiod_line_get_value"
				);
			}
	#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void Read(PinValues& value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

			Read(
				reinterpret_cast<PinValue&>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void Write(PinValue value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");
			
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if (gpiod_line_set_value(lpLine, static_cast<int>(value)) == -1)
			{

				throw Exceptions::SystemException(
					"gpiod_line_set_value"
				);
			}
	#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void Write(PinValues value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

			Write(
				static_cast<PinValue>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

			SetDirection(
				direction,
				PinValues::Low
			);
		}
		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValue value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");
			
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if (lpLine != nullptr)
			{
				gpiod_line_release(
					lpLine
				);

				lpLine = gpiod_chip_get_line(
					lpChip,
					static_cast<unsigned int>(GetNumber())
				);
			}
			else if (!(lpLine = gpiod_chip_get_line(lpChip, static_cast<unsigned int>(GetNumber()))))
			{

				throw Exceptions::SystemException(
					"gpiod_chip_get_line"
				);
			}

			switch (direction)
			{
				case PinDirection::In:
					if (gpiod_line_request_input(lpLine, nullptr) == -1)
					{

						throw Exceptions::SystemException(
							"gpiod_line_request_input"
						);
					}
					break;

				case PinDirection::Out:
					if (gpiod_line_request_output(lpLine, nullptr, static_cast<int>(value)) == -1)
					{

						throw Exceptions::SystemException(
							"gpiod_line_request_output"
						);
					}
					break;

				default:
					throw Exceptions::NotImplementedException();
			}
	#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValues value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

			SetDirection(
				direction,
				static_cast<PinValue>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void WaitForEdge(PinEdges edge)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

			if (!WaitForEdge(edge, TimeSpan::Infinite))
			{
			}
		}
		// @throw AL::Exceptions::Exception
		// @return false if maxWaitTime elapsed
		bool WaitForEdge(PinEdges edge, TimeSpan maxWaitTime)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			if (lpLine != nullptr)
			{
				gpiod_line_release(
					lpLine
				);

				lpLine = gpiod_chip_get_line(
					lpChip,
					static_cast<unsigned int>(GetNumber())
				);
			}
			else if (!(lpLine = gpiod_chip_get_line(lpChip, static_cast<unsigned int>(GetNumber()))))
			{

				throw Exceptions::SystemException(
					"gpiod_chip_get_line"
				);
			}

			int expectedEventType = 0;

			switch (edge)
			{
				case PinEdges::Both:
				{
					if (gpiod_line_request_both_edges_events(lpLine, nullptr) == -1)
					{

						throw Exceptions::SystemException(
							"gpiod_line_request_both_edges_events"
						);
					}
				}
				break;

				case PinEdges::Rising:
				{
					expectedEventType = GPIOD_LINE_EVENT_RISING_EDGE;

					if (gpiod_line_request_rising_edge_events(lpLine, nullptr) == -1)
					{

						throw Exceptions::SystemException(
							"gpiod_line_request_rising_edge_events"
						);
					}
				}
				break;

				case PinEdges::Falling:
				{
					expectedEventType = GPIOD_LINE_EVENT_RISING_EDGE;

					if (gpiod_line_request_falling_edge_events(lpLine, nullptr) == -1)
					{

						throw Exceptions::SystemException(
							"gpiod_line_request_falling_edge_events"
						);
					}
				}
				break;

				default:
					throw Exceptions::NotImplementedException();
			}

			timespec timeout;
			timeout.tv_sec = maxWaitTime.ToSeconds();
			timeout.tv_nsec = maxWaitTime.ToMicroseconds() - TimeSpan::FromSeconds(timeout.tv_sec).ToMicroseconds();

			switch (gpiod_line_event_wait(lpLine, &timeout))
			{
				case -1:
					throw Exceptions::SystemException(
						"gpiod_line_event_wait"
					);

				case 0:
					return false;
			}

			gpiod_line_event event;

			do
			{
				if (gpiod_line_event_read(lpLine, &event) == -1)
				{

					throw Exceptions::SystemException(
						"gpiod_line_event_read"
					);
				}
			} while (expectedEventType && (event.event_type != expectedEventType));

			return true;
	#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
	#endif
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		void Unexport()
		{
			if (IsExported())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
				gpiod_line_release(
					lpLine
				);

				gpiod_chip_close(
					lpChip
				);

				lpLine = nullptr;
				lpChip = nullptr;
	#endif
#endif
			}
		}

		Pin& operator = (Pin&& pin)
		{
			Unexport();

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_GPIOD)
			lpChip = pin.lpChip;
			pin.lpChip = nullptr;

			lpLine = pin.lpLine;
			pin.lpLine = nullptr;
	#endif
#endif
			
			number = pin.number;
			deviceId = pin.deviceId;

			return *this;
		}
	};
}
