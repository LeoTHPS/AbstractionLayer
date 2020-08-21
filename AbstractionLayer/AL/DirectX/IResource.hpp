#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::DirectX
{
	template<typename T>
	class IResource;
}
