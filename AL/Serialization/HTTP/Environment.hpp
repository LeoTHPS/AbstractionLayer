#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Dictionary.hpp"

namespace AL::Serialization::HTTP
{
	typedef Collections::Dictionary<String, String> Environment;

	constexpr const String::Char HTTP_HOST[]                      = "HTTP_HOST";
	constexpr const String::Char HTTP_CONNECTION[]                = "HTTP_CONNECTION";
	constexpr const String::Char HTTP_CACHE_CONTROL[]             = "HTTP_CACHE_CONTROL";
	constexpr const String::Char HTTP_DNT[]                       = "HTTP_DNT";
	constexpr const String::Char HTTP_UPGRADE_INSECURE_REQUESTS[] = "HTTP_UPGRADE_INSECURE_REQUESTS";
	constexpr const String::Char HTTP_USER_AGENT[]                = "HTTP_USER_AGENT";
	constexpr const String::Char HTTP_ACCEPT[]                    = "HTTP_ACCEPT";
	constexpr const String::Char HTTP_ACCEPT_ENCODING[]           = "HTTP_ACCEPT_ENCODING";
	constexpr const String::Char HTTP_ACCEPT_LANGUAGE[]           = "HTTP_ACCEPT_LANGUAGE";
	constexpr const String::Char HTTP_COOKIE[]                    = "HTTP_COOKIE";
	constexpr const String::Char PATH[]                           = "PATH";
	constexpr const String::Char SERVER_SIGNATURE[]               = "SERVER_SIGNATURE";
	constexpr const String::Char SERVER_SOFTWARE[]                = "SERVER_SOFTWARE";
	constexpr const String::Char SERVER_NAME[]                    = "SERVER_NAME";
	constexpr const String::Char SERVER_ADDR[]                    = "SERVER_ADDR";
	constexpr const String::Char SERVER_PORT[]                    = "SERVER_PORT";
	constexpr const String::Char REMOTE_ADDR[]                    = "REMOTE_ADDR";
	constexpr const String::Char DOCUMENT_ROOT[]                  = "DOCUMENT_ROOT";
	constexpr const String::Char REQUEST_SCHEME[]                 = "REQUEST_SCHEME";
	constexpr const String::Char CONTEXT_PREFIX[]                 = "CONTEXT_PREFIX";
	constexpr const String::Char CONTEXT_DOCUMENT_ROOT[]          = "CONTEXT_DOCUMENT_ROOT";
	constexpr const String::Char SERVER_ADMIN[]                   = "SERVER_ADMIN";
	constexpr const String::Char SCRIPT_FILENAME[]                = "SCRIPT_FILENAME";
	constexpr const String::Char REMOTE_PORT[]                    = "REMOTE_PORT";
	constexpr const String::Char GATEWAY_INTERFACE[]              = "GATEWAY_INTERFACE";
	constexpr const String::Char SERVER_PROTOCOL[]                = "SERVER_PROTOCOL";
	constexpr const String::Char REQUEST_METHOD[]                 = "REQUEST_METHOD";
	constexpr const String::Char QUERY_STRING[]                   = "QUERY_STRING";
	constexpr const String::Char REQUEST_URI[]                    = "REQUEST_URI";
	constexpr const String::Char SCRIPT_NAME[]                    = "SCRIPT_NAME";
}
