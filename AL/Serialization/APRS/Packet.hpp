#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Serialization::APRS
{
	enum class QConstructs : uint8
	{
		None,

		// Packet was received from the client directly via a verified connection (FROMCALL=login). The callSSID following the qAC is the server's callsign-SSID.
		AC,
		// Trace packet. This packet tells each server to add login identification to the packet. This packet starts with the callSSID of the originating station following the qAI. See algorithm for more details.
		AI,
		// Packet was received via a client-only port, the FROMCALL does not match the login, and the packet contains either a ,I or qAR construct where the indicated IGate matches the login.
		Ao,
		// Packet is placed on APRS-IS by a receive-only IGate from RF. The callSSID following the qAO is the callSSID of the IGate. Note that receive-only IGates are discouraged on standard APRS frequencies. Please consider a bidirectional IGate that only gates to RF messages for stations heard directly.
		// Packet was received via a client-only port and the FROMCALL does not match the login.
		AO,
		// Packet was received indirectly (via an intermediate server) from an IGate using the ,I construct. The callSSID following the qAr it the callSSID of the IGate.
		Ar,
		// Packet is placed on APRS-IS by an IGate from RF. The callSSID following the qAR is the callSSID of the IGate.
		// Packet was received directly (via a verified connection) from an IGate using the ,I construct. The callSSID following the qAR it the callSSID of the IGate.
		AR,
		// Packet was received from another server or generated by this server. The latter case would be for a beacon generated by the server. Due to the virtual nature of APRS-IS, use of beacon packets by servers is strongly discouraged. The callSSID following the qAS is the login or IP address of the first identifiable server (see algorithm).
		AS,
		// Packet was received from the client directly via a UDP connection. The callSSID following the qAU is the server's callsign-SSID.
		AU,
		// Packet was received from the client directly via a unverified connection (FROMCALL=login). The callSSID following the qAX is the server's callsign-SSID. This construct is in addition to the TCPIP*/TCPXX* construct currently in place. TCPXX and qAX have been depricated on APRS-IS.
		AX,
		// Packet is generated by the server/client/IGate and should not be propagated. The callSSID following the qAZ is the callSSID of the server/client/IGate. This is normally used for connection messages such as messages to USERLIST.
		AZ,
	};

	enum class PacketTypes : uint8
	{
		Unknown,

		Message,
		Position,
		Telemetry
	};

	typedef Collections::Array<String[8]> DigiPath;

	// TODO: support AL::ToString
	static String DigiPath_ToString(const DigiPath& value)
	{
		StringBuilder sb;
		size_t        pathSize = 0;

		for (auto& pathChunk : value)
		{
			if (pathChunk.GetLength() != 0)
			{
				if (pathSize++ != 0)
					sb.Append(',');

				sb.Append(pathChunk);
			}
		}

		return sb.ToString();
	}
	Bool   DigiPath_FromString(DigiPath& path, const String& value);

	// TODO: support AL::ToString
	static String QConstruct_ToString(QConstructs value)
	{
		switch (value)
		{
			case QConstructs::None: return "";
			case QConstructs::AC:   return "qAC";
			case QConstructs::AI:   return "qAI";
			case QConstructs::Ao:   return "qAo";
			case QConstructs::AO:   return "qAO";
			case QConstructs::Ar:   return "qAr";
			case QConstructs::AR:   return "qAR";
			case QConstructs::AS:   return "qAS";
			case QConstructs::AU:   return "qAU";
			case QConstructs::AX:   return "qAX";
			case QConstructs::AZ:   return "qAZ";
		}

		throw OperationNotSupportedException();
	}
	static Bool   QConstruct_FromString(QConstructs& qConstruct, const String& value)
	{
		if (value.Compare("qAC"))
		{
			qConstruct = QConstructs::AC;
			return True;
		}
		else if (value.Compare("qAI"))
		{
			qConstruct = QConstructs::AI;
			return True;
		}
		else if (value.Compare("qAo"))
		{
			qConstruct = QConstructs::Ao;
			return True;
		}
		else if (value.Compare("qAO"))
		{
			qConstruct = QConstructs::AO;
			return True;
		}
		else if (value.Compare("qAr"))
		{
			qConstruct = QConstructs::Ar;
			return True;
		}
		else if (value.Compare("qAR"))
		{
			qConstruct = QConstructs::AR;
			return True;
		}
		else if (value.Compare("qAS"))
		{
			qConstruct = QConstructs::AS;
			return True;
		}
		else if (value.Compare("qAU"))
		{
			qConstruct = QConstructs::AU;
			return True;
		}
		else if (value.Compare("qAX"))
		{
			qConstruct = QConstructs::AX;
			return True;
		}
		else if (value.Compare("qAZ"))
		{
			qConstruct = QConstructs::AZ;
			return True;
		}

		return False;
	}

	class Packet
	{
		friend class Message;
		friend class Position;
		friend class Telemetry;

		friend Bool DigiPath_FromString(DigiPath&, const String&);

		struct Context
		{
			DigiPath    Path;
			String      IGate;
			String      ToCall;
			String      Sender;
			String      Content;
			QConstructs QConstruct;
		};

		PacketTypes type;
		DigiPath    path;
		String      igate;
		String      tocall;
		String      sender;
		String      content;
		QConstructs qConstruct;

		Packet(PacketTypes type, String&& sender, String&& tocall, DigiPath&& path, String&& content)
			: type(
				type
			),
			path(
				Move(path)
			),
			tocall(
				Move(tocall)
			),
			sender(
				Move(sender)
			),
			content(
				Move(content)
			)
		{
		}
		Packet(PacketTypes type, String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, String&& content)
			: type(
				type
			),
			path(
				Move(path)
			),
			igate(
				Move(igate)
			),
			tocall(
				Move(tocall)
			),
			sender(
				Move(sender)
			),
			content(
				Move(content)
			),
			qConstruct(
				qConstruct
			)
		{
		}

	public:
		// @throw AL::Exception
		static Packet Create(String&& sender, String&& tocall, DigiPath&& path, String&& content)
		{
			if (!Validate_StationName(sender))
				throw Exception("Invalid sender");

			if (!Validate_ToCall(tocall))
				throw Exception("Invalid tocall");

			if (!Validate_DigiPath(path))
				throw Exception("Invalid path");

			PacketTypes type;

			if (!GetType(type, content))
				type = PacketTypes::Unknown;

			return Packet(type, Move(sender), Move(tocall), Move(path), Move(content));
		}
		// @throw AL::Exception
		static Packet Create(const String& sender, const String& tocall, const DigiPath& path, const String& content)
		{
			if (!Validate_StationName(sender))
				throw Exception("Invalid sender");

			if (!Validate_ToCall(tocall))
				throw Exception("Invalid tocall");

			if (!Validate_DigiPath(path))
				throw Exception("Invalid path");

			PacketTypes type;

			if (!GetType(type, content))
				type = PacketTypes::Unknown;

			return Packet(type, String(sender), String(tocall), DigiPath(path), String(content));
		}

		// @throw AL::Exception
		static Packet CreateIS(String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, String&& content)
		{
			if (!Validate_StationName(sender, False))
				throw Exception("Invalid sender");

			if (!Validate_ToCall(tocall, False))
				throw Exception("Invalid tocall");

			if (!Validate_DigiPath(path, False))
				throw Exception("Invalid path");

			if (!Validate_DigiPath(igate, False))
				throw Exception("Invalid igate");

			if (qConstruct == QConstructs::None)
				throw Exception("Invalid q construct");

			PacketTypes type;

			if (!GetType(type, content))
				type = PacketTypes::Unknown;

			return Packet(type, Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, Move(content));
		}
		// @throw AL::Exception
		static Packet CreateIS(const String& sender, const String& tocall, const DigiPath& path, const String& igate, QConstructs qConstruct, const String& content)
		{
			if (!Validate_StationName(sender, False))
				throw Exception("Invalid sender");

			if (!Validate_ToCall(tocall, False))
				throw Exception("Invalid tocall");

			if (!Validate_DigiPath(path, False))
				throw Exception("Invalid path");

			if (!Validate_DigiPath(igate, False))
				throw Exception("Invalid igate");

			if (qConstruct == QConstructs::None)
				throw Exception("Invalid q construct");

			PacketTypes type;

			if (!GetType(type, content))
				type = PacketTypes::Unknown;

			return Packet(type, String(sender), String(tocall), DigiPath(path), String(igate), qConstruct, String(content));
		}

		// @throw AL::Exception
		static Packet FromString(const String& value)
		{
			Context context;

			if (!GetContext(context, value))
				throw Exception("Invalid format");

			PacketTypes type;

			if (!GetType(type, context.Content))
				type = PacketTypes::Unknown;

			return Packet(type, Move(context.Sender), Move(context.ToCall), Move(context.Path), Move(context.IGate), context.QConstruct, Move(context.Content));
		}

		Packet()
			: type(
				PacketTypes::Unknown
			)
		{
		}

		auto GetType() const
		{
			return type;
		}

		auto& GetPath() const
		{
			return path;
		}

		auto& GetIGate() const
		{
			return igate;
		}

		auto& GetToCall() const
		{
			return tocall;
		}

		auto& GetSender() const
		{
			return sender;
		}

		auto& GetContent() const
		{
			return content;
		}

		auto& GetQConstruct() const
		{
			return qConstruct;
		}

		Bool SetPath(DigiPath&& value)
		{
			if (!Validate_DigiPath(value))
			{

				return False;
			}

			path = Move(
				value
			);

			return True;
		}
		Bool SetPath(const DigiPath& value)
		{
			return SetPath(DigiPath(value));
		}

		Bool SetSender(String&& value)
		{
			if (!Validate_StationName(value))
			{

				return False;
			}

			sender = Move(
				value
			);

			return True;
		}
		Bool SetSender(const String& value)
		{
			return SetSender(String(value));
		}

		Bool SetContent(String&& value)
		{
			if (value.GetLength() > 256)
			{

				return False;
			}

			content = Move(
				value
			);

			return True;
		}
		Bool SetContent(const String& value)
		{
			return SetContent(String(value));
		}

		String ToString(Bool include_is_fields) const
		{
			StringBuilder sb;
			sb << GetSender() << '>';
			sb << GetToCall() << ',';
			sb << DigiPath_ToString(GetPath());

			if (include_is_fields && (GetIGate().GetLength() != 0) && (GetQConstruct() != QConstructs::None))
			{
				sb << ',' << QConstruct_ToString(GetQConstruct());
				sb << ',' << GetIGate();
			}

			sb << ':' << GetContent();

			return sb.ToString();
		}

	private:
		static Bool GetType(PacketTypes& value, const String& content)
		{
			if (content.StartsWith(':'))
			{
				value = PacketTypes::Message;

				return True;
			}

			if ((content.StartsWith('!') || content.StartsWith('=')) || // Lat/Long Position Report Format — without Timestamp
				(content.StartsWith('/') || content.StartsWith('@')) || // Lat/Long Position Report Format — with Timestamp
				(content.StartsWith('!') || content.StartsWith('=')) || // Lat/Long Position Report Format — with Data Extension (no Timestamp)
				(content.StartsWith('/') || content.StartsWith('@')))   // Lat/Long Position Report Format — with Data Extension and Timestamp
			{
				value = PacketTypes::Position;

				return True;
			}

			if (content.StartsWith("T#"))
			{
				value = PacketTypes::Telemetry;

				return True;
			}

			return False;
		}

		static Bool GetContext(Context& context, const String& value)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^([^>]{3,9})>([^,]+),([^:]+):(.+)$", value))
			{

				return False;
			}

			if (!Validate_ToCall(matches[2], False))
			{

				return False;
			}

			if (!Validate_StationName(matches[1], False))
			{

				return False;
			}

			context.IGate.Clear();
			context.Sender     = Move(matches[1]);
			context.ToCall     = Move(matches[2]);
			context.Content    = Move(matches[4]);
			context.QConstruct = QConstructs::None;

			if (Regex::Match(matches, "^((\\S+?(?=,qA\\w)),(qA\\w),(.+))|(\\S+)$", String(matches[3])))
			{
				if (matches[5].GetLength() == 0)
				{
					if (!Validate_DigiPath(matches[4], False))
					{

						return False;
					}

					context.IGate = Move(matches[4]);

					if (!DigiPath_FromString(context.Path, matches[2]))
					{

						return False;
					}

					if (!QConstruct_FromString(context.QConstruct, matches[3]))
					{

						return False;
					}
				}
				else if (!DigiPath_FromString(context.Path, matches[5]))
				{

					return False;
				}
			}
			else if (!DigiPath_FromString(context.Path, matches[3]))
			{

				return False;
			}

			return True;
		}

		static Bool Validate_ToCall(const String& value, Bool strict = True)
		{
			// TODO: better filter
			return Math::IsInRange<size_t>(value.GetLength(), 2, 9);
		}

		static Bool Validate_DigiPath(const String& value, Bool strict = True)
		{
			// TODO: better filter
			return Math::IsInRange<size_t>(value.GetLength(), 2, 9);
		}
		static Bool Validate_DigiPath(const DigiPath& value, Bool strict = True)
		{
			Bool eop = False;

			for (auto& path : value)
			{
				if (!eop)
				{
					if (path.GetLength() == 0)
					{

						eop = True;
					}
					else if (!Validate_DigiPath(path, strict))
					{

						return False;
					}
				}
				else if (path.GetLength() != 0)
				{

					return False;
				}
			}

			return True;
		}

		static Bool Validate_QConstruct(const String& value)
		{
			return Regex::IsMatch("^qA.$", value);
		}

		static Bool Validate_StationName(const String& value, Bool strict = True)
		{
			// TODO: better filter
			return Math::IsInRange<size_t>(value.GetLength(), 2, 9);
		}
	};
}

inline AL::Bool AL::Serialization::APRS::DigiPath_FromString(DigiPath& path, const String& value)
{
	size_t pathSize = 0;

	for (auto& pathChunk : value.Split(','))
	{
		if (pathSize == path.GetCapacity())
		{

			return False;
		}

		if (!Packet::Validate_DigiPath(pathChunk))
		{

			return False;
		}

		path[pathSize++] = Move(pathChunk);
	}

	return True;
}
