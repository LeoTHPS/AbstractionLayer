#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include <hardware/uart.h>

namespace AL::Hardware::Drivers::RP2040
{
	enum class UARTFlags : uint8
	{
		None         = 0x00,

		Parity       = 0x01,
		Parity_Odd   = 0x02 | Parity,
		Parity_Even  = 0x04 | Parity,

		Use2StopBits = 0x08
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(UARTFlags);

	class UART
	{
		Bool               isOpen = False;

		::uart_inst*       uart;
		GPIOPin            rx;
		GPIOPin            tx;
		uint32             baud;
		BitMask<UARTFlags> flags;

		UART(const UART&) = delete;

	public:
		typedef UARTFlags Flags;

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
			),
			flags(
				Move(uart.flags)
			)
		{
			uart.isOpen = False;
		}

		UART(::uart_inst* uart, GPIOPin rx, GPIOPin tx, uint32 baud, Flags flags)
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
			),
			flags(
				flags
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

		Bool IsReadable() const
		{
			if (!::uart_is_readable(uart))
			{

				return False;
			}

			return True;
		}

		Bool IsWritable() const
		{
			if (!::uart_is_writable(uart))
			{

				return False;
			}

			return True;
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

		auto& GetFlags() const
		{
			return flags;
		}

		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"UART already open"
			);

			baud = ::uart_init(
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

			::uart_set_hw_flow(
				GetUART(),
				false,
				false
			);

			::uart_set_format(
				GetUART(),
				8,
				flags.IsSet(Flags::Use2StopBits) ? 2 : 1,
				flags.IsSet(Flags::Parity) ? (flags.IsSet(Flags::Parity_Odd) ? ::UART_PARITY_ODD : ::UART_PARITY_EVEN) : ::UART_PARITY_NONE
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

		Void Read(Void* lpBuffer, size_t size) const
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
			flags      = Move(uart.flags);

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
