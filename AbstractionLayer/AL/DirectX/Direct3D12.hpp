#pragma once
#include "AL/Common.hpp"

#include "IResource.hpp"

#if __has_include(<d3d12.h>)
	#include <d3d12.h>

	#define AL_DEPENDENCY_DIRECT3D_12

	#define AL_DEPENDENCY_DIRECT3D_12_VERSION 12.0
#endif

namespace AL::DirectX
{
	class Direct3D12;
}
