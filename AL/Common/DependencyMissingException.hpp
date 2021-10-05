#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class DependencyMissingException
		: public Exception
	{
	public:
		explicit DependencyMissingException(const char* name)
			: Exception(
				"Missing dependency: %s",
				name
			)
		{
		}
		explicit DependencyMissingException(const String& name)
			: DependencyMissingException(
				name.GetCString()
			)
		{
		}
	};
}
