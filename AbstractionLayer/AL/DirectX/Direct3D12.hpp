#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#if __has_include(<d3d12.h>)
	#include <d3d12.h>

	#define AL_DEPENDENCY_DIRECT3D_12

	#define AL_DEPENDENCY_DIRECT3D_12_VERSION 12.0
#endif

namespace AL::DirectX
{
	class Direct3D12
	{
		Direct3D12(Direct3D12&&) = delete;
		Direct3D12(const Direct3D12&) = delete;

	public:
		Direct3D12()
		{
		}

		virtual ~Direct3D12()
		{
		}
	};
}
