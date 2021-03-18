#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename KEY, typename VALUE>
	struct KeyValuePair
	{
		KEY   Key;
		VALUE Value;
	};
}
