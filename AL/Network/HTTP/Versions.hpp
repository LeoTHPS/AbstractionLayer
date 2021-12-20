#pragma once
#include "AL/Common.hpp"

#define AL_NETWORK_HTTP_VERSION_1_0 "HTTP/1.0"
#define AL_NETWORK_HTTP_VERSION_1_1 "HTTP/1.1"

namespace AL::Network::HTTP
{
	enum class Versions
	{
		HTTP_1_0,
		HTTP_1_1
	};
}
