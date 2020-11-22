#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#if __has_include(<d3d10_1.h>)
	#include <d3d10_1.h>

	#define AL_DEPENDENCY_DIRECT3D_10_1

	#define AL_DEPENDENCY_DIRECT3D_10_VERSION 10.1
#elif __has_include(<d3d10.h>)
	#include <d3d10.h>

	#define AL_DEPENDENCY_DIRECT3D_10

	#define AL_DEPENDENCY_DIRECT3D_10_VERSION 10.0
#endif

namespace AL::DirectX
{
	class Direct3D10;
}
