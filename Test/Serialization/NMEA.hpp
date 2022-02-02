#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Serialization/NMEA.hpp>

#include "NMEA_Messages.hpp"

// @throw AL::Exception
static void AL_Serialization_NMEA()
{
	using namespace AL;
	using namespace AL::Serialization;

	NMEASentence sentence;

	for (auto message : AL_Serialization_NMEA_Messages)
	{
		if (!NMEA::FromString(sentence, message))
		{

			continue;
		}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		// Type
		{
			OS::Console::Write(
				"Type: "
			);

			if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GGA))
			{
				OS::Console::Write(
					"GGA"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GLL))
			{
				OS::Console::Write(
					"GLL"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GSV))
			{
				OS::Console::Write(
					"GSV"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::RMC))
			{
				OS::Console::Write(
					"RMC"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::VTG))
			{
				OS::Console::Write(
					"VTG"
				);
			}
			else
			{
				OS::Console::Write(
					"Undefined"
				);
			}

			OS::Console::WriteLine();
		}

		// Constellation
		{
			OS::Console::Write(
				"Constellation: "
			);

			if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GA))
			{
				OS::Console::Write(
					"Galileo"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GB))
			{
				OS::Console::Write(
					"Beidou"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GL))
			{
				OS::Console::Write(
					"Glonass"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GN))
			{
				OS::Console::Write(
					"*"
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GP))
			{
				OS::Console::Write(
					"GPS"
				);
			}
			else
			{
				OS::Console::Write(
					"Undefined"
				);
			}

			OS::Console::WriteLine();
		}

		// Payload
		{
			if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GGA))
			{
				OS::Console::WriteLine(
					"Fix Quality: %u",
					sentence.GGA.FixQuality
				);

				OS::Console::WriteLine(
					"Time: [%lu:%lu:%lu.%lu]",
					sentence.GGA.Time.Hours,
					sentence.GGA.Time.Minutes,
					sentence.GGA.Time.Seconds,
					sentence.GGA.Time.Deciseconds
				);

				OS::Console::WriteLine(
					"Altitude: %.f",
					sentence.GGA.Altitude
				);

				OS::Console::WriteLine(
					"Latitude: %.f",
					sentence.GGA.Latitude
				);

				OS::Console::WriteLine(
					"Longitude: %.f",
					sentence.GGA.Longitude
				);

				OS::Console::WriteLine(
					"Relative Accuracy: %.f",
					sentence.GGA.RelativeAccuracyH
				);

				OS::Console::WriteLine(
					"Satellites In Use: %u",
					sentence.GGA.NumberOfSatellitesInUse
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GLL))
			{
				OS::Console::WriteLine(
					"Valid: %s",
					ToString(sentence.GLL.Valid).GetCString()
				);

				OS::Console::WriteLine(
					"Time: [%lu:%lu:%lu.%lu]",
					sentence.GLL.Time.Hours,
					sentence.GLL.Time.Minutes,
					sentence.GLL.Time.Seconds,
					sentence.GLL.Time.Deciseconds
				);

				OS::Console::WriteLine(
					"Latitude: %.f",
					sentence.GLL.Latitude
				);

				OS::Console::WriteLine(
					"Longitude: %.f",
					sentence.GLL.Longitude
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::GSV))
			{
				OS::Console::WriteLine(
					"Satellites In View: %lu",
					sentence.GSV.SatellitesInView
				);

				if (sentence.GSV.MessageIndex < sentence.GSV.MessageCount)
				{

					sentence.GSV.SatellitesInView = sizeof(sentence.GSV.Satellites) / sizeof(NMEASatellite);
				}
				if (sentence.GSV.MessageIndex == sentence.GSV.MessageCount)
				{

					sentence.GSV.SatellitesInView = sentence.GSV.SatellitesInView % (sizeof(sentence.GSV.Satellites) / sizeof(NMEASatellite));
				}

				for (AL::size_t i = 0; i < sentence.GSV.SatellitesInView; ++i)
				{
					OS::Console::WriteLine(
						"PRN: %lu, SNR: %lu, Azimuth: %lu, Elevation: %lu",
						sentence.GSV.Satellites[i].PRN,
						sentence.GSV.Satellites[i].SNR,
						sentence.GSV.Satellites[i].Azimuth,
						sentence.GSV.Satellites[i].Elevation
					);
				}
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::RMC))
			{
				OS::Console::WriteLine(
					"Valid: %s",
					ToString(sentence.RMC.Valid).GetCString()
				);

				OS::Console::WriteLine(
					"Time: [%lu:%lu:%lu.%lu]",
					sentence.RMC.Time.Hours,
					sentence.RMC.Time.Minutes,
					sentence.RMC.Time.Seconds,
					sentence.RMC.Time.Deciseconds
				);

				OS::Console::WriteLine(
					"Variation: %.f",
					sentence.RMC.Variation
				);

				OS::Console::WriteLine(
					"Latitude: %.f",
					sentence.RMC.Latitude
				);

				OS::Console::WriteLine(
					"Longitude: %.f",
					sentence.RMC.Longitude
				);

				OS::Console::WriteLine(
					"Speed: %.f (Knots)",
					sentence.RMC.GroundSpeedInKnots
				);
			}
			else if (sentence.Type.IsSet(AL::Serialization::NMEASentences::VTG))
			{
				OS::Console::WriteLine(
					"Speed: %.f (Knots)",
					sentence.VTG.SpeedInKnots
				);

				OS::Console::WriteLine(
					"Speed: %.f (KPH)",
					sentence.VTG.GroundSpeedInKPH
				);

				OS::Console::WriteLine(
					"Heading: %.f",
					sentence.VTG.Heading
				);

				OS::Console::WriteLine(
					"Heading: %.f (Magnetic)",
					sentence.VTG.HeadingMagnetic
				);
			}
		}

		OS::Console::WriteLine();
#endif
	}
}
