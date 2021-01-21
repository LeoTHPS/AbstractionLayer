#pragma once
#include "AL/Common.hpp"

namespace AL::Web::DOM
{
	struct ElementAttribute
	{
		String Name;
		String Value;

		ElementAttribute()
		{
		}

		ElementAttribute(String&& name, String&& value)
			: Name(
				Move(name)
			),
			Value(
				Move(value)
			)
		{
		}
	};
}
