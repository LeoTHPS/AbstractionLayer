#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

namespace AL::Serialization::APRS
{
	class Message
	{
		Packet packet;

		String id;
		String content;
		String destination;

		Message(Packet&& packet, String&& destination, String&& content, String&& id)
			: packet(
				Move(packet)
			),
			id(
				Move(id)
			),
			content(
				Move(content)
			),
			destination(
				Move(destination)
			)
		{
		}

	public:
		// @throw AL::Exception
		static Message Create(String&& sender, String&& tocall, DigiPath&& path, String&& destination, String&& content, String&& id = "")
		{
			if (!Packet::Validate_StationName(destination))
				throw Exception("Invalid destination");

			if (!Validate_Content(content))
				throw Exception("Invalid content");

			if ((id.GetLength() != 0) && !Validate_Id(id))
				throw Exception("Invalid id");

			auto packet = Packet::Create(Move(sender), Move(tocall), Move(path), "");

			return Message(Move(packet), Move(destination), Move(content), Move(id));
		}
		// @throw AL::Exception
		static Message Create(const String& sender, const String& tocall, const DigiPath& path, const String& destination, const String& content, const String& id = "")
		{
			if (!Packet::Validate_StationName(destination))
				throw Exception("Invalid destination");

			if (!Validate_Content(content))
				throw Exception("Invalid content");

			if ((id.GetLength() != 0) && !Validate_Id(id))
				throw Exception("Invalid id");

			auto packet = Packet::Create(String(sender), String(tocall), DigiPath(path), "");

			return Message(Move(packet), String(destination), String(content), String(id));
		}

		// @throw AL::Exception
		static Message CreateIS(String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, String&& destination, String&& content, String&& id = "")
		{
			if (!Packet::Validate_StationName(destination, False))
				throw Exception("Invalid destination");

			if (!Validate_Content(content, False))
				throw Exception("Invalid content");

			if ((id.GetLength() != 0) && !Validate_Id(id))
				throw Exception("Invalid id");

			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Message(Move(packet), Move(destination), Move(content), Move(id));
		}
		// @throw AL::Exception
		static Message CreateIS(const String& sender, const String& tocall, const DigiPath& path, const String& igate, QConstructs qConstruct, const String& destination, const String& content, const String& id = "")
		{
			if (!Packet::Validate_StationName(destination, False))
				throw Exception("Invalid destination");

			if (!Validate_Content(content, False))
				throw Exception("Invalid content");

			if ((id.GetLength() != 0) && !Validate_Id(id))
				throw Exception("Invalid id");

			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Message(Move(packet), String(destination), String(content), String(id));
		}

		// @throw AL::Exception
		static Message FromPacket(Packet&& value)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^:([^ :]+)\\s*:([^\\{]*)(\\{(\\S{1,5}))?$", value.GetContent()))
				throw Exception("Invalid message format");

			if (!Packet::Validate_StationName(matches[1], False))
				throw Exception("Invalid destination");

			if ((matches[4].GetLength() != 0) && !Validate_Id(matches[4]))
				throw Exception("Invalid id");

			if (!Validate_Content(matches[2], False))
				throw Exception("Invalid content");

			return Message(Packet(value), Move(matches[1]), Move(matches[2]), (matches[4].GetLength() != 0) ? Move(matches[4]) : "");
		}
		// @throw AL::Exception
		static Message FromPacket(const Packet& value)
		{
			return FromPacket(Packet(value));
		}

		// @throw AL::Exception
		static Message FromString(const String& value)
		{
			auto packet = Packet::FromString(
				value
			);

			return FromPacket(
				packet
			);
		}

		Message()
		{
		}

		auto& GetId() const
		{
			return id;
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

		auto& GetContent() const
		{
			return content;
		}

		auto& GetQConstruct() const
		{
			return packet.GetQConstruct();
		}

		auto& GetDestination() const
		{
			return destination;
		}

		Bool SetId(String&& value)
		{
			if (!Validate_Id(value))
			{

				return False;
			}

			id = Move(
				value
			);

			return True;
		}
		Bool SetId(const String& value)
		{
			return SetId(String(value));
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

		Bool SetContent(String&& value)
		{
			if (!Validate_Content(value))
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

		Bool SetDestination(String&& value)
		{
			if (!Packet::Validate_StationName(value))
			{

				return False;
			}

			destination = Move(
				value
			);

			return True;
		}
		Bool SetDestination(const String& value)
		{
			return SetDestination(String(value));
		}

		Packet ToPacket(Bool include_is_fields) const
		{
			StringBuilder sb;
			sb << ':' << GetDestination();

			for (size_t i = GetDestination().GetLength(); i < 9; ++i)
				sb << ' ';

			sb << ':' << GetContent();

			if (GetId().GetLength() != 0)
				sb << '{' << GetId();

			return !include_is_fields ? Packet::Create(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), sb.ToString()) :
										Packet::CreateIS(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), String(GetIGate()), GetQConstruct(), sb.ToString());
		}

		String ToString(Bool include_is_fields) const
		{
			return ToPacket(include_is_fields).ToString(include_is_fields);
		}

	private:
		static Bool Validate_Id(const String& value)
		{
			return Regex::IsMatch("^\\S{1,5}$", value);
		}
 
		static Bool Validate_Content(const String& value, Bool strict = True)
		{
			return Math::IsInRange<size_t>(value.GetLength(), 0, strict ? 67 : 255);
		}
	};
}
