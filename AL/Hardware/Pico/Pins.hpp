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
		PINS_GPIO_0          = 0,
		PINS_GPIO_1          = 1,
		PINS_GPIO_2          = 2,
		PINS_GPIO_3          = 3,
		PINS_GPIO_4          = 4,
		PINS_GPIO_5          = 5,
		PINS_GPIO_6          = 6,
		PINS_GPIO_7          = 7,
		PINS_GPIO_8          = 8,
		PINS_GPIO_9          = 9,
		PINS_GPIO_10         = 10,
		PINS_GPIO_11         = 11,
		PINS_GPIO_12         = 12,
		PINS_GPIO_13         = 13,
		PINS_GPIO_14         = 14,
		PINS_GPIO_15         = 15,
		PINS_GPIO_16         = 16,
		PINS_GPIO_17         = 17,
		PINS_GPIO_18         = 18,
		PINS_GPIO_19         = 19,
		PINS_GPIO_20         = 20,
		PINS_GPIO_21         = 21,
		PINS_GPIO_22         = 22,
		PINS_GPIO_23         = 23,
		PINS_GPIO_24         = 24,
		PINS_GPIO_25         = 25,
		PINS_GPIO_26         = 26,
		PINS_GPIO_27         = 27,
		PINS_GPIO_28         = 28,

		PINS_COUNT,

		PINS_LED             = PICO_DEFAULT_LED_PIN,

		PINS_SMPS            = PICO_SMPS_MODE_PIN,

		PINS_UART_RX         = PICO_DEFAULT_UART_RX_PIN,
		PINS_UART_TX         = PICO_DEFAULT_UART_TX_PIN,

		PINS_I2C_SCL         = PICO_DEFAULT_I2C_SCL_PIN,
		PINS_I2C_SDA         = PICO_DEFAULT_I2C_SDA_PIN,

		PINS_SPI_RX          = PICO_DEFAULT_SPI_RX_PIN,
		PINS_SPI_TX          = PICO_DEFAULT_SPI_TX_PIN,
		PINS_SPI_CS          = PICO_DEFAULT_SPI_CSN_PIN,
		PINS_SPI_SCLK        = PICO_DEFAULT_SPI_SCK_PIN,

		PINS_CLOCK_INPUT_20  = PINS_GPIO_20,
		PINS_CLOCK_INPUT_22  = PINS_GPIO_22,

		PINS_CLOCK_OUTPUT_21 = PINS_GPIO_21,
		PINS_CLOCK_OUTPUT_23 = PINS_GPIO_23,
		PINS_CLOCK_OUTPUT_24 = PINS_GPIO_24,
		PINS_CLOCK_OUTPUT_25 = PINS_GPIO_25
	};
}
