#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

namespace AL::Serialization::APRS
{
	class Telemetry
	{
		Packet packet;

		uint8  analog[5];
		uint8  digital;
		uint16 sequence;

		Telemetry(Packet&& packet, const uint8(&analog)[5], uint8 digital, uint16 sequence)
			: packet(
				Move(packet)
			),
			analog{
				analog[0],
				analog[1],
				analog[2],
				analog[3],
				analog[4]
			},
			digital(
				digital
			),
			sequence(
				sequence
			)
		{
		}

	public:
		// @throw AL::Exception
		static Telemetry Create(String&& sender, String&& tocall, DigiPath&& path, const uint8(&analog)[5], const Bool(&digital)[8], uint16 sequence)
		{
			if (!Validate_Sequence(sequence))
				throw Exception("Invalid sequence");

			auto packet = Packet::Create(Move(sender), Move(tocall), Move(path), "");

			return Telemetry(Move(packet), analog, (digital[7] << 7) | (digital[6] << 6) | (digital[5] << 5) | (digital[4] << 4) | (digital[3] << 3) | (digital[2] << 2) | (digital[1] << 1) | digital[0], sequence);
		}
		// @throw AL::Exception
		static Telemetry Create(const String& sender, const String& tocall, const DigiPath& path, const uint8(&analog)[5], const Bool(&digital)[8], uint16 sequence)
		{
			return Create(String(sender), String(tocall), DigiPath(path), analog, digital, sequence);
		}

		// @throw AL::Exception
		static Telemetry CreateIS(String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, const uint8(&analog)[5], const Bool(&digital)[8], uint16 sequence)
		{
			if (!Validate_Sequence(sequence))
				throw Exception("Invalid sequence");

			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Telemetry(Move(packet), analog, (digital[7] << 7) | (digital[6] << 6) | (digital[5] << 5) | (digital[4] << 4) | (digital[3] << 3) | (digital[2] << 2) | (digital[1] << 1) | digital[0], sequence);
		}
		// @throw AL::Exception
		static Telemetry CreateIS(const String& sender, const String& tocall, const DigiPath& path, const String& igate, QConstructs qConstruct, const uint8(&analog)[5], const Bool(&digital)[8], uint16 sequence)
		{
			return CreateIS(String(sender), String(tocall), DigiPath(path), String(igate), qConstruct, analog, digital, sequence);
		}

		// @throw AL::Exception
		static Telemetry FromPacket(Packet&& value)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^T#(\\d{3})(,(\\d{3}))?(,(\\d{3}))?(,(\\d{3}))?(,(\\d{3}))?(,(\\d{3}))?(,(\\d{8}))?.*?$", value.GetContent()))
				throw Exception("Invalid message format");

			auto sequence = AL::FromString<uint16>(matches[1]);

			if (!Validate_Sequence(sequence))
				throw Exception("Invalid sequence");

			uint8 analog[5] =
			{
				AL::FromString<uint8>(matches[3]),
				AL::FromString<uint8>(matches[5]),
				AL::FromString<uint8>(matches[7]),
				AL::FromString<uint8>(matches[9]),
				AL::FromString<uint8>(matches[11])
			};
			uint8 digital = AL::FromString<uint8>(matches[13]);

			return Telemetry(Move(value), analog, digital, sequence);
		}
		// @throw AL::Exception
		static Telemetry FromPacket(const Packet& value)
		{
			return FromPacket(Packet(value));
		}

		// @throw AL::Exception
		static Telemetry FromString(const String& value)
		{
			auto packet = Packet::FromString(
				value
			);

			return FromPacket(
				packet
			);
		}

		Telemetry()
		{
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

		auto GetSequence() const
		{
			return sequence;
		}

		auto& GetQConstruct() const
		{
			return packet.GetQConstruct();
		}

		Void GetValues(uint8(&analog)[5], uint8& digital) const
		{
			analog[0] = this->analog[0];
			analog[1] = this->analog[1];
			analog[2] = this->analog[2];
			analog[3] = this->analog[3];
			analog[4] = this->analog[4];
			digital   = this->digital;
		}
		Void GetValues(uint8(&analog)[5], Bool(&digital)[8]) const
		{
			uint8 value;
			GetValues(analog, value);
			digital[0] = (value & 0b00000001) == 0b00000001;
			digital[1] = (value & 0b00000010) == 0b00000010;
			digital[2] = (value & 0b00000100) == 0b00000100;
			digital[3] = (value & 0b00001000) == 0b00001000;
			digital[4] = (value & 0b00010000) == 0b00010000;
			digital[5] = (value & 0b00100000) == 0b00100000;
			digital[6] = (value & 0b01000000) == 0b01000000;
			digital[7] = (value & 0b10000000) == 0b10000000;
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

		Void SetValues(const uint8(&analog)[5], uint8 digital)
		{
			this->analog[0] = analog[0];
			this->analog[1] = analog[1];
			this->analog[2] = analog[2];
			this->analog[3] = analog[3];
			this->analog[4] = analog[4];
			this->digital   = digital;
		}
		Void SetValues(const uint8(&analog)[5], const Bool(&digital)[8])
		{
			SetValues(analog, (digital[7] << 7) | (digital[6] << 6) | (digital[5] << 5) | (digital[4] << 4) | (digital[3] << 3) | (digital[2] << 2) | (digital[1] << 1) | digital[0]);
		}

		Bool SetSequence(uint16 value)
		{
			if (!Validate_Sequence(value))
			{

				return False;
			}

			sequence = value;

			return True;
		}

		Packet ToPacket(Bool include_is_fields) const
		{
			StringBuilder sb;
			sb << 'T';
			sb << '#' << String::Format("%03u", GetSequence());
			sb << ',' << String::Format("%03u", analog[0]);
			sb << ',' << String::Format("%03u", analog[1]);
			sb << ',' << String::Format("%03u", analog[2]);
			sb << ',' << String::Format("%03u", analog[3]);
			sb << ',' << String::Format("%03u", analog[4]);
			sb << ','; for (uint8 i = 0; i < 8; ++i) sb << (((digital & (1 << i)) == (1 << i)) ? 1 : 0);

			return !include_is_fields ? Packet::Create(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), sb.ToString()) :
										Packet::CreateIS(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), String(GetIGate()), GetQConstruct(), sb.ToString());
		}

		String ToString(Bool include_is_fields) const
		{
			return ToPacket(include_is_fields).ToString(include_is_fields);
		}

	private:
		static Bool Validate_Sequence(uint16 value)
		{
			return Math::IsInRange<uint16>(value, 0, 999);
		}
	};
}
