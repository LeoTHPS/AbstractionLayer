#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#undef DELETE
#endif

#define AL_NETWORK_HTTP_REQUEST_METHOD_GET     "GET"
#define AL_NETWORK_HTTP_REQUEST_METHOD_HEAD    "HEAD"
#define AL_NETWORK_HTTP_REQUEST_METHOD_POST    "POST"
#define AL_NETWORK_HTTP_REQUEST_METHOD_PUT     "PUT"
#define AL_NETWORK_HTTP_REQUEST_METHOD_DELETE  "DELETE"
#define AL_NETWORK_HTTP_REQUEST_METHOD_CONNECT "CONNECT"
#define AL_NETWORK_HTTP_REQUEST_METHOD_OPTIONS "OPTIONS"
#define AL_NETWORK_HTTP_REQUEST_METHOD_TRACE   "TRACE"
#define AL_NETWORK_HTTP_REQUEST_METHOD_PATCH   "PATCH"

namespace AL::Network::HTTP
{
	enum class RequestMethods
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
}