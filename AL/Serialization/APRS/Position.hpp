#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

namespace AL::Serialization::APRS
{
	class Position 
	{
		Bool         isMessagingEnabled   = False;
		Bool         isCompressionEnabled = False;

		Packet       packet;

		int32        altitude;
		Float        latitude;
		Float        longitude;

		String       comment;
		String::Char symbolTable;
		String::Char symbolTableKey;

		Position(Packet&& packet, int32 altitude, Float latitude, Float longitude, String&& comment, String::Char symbolTable, String::Char symbolTableKey, Bool isMessagingEnabled, Bool isCompressionEnabled)
			: isMessagingEnabled(
				isMessagingEnabled
			),
			isCompressionEnabled(
				isCompressionEnabled
			),
			packet(
				Move(packet)
			),
			altitude(
				altitude
			),
			latitude(
				latitude
			),
			longitude(
				longitude
			),
			comment(
				Move(comment)
			),
			symbolTable(
				symbolTable
			),
			symbolTableKey(
				symbolTableKey
			)
		{
		}

	public:
		static Float CalculateDistance(Float latitude1, Float longitude1, Float latitude2, Float longitude2)
		{
			auto latitude_delta  = Math::Rad(latitude2 - latitude1);
			auto longitude_delta = Math::Rad(longitude2 - longitude1);
			auto latitude_1      = Math::Rad(latitude1);
			auto latitude_2      = Math::Rad(latitude2);
			auto a               = Math::Sin(latitude_delta / 2) * Math::Sin(latitude_delta / 2) + Math::Sin(longitude_delta / 2) * Math::Sin(longitude_delta / 2) * Math::Cos(latitude_1) * Math::Cos(latitude_2);
			auto distance        = 2 * Math::Atan2(Math::Sqrt(a), Math::Sqrt(1 - a));

			return (distance * 6371) * 3280.84f;
		}
		static Float CalculateDistance3D(int32 altitude1, Float latitude1, Float longitude1, int32 altitude2, Float latitude2, Float longitude2)
		{
			auto  latitude_delta  = Math::Rad(latitude2 - latitude1);
			auto  longitude_delta = Math::Rad(longitude2 - longitude1);
			auto  latitude_1      = Math::Rad(latitude1);
			auto  latitude_2      = Math::Rad(latitude2);
			auto  a               = Math::Sin(latitude_delta / 2) * Math::Sin(latitude_delta / 2) + Math::Sin(longitude_delta / 2) * Math::Sin(longitude_delta / 2) * Math::Cos(latitude_1) * Math::Cos(latitude_2);
			auto  distance        = 2 * Math::Atan2(Math::Sqrt(a), Math::Sqrt(1 - a));
			Float distance_z      = 0;

			if (altitude1 >= altitude2)
				distance_z = static_cast<Float>(altitude1 - altitude2);
			else
				distance_z = static_cast<Float>(altitude2 - altitude1);

			return ((distance * 6371) * 3280.84f) + distance_z;
		}

		// @throw AL::Exception
		static Position Create(String&& sender, String&& tocall, DigiPath&& path, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey, String&& comment = "", Bool isMessagingEnabled = False, Bool isCompressionEnabled = False)
		{
			if (!Validate_Altitude(altitude))
				throw Exception("Invalid altitude");

			if (!Validate_Latitude(latitude))
				throw Exception("Invalid latitude");

			if (!Validate_Longitude(longitude))
				throw Exception("Invalid longitude");

			if (!Validate_SymbolTable(symbolTable))
				throw Exception("Invalid symbol table");

			if (!Validate_SymbolTableKey(symbolTableKey))
				throw Exception("Invalid symbol table key");

			auto packet = Packet::Create(Move(sender), Move(tocall), Move(path), "");

			return Position(Move(packet), altitude, latitude, longitude, Move(comment), symbolTable, symbolTableKey, isMessagingEnabled, isCompressionEnabled);
		}
		// @throw AL::Exception
		static Position Create(const String& sender, const String& tocall, const DigiPath& path, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey, const String& comment = "", Bool isMessagingEnabled = False, Bool isCompressionEnabled = False)
		{
			if (!Validate_Altitude(altitude))
				throw Exception("Invalid altitude");

			if (!Validate_Latitude(latitude))
				throw Exception("Invalid latitude");

			if (!Validate_Longitude(longitude))
				throw Exception("Invalid longitude");

			if (!Validate_SymbolTable(symbolTable))
				throw Exception("Invalid symbol table");

			if (!Validate_SymbolTableKey(symbolTableKey))
				throw Exception("Invalid symbol table key");

			auto packet = Packet::Create(String(sender), String(tocall), DigiPath(path), "");

			return Position(Move(packet), altitude, latitude, longitude, String(comment), symbolTable, symbolTableKey, isMessagingEnabled, isCompressionEnabled);
		}

