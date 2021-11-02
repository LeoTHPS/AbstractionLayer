#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Hardware/Drivers/RTL_SDR.hpp>

static void AL_Hardware_Drivers_RTL_SDR()
{
	using namespace AL;
	using namespace AL::Hardware::Drivers;

	RTL_SDR            sdr;
	RTL_SDR_DeviceInfo sdrInfo;
	auto               sdrCount = RTL_SDR::GetDeviceCount();

	for (decltype(sdrCount) sdrIndex = 0; sdrIndex < sdrCount; ++sdrIndex)
	{
		try
		{
			RTL_SDR::ReadDeviceInfo(
				sdrIndex,
				sdrInfo
			);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"Name:         %s\n"
				"Serial:       %s\n"
				"Product:      %s\n"
				"Manufacturer: %s",
				sdrInfo.Name.GetCString(),
				sdrInfo.Serial.GetCString(),
				sdrInfo.Product.GetCString(),
				sdrInfo.Manufacturer.GetCString()
			);
#endif

			sdr = RTL_SDR(
				sdrIndex
			);

			sdr.Open();

			try
			{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
				OS::Console::WriteLine(
					"Index:                %lu\n"
					"Handle:               0x%p\n"
					"Sample Rate:          %lu\n"
					"Tuner Gain:           %li\n"
					"Tuner IF Gain:        %li\n"
					"Center Frequency:     %llu\n"
					"Frequency Correction: %li\n"
					"Auto Gain:            %s\n"
					"Tuner Auto Gain:      %s\n"
					"Direct Sampling:      %s\n"
					"Offset Tuning:        %s",
					sdr.GetIndex(),
					sdr.GetHandle(),
					sdr.GetSampleRate(),
					sdr.GetTunerGain(),
					sdr.GetTunerIFGain(),
					sdr.GetCenterFrequency(),
					sdr.GetFrequencyCorrection(),
					sdr.IsAutoGainEnabled()       ? "Enabled" : "Disabled",
					sdr.IsTunerAutoGainEnabled()  ? "Enabled" : "Disabled",
					sdr.IsDirectSamplingEnabled() ? "Enabled" : "Disabled",
					sdr.IsOffsetTuningEnabled()   ? "Enabled" : "Disabled"
				);
#endif
			}
			catch (Exception&)
			{
				sdr.Close();

				throw;
			}

			sdr.Close();
		}
		catch (const AL::Exception& exception)
		{
			OS::Console::WriteLine(
				exception.GetMessage()
			);

			if (auto lpInnerException = exception.GetInnerException())
			{
				do
				{
					OS::Console::WriteLine(
						lpInnerException->GetMessage()
					);
				} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
			}
		}
	}
}
