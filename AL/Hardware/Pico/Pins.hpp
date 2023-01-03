#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include <boards/pico.h>

namespace AL::Hardware::Pico
{
	enum PINS : uint8
	{
		PINS_LED      = PICO_DEFAULT_LED_PIN,

		PINS_SMPS     = PICO_SMPS_MODE_PIN,

		PINS_UART_RX  = PICO_DEFAULT_UART_RX_PIN,
		PINS_UART_TX  = PICO_DEFAULT_UART_TX_PIN,

		PINS_I2C_SCL  = PICO_DEFAULT_I2C_SCL_PIN,
		PINS_I2C_SDA  = PICO_DEFAULT_I2C_SDA_PIN,

		PINS_SPI_RX   = PICO_DEFAULT_SPI_RX_PIN,
		PINS_SPI_TX   = PICO_DEFAULT_SPI_TX_PIN,
		PINS_SPI_CS   = PICO_DEFAULT_SPI_CSN_PIN,
		PINS_SPI_SCLK = PICO_DEFAULT_SPI_SCK_PIN
	};
}