		// @throw AL::Exception
		static Position CreateIS(String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey, String&& comment = "", Bool isMessagingEnabled = False, Bool isCompressionEnabled = False)
		{
			if (!Validate_Altitude(altitude))
				throw Exception("Invalid altitude");

			if (!Validate_Latitude(latitude))
				throw Exception("Invalid latitude");

			if (!Validate_Longitude(longitude))
				throw Exception("Invalid longitude");

			if (!Validate_SymbolTable(symbolTable))
				throw Exception("Invalid symbol table");

			if (!Validate_SymbolTableKey(symbolTableKey))
				throw Exception("Invalid symbol table key");

			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Position(Move(packet), altitude, latitude, longitude, Move(comment), symbolTable, symbolTableKey, isMessagingEnabled, isCompressionEnabled);
		}
		// @throw AL::Exception
		static Position CreateIS(const String& sender, const String& tocall, const DigiPath& path, const String& igate, QConstructs qConstruct, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey, const String& comment = "", Bool isMessagingEnabled = False, Bool isCompressionEnabled = False)
		{
			if (!Validate_Altitude(altitude))
				throw Exception("Invalid altitude");

			if (!Validate_Latitude(latitude))
				throw Exception("Invalid latitude");

			if (!Validate_Longitude(longitude))
				throw Exception("Invalid longitude");

			if (!Validate_SymbolTable(symbolTable))
				throw Exception("Invalid symbol table");

			if (!Validate_SymbolTableKey(symbolTableKey))
				throw Exception("Invalid symbol table key");

			auto packet = Packet::CreateIS(String(sender), String(tocall), DigiPath(path), String(igate), qConstruct, "");

			return Position(Move(packet), altitude, latitude, longitude, String(comment), symbolTable, symbolTableKey, isMessagingEnabled, isCompressionEnabled);
		}

