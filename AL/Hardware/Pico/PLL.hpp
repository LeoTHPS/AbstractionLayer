#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "AL/OS/Timer.hpp"

#include <hardware/pll.h>

namespace AL::Hardware::Pico
{
	enum class PLLCSFlags : uint32
	{
		// PLL is locked
		Locked = PLL_CS_LOCK_BITS,
		// Passes the reference clock to the output instead of the divided VCO.
		// The VCO continues to run so the user can switch between the reference clock and the divided VCO but the output will glitch when doing so.
		Bypass = PLL_CS_BYPASS_BITS,
		// Divides the PLL input reference clock.
		// Behaviour is undefined for div=0.
		// PLL output will be unpredictable during refdiv changes, wait for lock=1 before using it.
		RefDiv = PLL_CS_REFDIV_BITS
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(PLLCSFlags);

	enum class PLLPowerFlags : uint32
	{
		// To save power set high when PLL output not required.
		PowerDown             = PLL_PWR_PD_BITS,
		// Nothing is achieved by setting this low.
		PowerDown_DSM         = PLL_PWR_DSMPD_BITS,
		// To save power set high when PLL output not required or bypass=1.
		PowerDown_VCO         = PLL_PWR_VCOPD_BITS,
		// To save power set high when PLL output not required or bypass=1.
		PowerDown_PostDivider = PLL_PWR_POSTDIVPD_BITS
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(PLLPowerFlags);

	class PLL
	{
		PLL() = delete;

	public:
		static Bool IsLocked(::PLL pll)
		{
			if (!GetFlag(pll, PLLCSFlags::Locked))
			{

				return False;
			}

			return True;
		}

		// @param ref_div Input clock divider
		// @param vco_freq Requested output from the VCO (voltage controlled oscillator)
		// @param post_div1 Post Divider 1 - range 1-7. Must be >= post_div2
		// @param post_div2 Post Divider 2 - range 1-7
		static Void Init(::PLL pll, uint32 ref_div, uint32 vco_freq, uint32 post_div1, uint32 post_div2)
		{
			::pll_init(
				pll,
				static_cast<uint>(ref_div),
				static_cast<uint>(vco_freq),
				static_cast<uint>(post_div1),
				static_cast<uint>(post_div2)
			);
		}

		// This will turn off the power to the specified PLL.
		// Note this function does not currently check if the PLL is in use before powering it off so should be used with care.
		static Void Deinit(::PLL pll)
		{
			::pll_deinit(
				pll
			);
		}

		static Bool GetFlag(::PLL pll, PLLCSFlags flag)
		{
			typedef decltype(pll->cs) T;

			if (!BitMask<T>::IsSet(pll->cs, static_cast<T>(flag)))
			{

				return False;
			}

			return True;
		}
		static Bool GetFlag(::PLL pll, PLLPowerFlags flag)
		{
			typedef decltype(pll->pwr) T;

			if (!BitMask<T>::IsSet(pll->pwr, static_cast<T>(flag)))
			{

				return False;
			}

			return True;
		}

		static Void SetFlag(::PLL pll, PLLCSFlags flag, Bool set = True)
		{
			typedef decltype(pll->cs) T;

			BitMask<T>::Set(
				pll->cs,
				static_cast<T>(flag),
				set
			);
		}
		static Void SetFlag(::PLL pll, PLLPowerFlags flag, Bool set = True)
		{
			typedef decltype(pll->pwr) T;

			BitMask<T>::Set(
				pll->pwr,
				static_cast<T>(flag),
				set
			);
		}

		// @return AL::False on timeout
		static Bool WaitForLock(::PLL pll, TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			OS::Timer timer;
			Bool      timeout;

			while (!(timeout = (timer.GetElapsed() >= maxWaitTime)) && IsLocked(pll))
			{
			}

			return timeout;
		}
	};
}
