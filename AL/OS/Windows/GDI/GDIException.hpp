#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

namespace AL::OS::Windows::GDI
{
	class GDIException
		: public Exception
	{
	public:
		template<size_t S>
		explicit GDIException(const char(&function)[S])
			: Exception(
				"Error calling '%s'",
				&function[0]
			)
		{
		}
	};
}