		// @throw AL::Exception
		static Position FromPacket(Packet&& value)
		{
			Bool isMessagingEnabled   = False;
			Bool isCompressionEnabled = False;

			String       comment;
			int32        altitude           = 0;
			String::Char symbolTable        = '\0';
			String::Char symbolTableKey     = '\0';

			Float        latitude           = 0;
			uint16       latitudeHours      = 0;
			uint16       latitudeMinutes    = 0;
			uint16       latitudeSeconds    = 0;
			String::Char latitudeNorthSouth = 'N';

			Float        longitude          = 0;
			uint16       longitudeHours     = 0;
			uint16       longitudeMinutes   = 0;
			uint16       longitudeSeconds   = 0;
			String::Char longitudeWestEast  = 'E';

			auto matches_GetUInt16 = [](Regex::MatchCollection& matches, size_t offset)
			{
				auto& source = matches[offset];

				for (size_t i = 0; i < source.GetLength(); ++i)
				{
					if (source[i] == '0')
						continue;
					else if (i == 0)
						break;

					return AL::FromString<uint16>(
						source.SubString(i)
					);
				}

				return AL::FromString<uint16>(
					source
				);
			};

			auto matches_Decompress = [&symbolTable, &symbolTableKey, &latitude, &longitude, &comment, &altitude](Regex::MatchCollection& matches)
			{
				auto match_IsValid = [](const Regex::MatchCollection& matches, size_t index)
				{
					for (auto c : matches[index])
					{
						if (!Math::IsInRange<char>(c - 33, '!', '{'))
						{

							return False;
						}
					}

					return True;
				};

				if (!match_IsValid(matches, 3) || !match_IsValid(matches, 4) || !match_IsValid(matches, 6))
				{

					return False;
				}

				comment        = Move(matches[8]);
				latitude       = 90 - (((matches[3][0] - 33) * 753571) + ((matches[3][1] - 33) * 8281) + ((matches[3][2] - 33) * 91) + (matches[3][3] - 33)) / 380926.0f;
				longitude      = -180 + (((matches[4][0] - 33) * 753571) + ((matches[4][1] - 33) * 8281) + ((matches[4][2] - 33) * 91) + (matches[4][3] - 33)) / 190463.0f;
				symbolTable    = matches[2][0];
				symbolTableKey = matches[5][0];
				char cs[2]     = { static_cast<char>(matches[6][0] - 33), static_cast<char>(matches[6][1] - 33) };
				// char t         = static_cast<char>(matches[7][0] - 33);

				if (cs[0] == '{')
				{
					// TODO: radio range
				}
				else if (Math::IsInRange(cs[0], '!', 'z'))
				{
					// TODO: course/speed
				}

				if (cs[0] != ' ')
				{
					// TODO: altitude
				}

				return True;
			};

			// Lat/Long Position Report Format - without Timestamp
			// Lat/Long Position Report Format - with Data Extension (no Timestamp)
			if (value.GetContent().StartsWith('!') || (isMessagingEnabled = value.GetContent().StartsWith('=')))
			{
				Regex::MatchCollection matches;

				if (Regex::Match(matches, "^[!=]((\\d{2})(\\d{2})\\.(\\d{2})([NS])(.)(\\d{3})(\\d{2})\\.(\\d{2})([EW])(.))(.*)$", value.GetContent()))
				{
					latitudeHours      = matches_GetUInt16(matches, 2);
					latitudeMinutes    = matches_GetUInt16(matches, 3);
					latitudeSeconds    = matches_GetUInt16(matches, 4);
					latitudeNorthSouth = matches[5][0];
					symbolTable        = matches[6][0];
					longitudeHours     = matches_GetUInt16(matches, 7);
					longitudeMinutes   = matches_GetUInt16(matches, 8);
					longitudeSeconds   = matches_GetUInt16(matches, 9);
					longitudeWestEast  = matches[10][0];
					symbolTableKey     = matches[11][0];
					comment            = Move(matches[12]);
				}
				else if (Regex::Match(matches, "^[!=]((.)([!-{]){4}([!-{]){4}(.)([!-{]){2}(.))(.*)$", value.GetContent()))
				{
					isCompressionEnabled = True;

					if (!matches_Decompress(matches))
						throw Exception("Invalid compression");
				}
				else
					throw Exception("Invalid format");

				if (Regex::Match(matches, "(\\/A=(-?\\d+))", comment))
				{
					comment.Remove(
						matches[1]
					);

					altitude = AL::FromString<int16>(matches[2]);
				}
			}
			// Lat/Long Position Report Format - with Timestamp
			// Lat/Long Position Report Format - with Data Extension and Timestamp
			else if (value.GetContent().StartsWith('/') || (isMessagingEnabled = value.GetContent().StartsWith('@')))
			{
				Regex::MatchCollection matches;

				if (Regex::Match(matches, "^[\\/@]((\\d+)[hz\\/](\\d{2})(\\d{2})\\.(\\d{2})([NS])(.)(\\d{3})(\\d{2})\\.(\\d{2})([EW])(.))(.*)$", value.GetContent()))
				{
					latitudeHours      = matches_GetUInt16(matches, 3);
					latitudeMinutes    = matches_GetUInt16(matches, 4);
					latitudeSeconds    = matches_GetUInt16(matches, 5);
					latitudeNorthSouth = matches[6][0];
					symbolTable        = matches[7][0];
					longitudeHours     = matches_GetUInt16(matches, 8);
					longitudeMinutes   = matches_GetUInt16(matches, 9);
					longitudeSeconds   = matches_GetUInt16(matches, 10);
					longitudeWestEast  = matches[11][0];
					symbolTableKey     = matches[12][0];
					comment            = Move(matches[13]);
				}
				else if (Regex::Match(matches, "^[\\/@]((.)([!-{]){4}([!-{]){4}(.)([!-{]){2}(.))(.*)$", value.GetContent()))
				{
					isCompressionEnabled = True;

					if (!matches_Decompress(matches))
						throw Exception("Invalid compression");
				}
				else
					throw Exception("Invalid format");

				if (Regex::Match(matches, "(\\/A=(-?\\d+))", comment))
				{
					comment.Remove(
						matches[1]
					);

					altitude = AL::FromString<int16>(matches[2]);
				}
			}

			if (!isCompressionEnabled)
			{
				// http://www.aprs.org/aprs12/datum.txt

				latitude  = (latitudeHours  + (latitudeMinutes / 60.0f)  + (latitudeSeconds / 6000.0f))  * ((latitudeNorthSouth == 'N') ? 1 : -1);
				longitude = (longitudeHours + (longitudeMinutes / 60.0f) + (longitudeSeconds / 6000.0f)) * ((longitudeWestEast  == 'E') ? 1 : -1);
			}

			return Position(Move(value), altitude, latitude, longitude, Move(comment), symbolTable, symbolTableKey, isMessagingEnabled, isCompressionEnabled);
		}
		// @throw AL::Exception
		static Position FromPacket(const Packet& value)
		{
			return FromPacket(Packet(value));
		}

