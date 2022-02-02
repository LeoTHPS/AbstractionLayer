#pragma once
#include "AL/Common.hpp"

namespace AL::Serialization
{
	struct NMEATime
	{
		uint32 Hours;
		uint32 Minutes;
		uint32 Seconds;
		uint32 Deciseconds;
	};

	struct NMEASatellite
	{
		uint32 PRN;
		uint32 SNR;
		uint32 Azimuth;
		uint32 Elevation;
	};

	enum class NMEASentences : uint32
	{
		GA  = 0x01, // Galileo
		GB  = 0x02, // Beidou
		GL  = 0x04, // Glonass
		GN  = 0x08, // *
		GP  = 0x10, // GPS

		GGA = 0x20,
		GLL = 0x40,
		GSV = 0x80,
		RMC = 0x100,
		VTG = 0x200
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(NMEASentences);

	enum class NMEAFixQualities : uint8
	{
		Invalid, GPS, DGPS
	};

	struct NMEASentence
	{
		BitMask<NMEASentences> Type;

		union
		{
			struct
			{
				NMEATime         Time;
				Double           Altitude;
				Double           Latitude;
				Double           Longitude;
				NMEAFixQualities FixQuality;
				Double           RelativeAccuracyH;
				uint16           NumberOfSatellitesInUse;
			} GGA;

			struct
			{
				NMEATime Time;
				Bool     Valid;
				Double   Latitude;
				Double   Longitude;
			} GLL;

			struct
			{
				uint32        MessageCount;
				uint32        MessageIndex;
				NMEASatellite Satellites[4];
				uint32        SatellitesInView;
			} GSV;

			struct
			{
				NMEATime Time;
				Bool     Valid;
				Double   Latitude;
				Double   Longitude;
				Double   Variation;
				Float    GroundSpeedInKnots;
			} RMC;

			struct
			{
				Double Heading;
				Double HeadingMagnetic;
				Bool   HeadingIsRelTrueNorth;
				Bool   HeadingIsRelMagneticNorth;
				Double SpeedInKnots;
				Double GroundSpeedInKPH;
			} VTG;
		};
	};

	class NMEA
	{
		NMEA() = delete;

	public:
		// @throw AL::Exception
		// @return AL::False on not supported
		static Bool FromString(NMEASentence& sentence, const String& string)
		{
			if (!string.StartsWith('$') || !string.EndsWith("\r\n"))
			{

				throw Exception(
					"Invalid input"
				);
			}

			auto message = string.SubString(
				1,
				string.GetLength() - 3
			);

			auto messageChunks = message.Split(
				L','
			);

			sentence = NMEASentence();

			if (!FromString(sentence, messageChunks))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on not supported
		static Bool FromString(NMEASentence& sentence, const WString& wstring)
		{
			auto string = wstring.ToString();

			if (!FromString(sentence, string))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		static Bool FromString(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			auto& type = wstringChunks[0];

			// Talker Field
			{
				if (type.StartsWith("GA"))
				{
					sentence.Type.Add(
						NMEASentences::GA
					);
				}
				else if (type.StartsWith("GB"))
				{
					sentence.Type.Add(
						NMEASentences::GB
					);
				}
				else if (type.StartsWith("GL"))
				{
					sentence.Type.Add(
						NMEASentences::GL
					);
				}
				else if (type.StartsWith("GN"))
				{
					sentence.Type.Add(
						NMEASentences::GN
					);
				}
				else if (type.StartsWith("GP"))
				{
					sentence.Type.Add(
						NMEASentences::GP
					);
				}
				else
				{

					return False;
				}
			}

			// Message Type
			{
				if (type.EndsWith("GGA", True))
				{
					sentence.Type.Add(
						NMEASentences::GGA
					);

					FromString_GGA(
						sentence,
						wstringChunks
					);

					return True;
				}
				else if (type.EndsWith("GLL", True))
				{
					sentence.Type.Add(
						NMEASentences::GLL
					);

					FromString_GLL(
						sentence,
						wstringChunks
					);

					return True;
				}
				else if (type.EndsWith("GSV", True))
				{
					sentence.Type.Add(
						NMEASentences::GSV
					);

					FromString_GSV(
						sentence,
						wstringChunks
					);

					return True;
				}
				else if (type.EndsWith("RMC", True))
				{
					sentence.Type.Add(
						NMEASentences::RMC
					);

					FromString_RMC(
						sentence,
						wstringChunks
					);

					return True;
				}
				else if (type.EndsWith("VTG", True))
				{
					sentence.Type.Add(
						NMEASentences::VTG
					);

					FromString_VTG(
						sentence,
						wstringChunks
					);

					return True;
				}
			}

			return False;
		}
		// @throw AL::Exception
		static Void FromString_GGA(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			if (wstringChunks.GetSize() != 15)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if ((sentence.GGA.FixQuality = AL::FromString<NMEAFixQualities>(wstringChunks[6])) != NMEAFixQualities::Invalid)
			{
				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^(\\d\\d)(\\d\\d)(\\d\\d).(\\d\\d)$", wstringChunks[1]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					sentence.GGA.Time.Hours = AL::FromString<uint32>(
						matches[1]
					);
					sentence.GGA.Time.Minutes = AL::FromString<uint32>(
						matches[2]
					);
					sentence.GGA.Time.Seconds = AL::FromString<uint32>(
						matches[3]
					);
					sentence.GGA.Time.Deciseconds = AL::FromString<uint32>(
						matches[4]
					);
				}

				sentence.GGA.NumberOfSatellitesInUse = AL::FromString<uint32>(
					wstringChunks[7]
				);

				sentence.GGA.RelativeAccuracyH = AL::FromString<Double>(
					wstringChunks[8]
				);

				sentence.GGA.Latitude = FromString_Latitude(
					wstringChunks[2]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([NS])$", wstringChunks[3]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("S"))
					{
						if (sentence.GGA.Latitude > 0)
						{
							sentence.GGA.Latitude = -sentence.GGA.Latitude;
						}
					}
					else if (sentence.GGA.Latitude < 0)
					{
						sentence.GGA.Latitude = +sentence.GGA.Latitude;
					}
				}

				sentence.GGA.Longitude = FromString_Longitude(
					wstringChunks[4]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([EW])$", wstringChunks[5]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("W"))
					{
						if (sentence.GGA.Longitude > 0)
						{
							sentence.GGA.Longitude = -sentence.GGA.Longitude;
						}
					}
					else if (sentence.GGA.Longitude < 0)
					{
						sentence.GGA.Longitude = +sentence.GGA.Longitude;
					}
				}

				sentence.GGA.Altitude = AL::FromString<Double>(
					wstringChunks[9]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([A-Z])$", wstringChunks[10]) || !matches[1].Compare("M"))
					{

						throw Exception(
							"Invalid format"
						);
					}
				}
			}
		}
		// @throw AL::Exception
		static Void FromString_GLL(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			if (wstringChunks.GetSize() != 8)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if ((sentence.GLL.Valid = wstringChunks[6].Compare("A")) != False)
			{
				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^(\\d\\d)(\\d\\d)(\\d\\d).(\\d\\d)$", wstringChunks[5]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					sentence.GLL.Time.Hours = AL::FromString<uint32>(
						matches[1]
					);
					sentence.GLL.Time.Minutes = AL::FromString<uint32>(
						matches[2]
					);
					sentence.GLL.Time.Seconds = AL::FromString<uint32>(
						matches[3]
					);
					sentence.GLL.Time.Deciseconds = AL::FromString<uint32>(
						matches[4]
					);
				}

