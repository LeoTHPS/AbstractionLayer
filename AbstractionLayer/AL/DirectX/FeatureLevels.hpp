#pragma once
#include "AL/Common.hpp"

#include <d3dcommon.h>

namespace AL::DirectX
{
	// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro
	enum class FeatureLevels
	{
		DX9_1  = D3D_FEATURE_LEVEL_9_1,
		DX9_2  = D3D_FEATURE_LEVEL_9_2,
		DX9_3  = D3D_FEATURE_LEVEL_9_3,

		DX10   = D3D_FEATURE_LEVEL_10_0,
		DX10_1 = D3D_FEATURE_LEVEL_10_1,

		DX11   = D3D_FEATURE_LEVEL_11_0,
		DX11_1 = D3D_FEATURE_LEVEL_11_1,

		DX12   = D3D_FEATURE_LEVEL_12_0,
		DX12_1 = D3D_FEATURE_LEVEL_12_1
	};
}