		// @throw AL::Exception
		static Position FromString(const String& value)
		{
			auto packet = Packet::FromString(
				value
			);

			return FromPacket(
				packet
			);
		}

		Position()
		{
		}

		Bool IsMessagingEnabled() const
		{
			return isMessagingEnabled;
		}

		Bool IsCompressionEnabled() const
		{
			return isCompressionEnabled;
		}

		auto& GetPath() const
		{
			return packet.GetPath();
		}

		auto& GetIGate() const
		{
			return packet.GetIGate();
		}

		auto& GetToCall() const
		{
			return packet.GetToCall();
		}

		auto& GetSender() const
		{
			return packet.GetSender();
		}

		auto GetAltitude() const
		{
			return altitude;
		}

		auto GetLatitude() const
		{
			return latitude;
		}

		auto GetLongitude() const
		{
			return longitude;
		}

		auto& GetComment() const
		{
			return comment;
		}

		auto& GetQConstruct() const
		{
			return packet.GetQConstruct();
		}

		auto GetSymbolTable() const
		{
			return symbolTable;
		}

		auto GetSymbolTableKey() const
		{
			return symbolTableKey;
		}

		Void EnableMessaging(Bool set = True)
		{
			isMessagingEnabled = set;
		}

		Void EnableCompression(Bool set = True)
		{
			isCompressionEnabled = set;
		}

		Bool SetPath(DigiPath&& value)
		{
			return packet.SetPath(Move(value));
		}
		Bool SetPath(const DigiPath& value)
		{
			return packet.SetPath(value);
		}

		Bool SetSender(String&& value)
		{
			return packet.SetSender(Move(value));
		}
		Bool SetSender(const String& value)
		{
			return packet.SetSender(value);
		}

		Bool SetAltitude(int32 value)
		{
			if (!Validate_Altitude(value))
			{

				return False;
			}

			altitude = value;

			return True;
		}

		Bool SetLatitude(Float value)
		{
			if (!Validate_Latitude(value))
			{

				return False;
			}

			latitude = value;

			return True;
		}

		Bool SetLongitude(Float value)
		{
			if (!Validate_Longitude(value))
			{

				return False;
			}

			longitude = value;

			return True;
		}

		Bool SetLatLong(Float latitude, Float longitude)
		{
			if (!Validate_Latitude(latitude) || !Validate_Longitude(longitude))
			{

				return False;
			}

			this->latitude  = latitude;
			this->longitude = longitude;

			return True;
		}

		Bool SetComment(String&& value)
		{
			if (value.GetLength() > 43)
			{

				return False;
			}

			comment = Move(
				value
			);

			return True;
		}
		Bool SetComment(const String& value)
		{
			return SetComment(String(value));
		}

		Bool SetSymbolTable(String::Char value)
		{
			if (!Validate_SymbolTable(value))
			{

				return False;
			}

			symbolTable = value;

			return True;
		}

		Bool SetSymbolTableKey(String::Char value)
		{
			if (!Validate_SymbolTableKey(value))
			{

				return False;
			}

			symbolTableKey = value;

			return True;
		}

