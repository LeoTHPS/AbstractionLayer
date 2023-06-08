#pragma once
#include "AL/Common.hpp"

#if !AL_HAS_INCLUDE(<openssl/ssl.h>)
	#error Missing openssl/ssl.h
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace AL::OpenSSL
{
	class SSLException
		: public Exception
	{
	public:
		template<size_t S>
		explicit SSLException(const char(&function)[S])
			: Exception(
				[&function]()
				{
					Exception exception(
						"Error calling '%s'",
						function
					);

					for (size_t error; (error = ::ERR_get_error()); )
					{
						exception = Exception(
							Move(exception),
							Exception(::ERR_error_string(error, 0))
						);
					}

					return exception;
				}()
			)
		{
		}
	};
}
