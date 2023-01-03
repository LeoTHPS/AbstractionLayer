#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include <hardware/uart.h>

namespace AL::Hardware::Drivers::RP2040
{
	class UART
	{
		Bool         isOpen = False;

		::uart_inst* uart;
		GPIOPin      rx;
		GPIOPin      tx;
		uint32       baud;

		UART(const UART&) = delete;

	public:
		typedef I2CAddress Address;

		UART(UART&& uart)
			: isOpen(
				uart.isOpen
			),
			uart(
				uart.uart
			),
			rx(
				uart.rx
			),
			tx(
				uart.tx
			),
			baud(
				uart.baud
			)
		{
			uart.isOpen = False;
		}

		UART(::uart_inst* uart, GPIOPin rx, GPIOPin tx, uint32 baud)
			: uart(
				uart
			),
			rx(
				rx
			),
			tx(
				tx
			),
			baud(
				baud
			)
		{
		}

		virtual ~UART()
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

		auto GetUART() const
		{
			return uart;
		}

		auto GetRX() const
		{
			return rx;
		}

		auto GetTX() const
		{
			return tx;
		}

		auto GetBaud() const
		{
			return baud;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"UART already open"
			);

			::uart_init(
				GetUART(),
				GetBaud()
			);

			::gpio_set_function(
				GetTX(),
				::GPIO_FUNC_UART
			);

			::gpio_set_function(
				GetRX(),
				::GPIO_FUNC_UART
			);

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::uart_deinit(
					GetUART()
				);

				isOpen = False;
			}
		}

		Void Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"UART not open"
			);

			::uart_read_blocking(
				GetUART(),
				reinterpret_cast<::uint8_t*>(lpBuffer),
				size
			);
		}

		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"UART not open"
			);

			::uart_write_blocking(
				GetUART(),
				reinterpret_cast<const ::uint8_t*>(lpBuffer),
				size
			);
		}

		UART& operator = (UART&& uart)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = uart.isOpen;
			uart.isOpen = False;

			this->uart = uart.uart;
			rx         = uart.rx;
			tx         = uart.tx;
			baud       = uart.baud;

			return *this;
		}

		Bool operator == (const UART& uart) const
		{
			if (GetUART() != uart.GetUART())
			{

				return False;
			}

			if (GetRX() != uart.GetRX())
			{

				return False;
			}

			if (GetTX() != uart.GetTX())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const UART& uart) const
		{
			if (operator==(uart))
			{

				return False;
			}

			return True;
		}
	};
}
