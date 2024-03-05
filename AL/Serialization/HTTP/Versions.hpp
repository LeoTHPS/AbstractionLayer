#pragma once
#include "AL/Common.hpp"

namespace AL::Serialization::HTTP
{
	enum class Versions : uint8
	{
		HTTP_1_0,
		HTTP_1_1
	};

	static String Versions_ToString(Versions value)
	{
		switch (value)
		{
			case Versions::HTTP_1_0: return "HTTP/1.0";
			case Versions::HTTP_1_1: return "HTTP/1.1";
		}

		throw OperationNotSupportedException();
	}

	static Versions Versions_FromString(const String& value)
	{
		if (value.Compare("HTTP/1.0", True))      return Versions::HTTP_1_0;
		else if (value.Compare("HTTP/1.1", True)) return Versions::HTTP_1_1;

		throw OperationNotSupportedException();
	}
}

AL_DEFINE_TO_STRING(
	AL::Serialization::HTTP::Versions,
	(AL::Serialization::HTTP::Versions value)
	{
		return AL::Serialization::HTTP::Versions_ToString(value);
	}
);
AL_DEFINE_FROM_STRING(
	AL::Serialization::HTTP::Versions,
	(const AL::String& string)
	{
		return AL::Serialization::HTTP::Versions_FromString(string);
	}
);
