#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename TKey, typename TValue>
	struct KeyValuePair
	{
		TKey Key;
		TValue Value;
	};
}