		Float CalculateDistance(const Position& position) const
		{
			return CalculateDistance(GetLatitude(), GetLongitude(), position.GetLatitude(), position.GetLongitude());
		}
		Float CalculateDistance3D(const Position& position) const
		{
			return CalculateDistance3D(GetAltitude(), GetLatitude(), GetLongitude(), position.GetAltitude(), position.GetLatitude(), position.GetLongitude());
		}

		Packet ToPacket(Bool include_is_fields) const
		{
			StringBuilder sb;

			if (IsMessagingEnabled())
			{
				sb << '=';

				if (!IsCompressionEnabled())
					ToPacket_Append(sb, GetAltitude(), GetLatitude(), GetLongitude(), GetSymbolTable(), GetSymbolTableKey());
				else
					ToPacket_AppendCompressed(sb, GetAltitude(), GetLatitude(), GetLongitude(), GetSymbolTable(), GetSymbolTableKey());
			}
			else
			{
				sb << '!';

				if (!IsCompressionEnabled())
					ToPacket_Append(sb, GetAltitude(), GetLatitude(), GetLongitude(), GetSymbolTable(), GetSymbolTableKey());
				else
					ToPacket_AppendCompressed(sb, GetAltitude(), GetLatitude(), GetLongitude(), GetSymbolTable(), GetSymbolTableKey());
			}

			sb << GetComment();

			return !include_is_fields ? Packet::Create(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), sb.ToString()) :
										Packet::CreateIS(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), String(GetIGate()), GetQConstruct(), sb.ToString());
		}

		String ToString(Bool include_is_fields) const
		{
			return ToPacket(include_is_fields).ToString(include_is_fields);
		}

	private:
		static Void ToPacket_Append(StringBuilder& sb, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey)
		{
			String::Char latitudeNorthSouth = (latitude >= 0) ? 'N' : 'S';
			auto         latitudeHours      = BitConverter::FromFloat<int16>(latitude, latitude, False);
			auto         latitudeMinutes    = BitConverter::FromFloat<uint16>(((latitude < 0) ? (latitude * -1) : latitude) * 60, latitude, False);
			auto         latitudeSeconds    = BitConverter::FromFloat<uint16>((latitude * 6000) / 60);

			String::Char longitudeWestEast = (longitude >= 0) ? 'E' : 'W';
			auto         longitudeHours    = BitConverter::FromFloat<int16>(longitude, longitude, False);
			auto         longitudeMinutes  = BitConverter::FromFloat<uint16>(((longitude < 0) ? (longitude * -1) : longitude) * 60, longitude, False);
			auto         longitudeSeconds  = BitConverter::FromFloat<uint16>((longitude * 6000) / 60);

			sb.Append(String::Format("%02i%02u.%02u%c", (latitudeHours >= 0) ? latitudeHours : (latitudeHours * -1), latitudeMinutes, latitudeSeconds, latitudeNorthSouth));
			sb.Append(symbolTable);
			sb.Append(String::Format("%03i%02u.%02u%c", (longitudeHours >= 0) ? longitudeHours : (longitudeHours * -1), longitudeMinutes, longitudeSeconds, longitudeWestEast));
			sb.Append(symbolTableKey);
			sb.Append(String::Format("/A=%06i", altitude));
		}
		static Void ToPacket_AppendCompressed(StringBuilder& sb, int32 altitude, Float latitude, Float longitude, String::Char symbolTable, String::Char symbolTableKey)
		{
			// T = 0x30

			// TODO: implement
			throw NotImplementedException();
		}

	private:
		static Bool Validate_Altitude(int32 value)
		{
			return Math::IsInRange<int32>(value, -0xFFFFF, 0xFFFFFF);
		}

		static Bool Validate_Latitude(Float value)
		{
			return value <= 90;
		}

		static Bool Validate_Longitude(Float value)
		{
			return Math::IsInRange<Float>(value, -180, 180);
		}

		static Bool Validate_SymbolTable(String::Char value)
		{
			return value != '\0';
		}

		static Bool Validate_SymbolTableKey(String::Char value)
		{
			return value != '\0';
		}
	};
}
