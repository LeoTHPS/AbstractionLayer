#pragma once
#include "AL/Common.hpp"

namespace AL
{
	enum class Endians : uint8
	{
		Big,
		Little,

		Machine = ((0xFFFFFFFF & 0x00000001) == 0x00000001) ? Little : Big
	};
}
