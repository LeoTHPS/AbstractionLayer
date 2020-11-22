#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

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
	class Direct3D10
	{
		Direct3D10(Direct3D10&&) = delete;
		Direct3D10(const Direct3D10&) = delete;

	public:
		Direct3D10()
		{
		}

		virtual ~Direct3D10()
		{
		}
	};
}
