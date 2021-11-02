#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Hardware/Drivers/RTL_SDR.hpp>

static void AL_Hardware_Drivers_RTL_SDR()
{
	using namespace AL;
	using namespace AL::Hardware::Drivers;

	RTL_SDR sdr;

	sdr.Open();

	sdr.Close();
}
