#pragma once
#include "AL/Common.hpp"

namespace AL::Network::HTTP
{
	enum class StatusCodes : uint16
	{
		// Info (100-199)
		Continue                      = 100,
		SwitchingProtocols            = 101,
		Processing                    = 102,
		EarlyHints                    = 103,

		// Success (200-299)
		OK                            = 200,
		Created                       = 201,
		Accepted                      = 202,
		NonAuthoritativeInformation   = 203,
		NoContent                     = 204,
		ResetContent                  = 205,
		PartialContent                = 206,
		MultiStatus                   = 207,
		AlreadyReported               = 208,
		IMUsed                        = 226,

		// Redirection (300-399)
		MultipleChoice                = 300,
		MovedPermanently              = 301,
		Found                         = 302,
		SeeOther                      = 303,
		NotModified                   = 304,
		UseProxy                      = 305,
		TemporaryRedirect             = 307,
		PermanentRedirect             = 308,

		// Client Error (400-499)
		BadRequest                    = 400,
		Unauthorized                  = 401,
		PaymentRequired               = 402,
		Forbidden                     = 403,
		NotFound                      = 404,
		MethodNotFound                = 405,
		NotAcceptable                 = 406,
		ProxyAuthenticationRequired   = 407,
		RequestTimeout                = 408,
		Conflict                      = 409,
		Gone                          = 410,
		LengthRequired                = 411,
		PreconditionFailed            = 412,
		PayloadTooLarge               = 413,
		UriTooLong                    = 414,
		UnsupportedMediaType          = 415,
		RangeNotSatisfiable           = 416,
		ExpectationFailed             = 417,
		ImATeapot                     = 418,
		MisdirectRequest              = 421,
		UnprocessableEntity           = 422,
		FailedDependency              = 424,
		TooEarly                      = 425,
		UpgradeRequired               = 426,
		PreconditionRequired          = 428,
		TooManyRequests               = 429,
		RequestHeaderFieldsTooLarge   = 431,
		UnavailableForLegalReasons    = 451,

		// Server Error (500-599)
		InternalServerError           = 500,
		NotImplemented                = 501,
		BadGateway                    = 502,
		ServiceUnavailable            = 503,
		GatewayTimeout                = 504,
		HttpVersionNotSupported       = 505,
		VariantAlsoNegotiates         = 506,
		InsufficientStorage           = 507,
		LoopDetected                  = 508,
		NotExtended                   = 510,
		NetworkAuthenticationRequired = 511
	};
}
