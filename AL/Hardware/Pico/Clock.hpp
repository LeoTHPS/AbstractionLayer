#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "GPIO.hpp"

#include <hardware/clocks.h>

namespace AL::Hardware::Pico
{
	namespace Extensions
	{
		static clock_index clock_gpio_get_index(uint gpio)
		{
			switch (gpio)
			{
				case 21: return clk_gpout0;
				case 23: return clk_gpout1;
				case 24: return clk_gpout2;
				case 25: return clk_gpout3;
			}

			invalid_params_if(CLOCKS, true);

			return CLK_COUNT;
		}

		inline void        clock_gpio_get_div(uint gpio, uint32_t& divider, uint8_t& dividerFraction)
		{
			auto index      = clock_gpio_get_index(gpio);
			auto div        = clocks_hw->clk[index].div;
			divider         = (div & CLOCKS_CLK_GPOUT0_DIV_INT_BITS) >> CLOCKS_CLK_GPOUT0_DIV_INT_LSB;
			dividerFraction = (div & CLOCKS_CLK_GPOUT0_DIV_FRAC_BITS) >> CLOCKS_CLK_GPOUT0_DIV_FRAC_LSB;
		}
		inline void        clock_gpio_get_div(uint gpio, float& divider)
		{
			uint32_t div;
			uint8_t  divFraction;
			clock_gpio_get_div(gpio, div, divFraction);

			divider = div + (divFraction / 256.0f);
		}
	}

	enum class Clocks : Get_Enum_Or_Integer_Base<::clock_index>::Type
	{
		ADC        = ::clk_adc,
		RTC        = ::clk_rtc,
		USB        = ::clk_usb,
		GPIO0      = ::clk_gpout0,
		GPIO1      = ::clk_gpout1,
		GPIO2      = ::clk_gpout2,
		GPIO3      = ::clk_gpout3,
		System     = ::clk_sys,
		Reference  = ::clk_ref,
		Peripheral = ::clk_peri
	};

	class Clock
	{
		inline static Bool isInitialized = False;

		Clock() = delete;

	public:
		static Bool IsInitialized()
		{
			return isInitialized;
		}

		static Void Init()
		{
			if (!IsInitialized())
			{
				::clocks_init();

				isInitialized = True;
			}
		}

		// Get the current frequency of the specified clock
		static uint32 GetFrequency(Clocks clock)
		{
			return ::clock_get_hz(
				static_cast<::clock_index>(clock)
			);
		}

		// Get the current frequency of the specified gpio pin
		static uint32 GetGPIOFrequency(Clocks clock, GPIOPin pin)
		{
			auto frequency = GetFrequency(
				clock
			);

			return GetGPIOFrequency(
				frequency,
				pin
			);
		}
		// Get the current frequency of the specified gpio pin
		static uint32 GetGPIOFrequency(uint32 frequency, GPIOPin pin)
		{
			if (::gpio_get_function(pin) != ::GPIO_FUNC_GPCK)
			{

				return 0;
			}

			float divider;

			Extensions::clock_gpio_get_div(
				pin,
				divider
			);

			return BitConverter::FromFloat<uint32>(
				frequency / divider
			);
		}

		// Measure a clocks frequency using the Frequency counter
		static uint32 GetCountedFrequency(Clocks clock)
		{
			auto frequencyKHz = ::frequency_count_khz(
				static_cast<::clock_index>(clock)
			);

			return frequencyKHz * 1000;
		}

		// Measure a pin frequency using the Frequency counter
		static uint32 GetCountedGPIOFrequency(Clocks clock, GPIOPin pin)
		{
			auto frequency = GetCountedFrequency(
				clock
			);

			return GetGPIOFrequency(
				frequency,
				pin
			);
		}

		// @param src The main clock source, can be 0
		// @param aux_src The auxiliary clock source, which depends on which clock is being set. Can be 0
		// @param src_freq Frequency of the input clock source
		// @param freq Requested frequency
		static Bool Configure(Clocks clock, uint32 src, uint32 aux_src, uint32 src_freq, uint32 freq)
		{
			if (!::clock_configure(static_cast<::clock_index>(clock), src, aux_src, src_freq, freq))
			{

				return False;
			}

			return True;
		}

		// @param pin The GPIO pin to output the clock to. Valid GPIOs are: 21, 23, 24, 25. These GPIOs are connected to the GPOUT0-3 clock generators
		// @param src The source clock. See the register field CLOCKS_CLK_GPOUT0_CTRL_AUXSRC for a full list. The list is the same for each GPOUT clock generator
		// @param div The amount to divide the source clock by. This is useful to not overwhelm the GPIO pin with a fast clock
		static Void ConfigureGPIO(Clocks clock, GPIOPin pin, uint32 src, Float div)
		{
			::clock_gpio_init(
				pin,
				src,
				div
			);
		}
		// @param pin The GPIO pin to output the clock to. Valid GPIOs are: 21, 23, 24, 25. These GPIOs are connected to the GPOUT0-3 clock generators
		// @param src The source clock. See the register field CLOCKS_CLK_GPOUT0_CTRL_AUXSRC for a full list. The list is the same for each GPOUT clock generator
		// @param div_int The fractional part of the value to divide the source clock by. This is in range of 0..255 (/256).
		// @param div_frac The integer part of the value to divide the source clock by. This is useful to not overwhelm the GPIO pin with a fast clock. this is in range of 1..2^24-1.
		static Void ConfigureGPIO(Clocks clock, GPIOPin pin, uint32 src, uint32 div_int, uint32 div_frac)
		{
			::clock_gpio_init_int_frac(
				pin,
				src,
				div_int,
				div_frac
			);
		}

		// @param pin The GPIO pin to run the clock from. Valid GPIOs are: 20 and 22
		// @param src_freq Frequency of the input clock source
		// @param freq Requested frequency
		static Bool ConfigureGPIOInput(Clocks clock, GPIOPin pin, uint32 src_freq, uint32 freq)
		{
			if (!::clock_configure_gpin(static_cast<::clock_index>(clock), pin, src_freq, freq))
			{

				return False;
			}

			return True;
		}

		static Void Stop(Clocks clock)
		{
			::clock_stop(
				static_cast<::clock_index>(clock)
			);
		}

		static Void EnableGPIO(GPIOPin pin, Bool set = True)
		{
			::gpio_set_function(
				pin,
				set ? ::GPIO_FUNC_GPCK : ::GPIO_FUNC_NULL
			);
		}
	};
}
