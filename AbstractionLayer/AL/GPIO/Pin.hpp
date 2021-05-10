#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <sys/stat.h>

	#if __has_include(<gpiod.hpp>)
		#include <gpiod.hpp>

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
		static bool Export(Pin& pin, DeviceId deviceId, PinNumber number, PinDirection direction, PinValue value)
		{
#if defined(AL_PLATFORM_LINUX)
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
#else
			throw Exceptions::NotImplementedException();
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
			AL_ASSERT(IsExported(), "Pin not exported");

			Write(
				static_cast<PinValue>(value)
			);
		}

		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValue value)
		{
			AL_ASSERT(IsExported(), "Pin not exported");

#if defined(AL_PLATFORM_LINUX)
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
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		void SetDirection(PinDirection direction, PinValues value = PinValues::Low)
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

#if defined(AL_PLATFORM_LINUX)
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
				gpiod_line_release(lpLine);
				lpLine = nullptr;

				gpiod_chip_close(lpChip);
				lpChip = nullptr;
	#endif
#endif
			}
		}

		Pin& operator = (Pin&& pin)
		{
			OS::Console::WriteLine(__FUNCTION__);

			Unexport();

			OS::Console::WriteLine(__FUNCTION__);

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
