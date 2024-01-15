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

	struct NMEADateTime
	{
		NMEATime Time;
		uint8    Month, Day, Year;
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
		GA  = 0x01, // Galileo Positioning System
		GB  = 0x02, // BeiDou
		GI  = 0x04, // NavIC (IRNSS)
		GL  = 0x08, // GLONASS
		GN  = 0x10, // Global Navigation Satellite System
		GP  = 0x20, // Global Positioning System,
		GQ  = 0x40, // QZSS

		GGA = 0x100,
		GLL = 0x200,
		GSA = 0x400,
		GSV = 0x800,
		RMC = 0x1000,
		VTG = 0x2000
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(NMEASentences);

	enum class NMEAFixModes : uint8
	{
		Manual,
		Automatic
	};

	enum class NMEAFixTypes : uint8
	{
		None  = 1,
		Fix2D = 2,
		Fix3D = 3
	};

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
				uint32       PRN;
				NMEAFixModes FixMode;
				NMEAFixTypes FixType;
			} GSA;

			struct
			{
				uint32        MessageCount;
				uint32        MessageIndex;
				NMEASatellite Satellites[4];
				uint32        SatellitesInView;
			} GSV;

			struct
			{
				NMEADateTime DateTime;
				Bool         Valid;
				Double       Latitude;
				Double       Longitude;
				Double       Variation;
				Float        GroundSpeedInKnots;
			} RMC;

			struct
			{
				Bool   Valid;
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
		typedef Collections::Array<String> _StringChunks;

		NMEA() = delete;

	public:
		// @throw AL::Exception
		static Void FromString(NMEASentence& sentence, const String& string)
		{
			if (!string.StartsWith('$') || !string.EndsWith("\r\n"))
			{

				throw Exception(
					"Invalid format"
				);
			}

			auto stringChunks = string.Split(',');
			stringChunks[0] = stringChunks[0].SubString(1);
			stringChunks[stringChunks.GetSize() - 1] = stringChunks[stringChunks.GetSize() - 1].SubString(0, stringChunks[stringChunks.GetSize() - 1].GetLength() - 2);

			if (!stringChunks[stringChunks.GetSize() - 1].StartsWith('*'))
			{
				auto tmp = stringChunks[stringChunks.GetSize() - 1].Split('*');

				if (tmp.GetSize() != 2)
				{

					throw Exception(
						"Invalid format"
					);
				}

				stringChunks.SetSize(stringChunks.GetSize() + 1);
				stringChunks[stringChunks.GetSize() - 2] = Move(tmp[0]);
				stringChunks[stringChunks.GetSize() - 1] = Move(tmp[1]);
			}

			if (stringChunks[0].StartsWith("GA"))      sentence.Type = NMEASentences::GA;
			else if (stringChunks[0].StartsWith("GB")) sentence.Type = NMEASentences::GB;
			else if (stringChunks[0].StartsWith("GI")) sentence.Type = NMEASentences::GI;
			else if (stringChunks[0].StartsWith("GL")) sentence.Type = NMEASentences::GL;
			else if (stringChunks[0].StartsWith("GN")) sentence.Type = NMEASentences::GN;
			else if (stringChunks[0].StartsWith("GP")) sentence.Type = NMEASentences::GP;
			else if (stringChunks[0].StartsWith("GQ")) sentence.Type = NMEASentences::GQ;
			else throw Exception("Invalid talker id");

			if (stringChunks[0].CompareAt("GGA", 2))
			{
				sentence.Type.Add(NMEASentences::GGA);

				try
				{
					FromString_GGA(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing GGA"
					);
				}
			}
			else if (stringChunks[0].CompareAt("GLL", 2))
			{
				sentence.Type.Add(NMEASentences::GLL);

				try
				{
					FromString_GLL(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing GLL"
					);
				}
			}
			else if (stringChunks[0].CompareAt("GSA", 2))
			{
				sentence.Type.Add(NMEASentences::GSA);

				try
				{
					FromString_GSA(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing GSA"
					);
				}
			}
			else if (stringChunks[0].CompareAt("GSV", 2))
			{
				sentence.Type.Add(NMEASentences::GSV);

				try
				{
					FromString_GSV(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing GSV"
					);
				}
			}
			else if (stringChunks[0].CompareAt("RMC", 2))
			{
				sentence.Type.Add(NMEASentences::RMC);

				try
				{
					FromString_RMC(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing RMC"
					);
				}
			}
			else if (stringChunks[0].CompareAt("VTG", 2))
			{
				sentence.Type.Add(NMEASentences::VTG);

				try
				{
					FromString_VTG(
						sentence,
						stringChunks
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error parsing VTG"
					);
				}
			}
			else
			{

				throw Exception(
					"Invalid frame type"
				);
			}
		}
		// @throw AL::Exception
		static Void FromString(NMEASentence& sentence, const WString& wstring)
		{
			FromString(
				sentence,
				wstring.ToString()
			);
		}

	private:
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
		static Void FromString_GGA(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 11)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if (stringChunks[1].GetLength() != 0)
			{
				sentence.GGA.Time = FromString_Time(
					stringChunks[1]
				);
			}

			if ((sentence.GGA.FixQuality = AL::FromString<NMEAFixQualities>(stringChunks[6])) != NMEAFixQualities::Invalid)
			{
				if (!stringChunks[10].Compare('M'))
				{

					throw Exception(
						"Invalid height unit"
					);
				}

				if (!stringChunks[12].Compare('M'))
				{

					throw Exception(
						"Invalid separation unit"
					);
				}

				sentence.GGA.NumberOfSatellitesInUse = AL::FromString<uint32>(
					stringChunks[7]
				);

				sentence.GGA.RelativeAccuracyH = AL::FromString<Double>(
					stringChunks[8]
				);

				sentence.GGA.Latitude = FromString_Latitude(
					stringChunks[2]
				);

				if (stringChunks[3].Compare('S'))
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

				sentence.GGA.Longitude = FromString_Longitude(
					stringChunks[4]
				);

				if (stringChunks[5].Compare('W'))
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

				sentence.GGA.Altitude = AL::FromString<Double>(
					stringChunks[9]
				);
			}
		}
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GLL.html
		static Void FromString_GLL(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 7)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if (stringChunks[5].GetLength() != 0)
			{
				sentence.GLL.Time = FromString_Time(
					stringChunks[5]
				);
			}

			if ((sentence.GLL.Valid = stringChunks[6].Compare('A')) != False)
			{
				sentence.GLL.Latitude = FromString_Latitude(
					stringChunks[1]
				);

				if (stringChunks[2].Compare('S'))
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

				sentence.GLL.Longitude = FromString_Longitude(
					stringChunks[3]
				);

				if (stringChunks[4].Compare('W'))
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
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_gsa.html
		static Void FromString_GSA(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 4)
			{

				throw Exception(
					"Invalid format"
				);
			}

			sentence.GSA.PRN     = AL::FromString<uint32>(stringChunks[3]);
			sentence.GSA.FixMode = AL::FromString<NMEAFixModes>(stringChunks[1]);
			sentence.GSA.FixType = AL::FromString<NMEAFixTypes>(stringChunks[2]);
		}
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSV.html
		static Void FromString_GSV(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 4)
			{

				throw Exception(
					"Invalid format"
				);
			}

			sentence.GSV.MessageCount = AL::FromString<uint32>(
				stringChunks[1]
			);
			sentence.GSV.MessageIndex = AL::FromString<uint32>(
				stringChunks[2]
			);
			sentence.GSV.SatellitesInView = AL::FromString<uint32>(
				stringChunks[3]
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

			if (stringChunks.GetSize() < (4 + (satelliteCount * 4)))
			{

				throw Exception(
					"Invalid format"
				);
			}

			for (size_t i = 0; i < satelliteCount; ++i)
			{
				auto j = 4 + (i * 4);

				sentence.GSV.Satellites[i].PRN = AL::FromString<uint32>(
					stringChunks[j + 0]
				);
				sentence.GSV.Satellites[i].Elevation = AL::FromString<uint32>(
					stringChunks[j + 1]
				);
				sentence.GSV.Satellites[i].Azimuth = AL::FromString<uint32>(
					stringChunks[j + 2]
				);
				sentence.GSV.Satellites[i].SNR = AL::FromString<uint32>(
					stringChunks[j + 3]
				);
			}
		}
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_RMC.html
		static Void FromString_RMC(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 10)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if (stringChunks[1].GetLength() != 0)
			{
				sentence.RMC.DateTime.Time = FromString_Time(
					stringChunks[1]
				);
			}

			if ((sentence.RMC.Valid = stringChunks[2].Compare('A')) != False)
			{
				if (stringChunks[9].GetLength() != 6)
				{

					throw Exception(
						"Invalid date format"
					);
				}

				sentence.RMC.Latitude = FromString_Latitude(
					stringChunks[3]
				);

				if (stringChunks[4].Compare('S'))
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

				sentence.RMC.Longitude = FromString_Longitude(
					stringChunks[5]
				);

				if (stringChunks[6].Compare('W'))
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

				sentence.RMC.GroundSpeedInKnots = AL::FromString<Float>(
					stringChunks[7]
				);
				sentence.RMC.Variation = AL::FromString<Double>(
					stringChunks[8]
				);

				{
					String buffer(String::END, 3);

					buffer[0] = stringChunks[9][0];
					buffer[1] = stringChunks[9][1];
					sentence.RMC.DateTime.Day = AL::FromString<AL::uint8>(buffer);

					buffer[0] = stringChunks[9][2];
					buffer[1] = stringChunks[9][3];
					sentence.RMC.DateTime.Month = AL::FromString<AL::uint8>(buffer);

					buffer[0] = stringChunks[9][4];
					buffer[1] = stringChunks[9][5];
					sentence.RMC.DateTime.Year = AL::FromString<AL::uint8>(buffer);
				}
			}
		}
		// @throw AL::Exception
		// https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_VTG.html
		static Void FromString_VTG(NMEASentence& sentence, const _StringChunks& stringChunks)
		{
			if (stringChunks.GetSize() < 10)
			{

				throw Exception(
					"Invalid format"
				);
			}

			if ((sentence.VTG.Valid = !stringChunks[9].Compare('N')) == True)
			{
				if (!stringChunks[6].Compare('N'))
				{

					throw Exception(
						"Invalid speed unit"
					);
				}

				if (!stringChunks[8].Compare('K'))
				{

					throw Exception(
						"Invalid ground speed unit"
					);
				}

				sentence.VTG.Heading = AL::FromString<Double>(
					stringChunks[1]
				);
				sentence.VTG.HeadingIsRelTrueNorth = stringChunks[2].Compare(
					'T'
				);
				sentence.VTG.HeadingMagnetic = AL::FromString<Double>(
					stringChunks[3]
				);
				sentence.VTG.HeadingIsRelMagneticNorth = stringChunks[4].Compare(
					'M'
				);
				sentence.VTG.SpeedInKnots = AL::FromString<Double>(
					stringChunks[5]
				);
				sentence.VTG.GroundSpeedInKPH = AL::FromString<Double>(
					stringChunks[7]
				);
			}
		}

		// @throw AL::Exception
		static NMEATime FromString_Time(const String& string)
		{
			if (string.GetLength() != 9)
			{

				throw Exception(
					"Invalid time format"
				);
			}

			NMEATime time;
			String   buffer(String::END, 2);

			buffer[0] = string[0];
			buffer[1] = string[1];
			time.Hours = AL::FromString<uint32>(buffer);

			buffer[0] = string[2];
			buffer[1] = string[3];
			time.Minutes = AL::FromString<uint32>(buffer);

			buffer[0] = string[4];
			buffer[1] = string[5];
			time.Seconds = AL::FromString<uint32>(buffer);

			buffer[0] = string[7];
			buffer[1] = string[8];
			time.Deciseconds = AL::FromString<uint32>(buffer);

			return time;
		}

		// @throw AL::Exception
		static Double FromString_Latitude(const String& string)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^(\\d{2})(\\d{2})\\.(\\d{2})(\\d{2})(\\d+)?$", string))
			{

				throw Exception(
					"Invalid latitude format"
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
					"Invalid longitude format"
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
