#pragma once
#include "AL/Common.hpp"

#include "Direct3D10.hpp"
#include "Direct3D11.hpp"
#include "Direct3D12.hpp"

namespace AL::DirectX
{
#if false && defined(AL_DEPENDENCY_DIRECT3D_12_VERSION)
	typedef Direct3D12 Direct3D;
#elif defined(AL_DEPENDENCY_DIRECT3D_11_VERSION)
	typedef Direct3D11 Direct3D;
#elif false && defined(AL_DEPENDENCY_DIRECT3D_10_VERSION)
	typedef Direct3D10 Direct3D;
#endif
}
