#pragma once
#include "AL/Common.hpp"

#include "Exception.hpp"

namespace AL::Exceptions
{
	class DependencyMissingException
		: public Exception
	{
		String dependency;

	public:
		explicit DependencyMissingException(String&& dependency)
			: Exception(
				"Missing dependency '%s'",
				dependency.GetCString()
			),
			dependency(
				Move(dependency)
			)
		{
		}

		auto& GetDependency() const
		{
			return dependency;
		}
	};
}
