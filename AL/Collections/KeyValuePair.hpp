#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename T_KEY, typename T_VALUE>
	struct KeyValuePair
	{
		T_KEY   Key;
		T_VALUE Value;
	};
}
