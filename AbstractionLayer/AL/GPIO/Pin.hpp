#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#if __has_include(<gpiod.hpp>)
	#include <gpiod.hpp>

	#define AL_DEPENDENCY_GPIOD
#endif

#include <sys/stat.h>

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
#if !defined(AL_DEPENDENCY_GPIOD)
		class gpiod_chip;
		class gpiod_line;
#endif

		gpiod_chip* lpChip;
		gpiod_line* lpLine;

		PinNumber number;
		DeviceId deviceId;

		Pin(const Pin&) = delete;

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

	public:
		// @throw AL::Exceptions::Exception
		// @return false if already exported
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number, PinDirection direction, PinValue value)
		{
#if defined(AL_DEPENDENCY_GPIOD)
			char device[32];
			sprintf(device, "%u", deviceId);
			
			if (auto lpChip = gpiod_chip_open_by_number(static_cast<unsigned int>(deviceId)))
			{
				if (auto lpLine = gpiod_chip_get_line(lpChip, static_cast<unsigned int>(number)))
				{
					{
						struct stat s;

						if (!stat(String::Format("/sys/class/gpio/gpio%u", number).GetCString(), &s) && S_ISDIR(s.st_mode))
						{
							gpiod_line_release(lpLine);
							gpiod_chip_close(lpChip);

							return false;
						}
					}

					gpiod_line_request_config request = { 0 };
					request.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;

					if (direction == PinDirection::Out)
					{

						request.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
					}

					if (gpiod_line_request(lpLine, &request, static_cast<int>(value)) == -1)
					{
						gpiod_line_release(lpLine);
						gpiod_chip_close(lpChip);

						throw Exceptions::SystemException(
							"gpiod_line_request"
						);
					}

					pin = Pin(
						lpChip,
						lpLine,
						deviceId,
						number
					);

					return true;
				}

				gpiod_chip_close(lpChip);

				throw Exceptions::Exception(
					"GPIO line #%u on device #%u not found",
					number,
					deviceId
				);
			}

			throw Exceptions::Exception(
				"GPIO device #%u not found",
				deviceId
			);
#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
#endif
		}
		// @throw AL::Exceptions::Exception
		// @return false if already exported
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number, PinDirection direction, PinValues value = PinValues::Low)
		{
			return Export(
				pin,
				deviceId,
				number,
				direction,
				static_cast<PinValue>(value)
			);
		}

		Pin()
			: Pin(
				nullptr,
				nullptr,
				0,
				0
			)
		{
		}

		Pin(Pin&& pin)
			: lpChip(
				pin.lpChip
			),
			lpLine(
				pin.lpLine
			),
			number(
				pin.number
			),
			deviceId(
				pin.deviceId
			)
		{
			pin.lpChip = nullptr;
			pin.lpLine = nullptr;
		}

		virtual ~Pin()
		{
			Unexport();
		}

		bool IsExported() const
		{
			return lpLine != nullptr;
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
		PinValue Read() const
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_DEPENDENCY_GPIOD)
			int value;

			if ((value = gpiod_line_get_value(lpLine)) == -1)
			{

				throw Exceptions::SystemException(
					"gpiod_line_get_value"
				);
			}

			return static_cast<PinValue>(
				value
			);
#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		void Write(PinValue value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

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
		}
		// @throw AL::Exceptions::Exception
		void Write(PinValues value)
		{
			Write(
				static_cast<PinValue>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValue value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_DEPENDENCY_GPIOD)
			gpiod_line_release(
				lpLine
			);
			
			lpLine = gpiod_chip_get_line(
				lpChip,
				static_cast<unsigned int>(number)
			);

			gpiod_line_request_config request = { 0 };
			request.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;

			if (direction == PinDirection::Out)
			{

				request.request_type = GPIOD_LINE_DIRECTION_OUTPUT;
			}

			if (gpiod_line_request(lpLine, &request, static_cast<int>(value)) == -1)
			{

				throw Exceptions::SystemException(
					"gpiod_line_request"
				);
			}
#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
#endif
		}
		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValues value = PinValues::Low)
		{
			SetDirection(
				direction,
				static_cast<PinValue>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void WaitForEdge(PinEdges edge)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_DEPENDENCY_GPIOD)
			gpiod_line_release(
				lpLine
			);

			lpLine = gpiod_chip_get_line(
				lpChip,
				static_cast<unsigned int>(number)
			);

			gpiod_line_request_config request = { 0 };
			request.request_type = GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES;

			switch (edge)
			{
				case PinEdges::Rising:
					request.request_type = GPIOD_LINE_REQUEST_EVENT_RISING_EDGE;
					break;

				case PinEdges::Falling:
					request.request_type = GPIOD_LINE_REQUEST_EVENT_FALLING_EDGE;
					break;
			}

			if (gpiod_line_request(lpLine, &request, 0) == -1)
			{

				throw Exceptions::SystemException(
					"gpiod_line_request"
				);
			}
#else
			throw Exceptions::DependencyMissingException(
				"gpiod"
			);
#endif
		}

		void Unexport()
		{
			if (IsExported())
			{
#if defined(AL_DEPENDENCY_GPIOD)
				gpiod_line_release(lpLine);
				gpiod_chip_close(lpChip);
#endif

				lpLine = nullptr;
				lpChip = nullptr;
			}
		}

		Pin& operator = (Pin&& pin)
		{
			Unexport();

			lpChip = pin.lpChip;
			pin.lpChip = nullptr;

			lpLine = pin.lpLine;
			pin.lpLine = nullptr;

			number = pin.number;
			deviceId = pin.deviceId;

			return *this;
		}
	};
}