				sentence.GLL.Latitude = FromString_Latitude(
					wstringChunks[1]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([NS])$", wstringChunks[2]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("S"))
					{
						if (sentence.GLL.Latitude > 0)
						{
							sentence.GLL.Latitude = -sentence.GLL.Latitude;
						}
					}
					else if (sentence.GLL.Latitude < 0)
					{
						sentence.GLL.Latitude = +sentence.GLL.Latitude;
					}
				}

				sentence.GLL.Longitude = FromString_Longitude(
					wstringChunks[3]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([EW])$", wstringChunks[4]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("W"))
					{
						if (sentence.GLL.Longitude > 0)
						{
							sentence.GLL.Longitude = -sentence.GLL.Longitude;
						}
					}
					else if (sentence.GLL.Longitude < 0)
					{
						sentence.GLL.Longitude = +sentence.GLL.Longitude;
					}
				}
			}
		}
		// @throw AL::Exception
		static Void FromString_GSV(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			if (wstringChunks.GetSize() < 4)
			{

				throw Exception(
					"Invalid format"
				);
			}

			sentence.GSV.MessageCount = AL::FromString<uint32>(
				wstringChunks[1]
			);
			sentence.GSV.MessageIndex = AL::FromString<uint32>(
				wstringChunks[2]
			);
			sentence.GSV.SatellitesInView = AL::FromString<uint32>(
				wstringChunks[3]
			);

			size_t satelliteCount;

			static constexpr size_t MAX_SATELLITES_PER_SENTENCE = sizeof(sentence.GSV.Satellites) / sizeof(NMEASatellite);

			if ((satelliteCount = sentence.GSV.SatellitesInView) > MAX_SATELLITES_PER_SENTENCE)
			{
				if (sentence.GSV.MessageIndex < sentence.GSV.MessageCount)
				{
					satelliteCount = MAX_SATELLITES_PER_SENTENCE;
				}
				else
				{
					satelliteCount = sentence.GSV.SatellitesInView % MAX_SATELLITES_PER_SENTENCE;
				}
			}

			if (wstringChunks.GetSize() < (4 + (satelliteCount * 4)))
			{

				throw Exception(
					"Invalid format"
				);
			}

			for (size_t i = 0; i < satelliteCount; ++i)
			{
				auto j = 4 + (i * 4);

				sentence.GSV.Satellites[i].PRN = AL::FromString<uint32>(
					wstringChunks[j + 0]
				);
				sentence.GSV.Satellites[i].Elevation = AL::FromString<uint32>(
					wstringChunks[j + 1]
				);
				sentence.GSV.Satellites[i].Azimuth = AL::FromString<uint32>(
					wstringChunks[j + 2]
				);
				sentence.GSV.Satellites[i].SNR = AL::FromString<uint32>(
					wstringChunks[j + 3]
				);
			}
		}
		// @throw AL::Exception
		static Void FromString_RMC(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			if (wstringChunks.GetSize() != 13)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if ((sentence.RMC.Valid = wstringChunks[2].Compare("A")) != False)
			{
				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^(\\d\\d)(\\d\\d)(\\d\\d).(\\d\\d)$", wstringChunks[1]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					sentence.RMC.Time.Hours = AL::FromString<uint32>(
						matches[1]
					);
					sentence.RMC.Time.Minutes = AL::FromString<uint32>(
						matches[2]
					);
					sentence.RMC.Time.Seconds = AL::FromString<uint32>(
						matches[3]
					);
					sentence.RMC.Time.Deciseconds = AL::FromString<uint32>(
						matches[4]
					);
				}

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([AV])$", wstringChunks[2]))
					{

						throw Exception(
							"Invalid format"
						);
					}
				}

				sentence.RMC.Latitude = FromString_Latitude(
					wstringChunks[3]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([NS])$", wstringChunks[4]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("S"))
					{
						if (sentence.RMC.Latitude > 0)
						{
							sentence.RMC.Latitude = -sentence.RMC.Latitude;
						}
					}
					else if (sentence.RMC.Latitude < 0)
					{
						sentence.RMC.Latitude = +sentence.RMC.Latitude;
					}
				}

				sentence.RMC.Longitude = FromString_Longitude(
					wstringChunks[5]
				);

				{
					Regex::MatchCollection matches;

					if (!Regex::Match(matches, "^([EW])$", wstringChunks[6]))
					{

						throw Exception(
							"Invalid format"
						);
					}

					if (matches[1].Compare("W"))
					{
						if (sentence.RMC.Longitude > 0)
						{
							sentence.RMC.Longitude = -sentence.RMC.Longitude;
						}
					}
					else if (sentence.RMC.Longitude < 0)
					{
						sentence.RMC.Longitude = +sentence.RMC.Longitude;
					}
				}

				sentence.RMC.GroundSpeedInKnots = AL::FromString<Float>(
					wstringChunks[7]
				);
				sentence.RMC.Variation = AL::FromString<Double>(
					wstringChunks[8]
				);
			}
		}
		// @throw AL::Exception
		static Void FromString_VTG(NMEASentence& sentence, const Collections::Array<String>& wstringChunks)
		{
			if (wstringChunks.GetSize() != 10)
			{

				throw Exception(
					"Invalid format"
				);
			}

			sentence.VTG.Heading = AL::FromString<Double>(
				wstringChunks[1]
			);
			sentence.VTG.HeadingIsRelTrueNorth = wstringChunks[2].Compare(
				"T"
			);
			sentence.VTG.HeadingMagnetic = AL::FromString<Double>(
				wstringChunks[3]
			);
			sentence.VTG.HeadingIsRelMagneticNorth = wstringChunks[4].Compare(
				"M"
			);
			sentence.VTG.SpeedInKnots = AL::FromString<Double>(
				wstringChunks[5]
			);
			sentence.VTG.GroundSpeedInKPH = AL::FromString<Double>(
				wstringChunks[7]
			);
		}

		// @throw AL::Exception
		static Double FromString_Latitude(const String& string)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^(\\d{2})(\\d{2})\\.(\\d{2})(\\d{2})(\\d+)?$", string))
			{

				throw Exception(
					"Invalid format"
				);
			}

			auto days = AL::FromString<Double>(
				matches[1]
			);

			auto hours = AL::FromString<Double>(
				matches[2]
			);

			auto minutes = AL::FromString<Double>(
				matches[3]
			);

			auto seconds = AL::FromString<Double>(
				matches[4]
			);

			auto deciseconds = AL::FromString<Double>(
				matches[5]
			);

			return days + (hours / 60) + (minutes / 3600) + (seconds / 36000) + (deciseconds / 360000);
		}
		// @throw AL::Exception
		static Double FromString_Longitude(const String& string)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^(\\d{2,3})(\\d{2})\\.(\\d{2})(\\d{2})(\\d+)?$", string))
			{

				throw Exception(
					"Invalid format"
				);
			}

			auto days = AL::FromString<Double>(
				matches[1]
			);

			auto hours = AL::FromString<Double>(
				matches[2]
			);

			auto minutes = AL::FromString<Double>(
				matches[3]
			);

			auto seconds = AL::FromString<Double>(
				matches[4]
			);

			auto deciseconds = AL::FromString<Double>(
				matches[5]
			);

			return days + (hours / 60) + (minutes / 3600) + (seconds / 36000) + (deciseconds / 360000);
		}
	};
}
