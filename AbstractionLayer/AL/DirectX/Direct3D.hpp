#pragma once
#include "AL/Common.hpp"

#if defined(AL_DIRECT3D_10)
	#include "Direct3D10.hpp"
#elif defined(AL_DIRECT3D_11)
	#include "Direct3D11.hpp"
#elif defined(AL_DIRECT3D_12)
	#include "Direct3D12.hpp"
#else
	#include "Direct3D11.hpp"
#endif

namespace AL::DirectX
{
#if defined(AL_DIRECT3D_10)
	typedef Direct3D10 Direct3D;
#elif defined(AL_DIRECT3D_11)
	typedef Direct3D11 Direct3D;
#elif defined(AL_DIRECT3D_12)
	typedef Direct3D12 Direct3D;
#else
	typedef Direct3D11 Direct3D;
#endif
}
