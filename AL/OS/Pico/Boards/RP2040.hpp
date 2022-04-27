#pragma once
#include "AL/Common.hpp"

#include "Board.hpp"

#include <boards/pico.h>

namespace AL::OS::Pico::Boards
{
	enum RP2040_ADDRESS_MAP : uint32
	{
		RP2040_ADDRESS_MAP_ROM                                      = 0x00000000,
		RP2040_ADDRESS_MAP_ROM_BASE                                 = 0x00000000,

		RP2040_ADDRESS_MAP_XIP                                      = 0x10000000,
		RP2040_ADDRESS_MAP_XIP_BASE                                 = 0x10000000,
		RP2040_ADDRESS_MAP_XIP_NOALLOC_BASE                         = 0x11000000,
		RP2040_ADDRESS_MAP_XIP_NOCACHE_BASE                         = 0x12000000,
		RP2040_ADDRESS_MAP_XIP_NOCACHE_NOALLOC_BASE                 = 0x13000000,
		RP2040_ADDRESS_MAP_XIP_CTRL_BASE                            = 0x14000000,
		RP2040_ADDRESS_MAP_XIP_SRAM_BASE                            = 0x15000000,
		RP2040_ADDRESS_MAP_XIP_SRAM_END                             = 0x15004000,
		RP2040_ADDRESS_MAP_XIP_SSI_BASE                             = 0x18000000,

		RP2040_ADDRESS_MAP_SRAM                                     = 0x20000000,
		RP2040_ADDRESS_MAP_SRAM_BASE                                = 0x20000000,
		RP2040_ADDRESS_MAP_SRAM_STRIPED_BASE                        = 0x20000000,
		RP2040_ADDRESS_MAP_SRAM_STRIPED_END                         = 0x20040000,
		RP2040_ADDRESS_MAP_SRAM4_BASE                               = 0x20040000,
		RP2040_ADDRESS_MAP_SRAM5_BASE                               = 0x20041000,
		RP2040_ADDRESS_MAP_SRAM_END                                 = 0x20042000,
		RP2040_ADDRESS_MAP_SRAM0_BASE                               = 0x21000000,
		RP2040_ADDRESS_MAP_SRAM1_BASE                               = 0x21010000,
		RP2040_ADDRESS_MAP_SRAM2_BASE                               = 0x21020000,
		RP2040_ADDRESS_MAP_SRAM3_BASE                               = 0x21030000,

		RP2040_ADDRESS_MAP_APB_PERIPHERALS                          = 0x40000000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_SYSINFO_BASE             = 0x40000000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_SYSCFG_BASE              = 0x40004000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_CLOCKS_BASE              = 0x40008000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_RESETS_BASE              = 0x4000C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PSM_BASE                 = 0x40010000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_IO_BANK0_BASE            = 0x40014000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_IO_QSPI_BASE             = 0x40018000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PADS_BANK0_BASE          = 0x4001C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PADS_QSPI_BASE           = 0x40020000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_XOSC_BASE                = 0x40024000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PLL_SYS_BASE             = 0x40028000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PLL_USB_BASE             = 0x4002C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_BUSCTRL_BASE             = 0x40030000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_UART0_BASE               = 0x40034000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_UART1_BASE               = 0x40038000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_SPI0_BASE                = 0x4003C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_SPI1_BASE                = 0x40040000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_I2C0_BASE                = 0x40044000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_I2C1_BASE                = 0x40048000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_ADC_BASE                 = 0x4004C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_PWM_BASE                 = 0x40050000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_TIMER_BASE               = 0x40054000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_WATCHDOG_BASE            = 0x40058000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_RTC_BASE                 = 0x4005C000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_ROSC_BASE                = 0x40060000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_VREG_AND_CHIP_RESET_BASE = 0x40064000,
		RP2040_ADDRESS_MAP_APB_PERIPHERALS_TBMAN_BASE               = 0x4006C000,

		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS                     = 0x50000000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_DMA_BASE            = 0x50000000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_USBCTRL_BASE        = 0x50100000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_DPRAM_BASE          = 0x50100000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_USBCTRL_REGS_BASE   = 0x50110000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_PIO0_BASE           = 0x50200000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_PIO1_BASE           = 0x50300000,
		RP2040_ADDRESS_MAP_AHB_LITE_PERIPHERALS_XIP_AUX_BASE        = 0x50400000,

		RP2040_ADDRESS_MAP_IOPORT_REGISTERS                         = 0xD0000000,
		RP2040_ADDRESS_MAP_IOPORT_REGISTERS_BASE                    = 0xD0000000,

		RP2040_ADDRESS_MAP_CORTEX_M0_PLUS_INTERNAL_REGISTERS        = 0xE0000000,
		RP2040_ADDRESS_MAP_CORTEX_M0_PLUS_INTERNAL_REGISTERS_BASE   = 0xE0000000
	};

	enum RP2040_DEFAULT_PINS : uint8
	{
		RP2040_DEFAULT_PIN_LED      = PICO_DEFAULT_LED_PIN,

		RP2040_DEFAULT_PIN_SMPS     = PICO_SMPS_MODE_PIN,

		RP2040_DEFAULT_PIN_UART_RX  = PICO_DEFAULT_UART_RX_PIN,
		RP2040_DEFAULT_PIN_UART_TX  = PICO_DEFAULT_UART_TX_PIN,

		RP2040_DEFAULT_PIN_I2C_SCL  = PICO_DEFAULT_I2C_SCL_PIN,
		RP2040_DEFAULT_PIN_I2C_SDA  = PICO_DEFAULT_I2C_SDA_PIN,

		RP2040_DEFAULT_PIN_SPI_RX   = PICO_DEFAULT_SPI_RX_PIN,
		RP2040_DEFAULT_PIN_SPI_TX   = PICO_DEFAULT_SPI_TX_PIN,
		RP2040_DEFAULT_PIN_SPI_CS   = PICO_DEFAULT_SPI_CSN_PIN,
		RP2040_DEFAULT_PIN_SPI_SCLK = PICO_DEFAULT_SPI_SCK_PIN
	};

	typedef Board<PICO_FLASH_SIZE_BYTES, RP2040_ADDRESS_MAP, RP2040_DEFAULT_PINS> RP2040;
}
