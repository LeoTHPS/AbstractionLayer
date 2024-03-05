#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#undef DELETE
#endif

namespace AL::Serialization::HTTP
{
	enum class RequestMethods : uint8
	{
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		PATCH
	};

	static String RequestMethods_ToString(RequestMethods value)
	{
		switch (value)
		{
			case RequestMethods::GET:     return "GET";
			case RequestMethods::HEAD:    return "HEAD";
			case RequestMethods::POST:    return "POST";
			case RequestMethods::PUT:     return "PUT";
			case RequestMethods::DELETE:  return "DELETE";
			case RequestMethods::CONNECT: return "CONNECT";
			case RequestMethods::OPTIONS: return "OPTIONS";
			case RequestMethods::TRACE:   return "TRACE";
			case RequestMethods::PATCH:   return "PATCH";
		}

		throw OperationNotSupportedException();
	}

	static RequestMethods RequestMethods_FromString(const String& value)
	{
		if (value.Compare("GET", True))          return RequestMethods::GET;
		else if (value.Compare("HEAD", True))    return RequestMethods::HEAD;
		else if (value.Compare("POST", True))    return RequestMethods::POST;
		else if (value.Compare("PUT", True))     return RequestMethods::PUT;
		else if (value.Compare("DELETE", True))  return RequestMethods::DELETE;
		else if (value.Compare("CONNECT", True)) return RequestMethods::CONNECT;
		else if (value.Compare("OPTIONS", True)) return RequestMethods::OPTIONS;
		else if (value.Compare("TRACE", True))   return RequestMethods::TRACE;
		else if (value.Compare("PATCH", True))   return RequestMethods::PATCH;

		throw OperationNotSupportedException();
	}
}

AL_DEFINE_TO_STRING(
	AL::Serialization::HTTP::RequestMethods,
	(AL::Serialization::HTTP::RequestMethods value)
	{
		return AL::Serialization::HTTP::RequestMethods_ToString(value);
	}
);
AL_DEFINE_FROM_STRING(
	AL::Serialization::HTTP::RequestMethods,
	(const AL::String& string)
	{
		return AL::Serialization::HTTP::RequestMethods_FromString(string);
	}
);
