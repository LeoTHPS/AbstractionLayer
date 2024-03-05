#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

namespace AL::Serialization::APRS
{
	class Weather
	{
		struct Wind
		{
			uint16 Speed;
			uint16 SpeedGust;
			uint16 Direction;
		};

		struct Rainfall
		{
			uint16 LastHour;
			uint16 Last24Hours;
			uint16 SinceMidnight;
		};

		Packet   packet;

		DateTime time;
		Wind     wind;
		Rainfall rainfall;
		uint8    humidity;
		int16    temperature;
		uint32   barometricPressure;

		Weather(Packet&& packet, DateTime&& time, Wind&& wind, Rainfall&& rainfall, uint8 humidity, int16 temperature, uint32 barometricPressure)
			: packet(
				Move(packet)
			),
			time(
				Move(time)
			),
			wind(
				Move(wind)
			),
			rainfall(
				Move(rainfall)
			),
			humidity(
				humidity
			),
			temperature(
				temperature
			),
			barometricPressure(
				barometricPressure
			)
		{
		}

	public:
		// @throw AL::Exception
		static Weather Create(String&& sender, String&& tocall, DigiPath&& path, DateTime&& time, uint16 windSpeed, uint16 windSpeedGust, uint16 windDirection, uint16 rainfallLastHour, uint16 rainfallLast24Hours, uint16 rainfallSinceMidnight, uint8 humidity, int16 temperature, uint32 barometricPressure)
		{
			auto packet = Packet::Create(Move(sender), Move(tocall), Move(path), "");

			return Weather(Move(packet), Move(time), { .Speed = windSpeed, .SpeedGust = windSpeedGust, .Direction = windDirection }, { .LastHour = rainfallLastHour, .Last24Hours = rainfallLast24Hours, .SinceMidnight = rainfallSinceMidnight }, humidity, temperature, barometricPressure);
		}
		// @throw AL::Exception
		static Weather Create(const String& sender, const String& tocall, const DigiPath& path, const DateTime& time, uint16 windSpeed, uint16 windSpeedGust, uint16 windDirection, uint16 rainfallLastHour, uint16 rainfallLast24Hours, uint16 rainfallSinceMidnight, uint8 humidity, int16 temperature, uint32 barometricPressure)
		{
			auto packet = Packet::Create(String(sender), String(tocall), DigiPath(path), "");

			return Weather(Move(packet), DateTime(time), { .Speed = windSpeed, .SpeedGust = windSpeedGust, .Direction = windDirection }, { .LastHour = rainfallLastHour, .Last24Hours = rainfallLast24Hours, .SinceMidnight = rainfallSinceMidnight }, humidity, temperature, barometricPressure);
		}

		// @throw AL::Exception
		static Weather CreateIS(String&& sender, String&& tocall, DigiPath&& path, String&& igate, QConstructs qConstruct, DateTime&& time, uint16 windSpeed, uint16 windSpeedGust, uint16 windDirection, uint16 rainfallLastHour, uint16 rainfallLast24Hours, uint16 rainfallSinceMidnight, uint8 humidity, int16 temperature, uint32 barometricPressure)
		{
			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Weather(Move(packet), Move(time), { .Speed = windSpeed, .SpeedGust = windSpeedGust, .Direction = windDirection }, { .LastHour = rainfallLastHour, .Last24Hours = rainfallLast24Hours, .SinceMidnight = rainfallSinceMidnight }, humidity, temperature, barometricPressure);
		}
		// @throw AL::Exception
		static Weather CreateIS(const String& sender, const String& tocall, const DigiPath& path, const String& igate, QConstructs qConstruct, const DateTime& time, uint16 windSpeed, uint16 windSpeedGust, uint16 windDirection, uint16 rainfallLastHour, uint16 rainfallLast24Hours, uint16 rainfallSinceMidnight, uint8 humidity, int16 temperature, uint32 barometricPressure)
		{
			auto packet = Packet::CreateIS(Move(sender), Move(tocall), Move(path), Move(igate), qConstruct, "");

			return Weather(Move(packet), DateTime(time), { .Speed = windSpeed, .SpeedGust = windSpeedGust, .Direction = windDirection }, { .LastHour = rainfallLastHour, .Last24Hours = rainfallLast24Hours, .SinceMidnight = rainfallSinceMidnight }, humidity, temperature, barometricPressure);
		}

		// @throw AL::Exception
		static Weather FromPacket(Packet&& value);
		// @throw AL::Exception
		static Weather FromPacket(const Packet& value)
		{
			return FromPacket(Packet(value));
		}

		// @throw AL::Exception
		static Weather FromString(const String& value)
		{
			auto packet = Packet::FromString(
				value
			);

			return FromPacket(
				packet
			);
		}

		Weather()
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

		auto& GetTime() const
		{
			return time;
		}

		auto& GetToCall() const
		{
			return packet.GetToCall();
		}

		auto& GetSender() const
		{
			return packet.GetSender();
		}

		auto& GetQConstruct() const
		{
			return packet.GetQConstruct();
		}

		auto GetHumidity() const
		{
			return humidity;
		}

		auto GetTemperature() const
		{
			return temperature;
		}

		auto GetWindSpeed() const
		{
			return wind.Speed;
		}

		auto GetWindSpeedGust() const
		{
			return wind.SpeedGust;
		}

		auto GetWindDirection() const
		{
			return wind.Direction;
		}

		auto GetRainfallLastHour() const
		{
			return rainfall.LastHour;
		}

		auto GetRainfallLast24Hours() const
		{
			return rainfall.Last24Hours;
		}

		auto GetRainfallSinceMidnight() const
		{
			return rainfall.SinceMidnight;
		}

		auto GetBarometricPressure() const
		{
			return barometricPressure;
		}

		Bool SetPath(DigiPath&& value)
		{
			return packet.SetPath(Move(value));
		}
		Bool SetPath(const DigiPath& value)
		{
			return packet.SetPath(value);
		}

		Void SetTime(DateTime&& value)
		{
			time = Move(value);
		}
		Void SetTime(const DateTime& value)
		{
			SetTime(
				DateTime(value)
			);
		}

		Void SetWind(uint16 speed, uint16 speedGust, uint16 direction)
		{
			wind.Speed     = speed;
			wind.SpeedGust = speedGust;
			wind.Direction = direction;
		}

		Bool SetSender(String&& value)
		{
			return packet.SetSender(Move(value));
		}
		Bool SetSender(const String& value)
		{
			return packet.SetSender(value);
		}

		Packet ToPacket(Bool include_is_fields) const
		{
			auto& time = GetTime();

			StringBuilder sb;
			sb << '_';
			sb << String::Format("%02u%02u%02u%02u", time.Month, time.Day, time.Hour, time.Minutes);
			sb << String::Format("c%03us%03ug%03u", GetWindDirection(), GetWindSpeed(), GetWindSpeedGust());
			sb << String::Format("t%03i", GetTemperature());
			sb << String::Format("r%03up%03uP%03u", GetRainfallLastHour(), GetRainfallLast24Hours(), GetRainfallSinceMidnight());
			sb << String::Format("h%02u", GetHumidity());
			sb << String::Format("b%04u", GetBarometricPressure());
			sb << "AL";

			return !include_is_fields ? Packet::Create(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), sb.ToString()) :
										Packet::CreateIS(String(GetSender()), String(GetToCall()), DigiPath(GetPath()), String(GetIGate()), GetQConstruct(), sb.ToString());
		}

		String ToString(Bool include_is_fields) const
		{
			return ToPacket(include_is_fields).ToString(include_is_fields);
		}
	};
}
