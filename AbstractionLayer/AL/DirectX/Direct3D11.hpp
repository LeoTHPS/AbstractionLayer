#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#if __has_include(<d3d11_4.h>)
	#include <d3d11_4.h>

	#define AL_DEPENDENCY_DIRECT3D_11_4

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.4
#elif __has_include(<d3d11_3.h>)
	#include <d3d11_3.h>

	#define AL_DEPENDENCY_DIRECT3D_11_3

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.3
#elif __has_include(<d3d11_2.h>)
	#include <d3d11_2.h>

	#define AL_DEPENDENCY_DIRECT3D_11_2

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.2
#elif __has_include(<d3d11_1.h>)
	#include <d3d11_1.h>

	#define AL_DEPENDENCY_DIRECT3D_11_1

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.1
#elif __has_include(<d3d11.h>)
	#include <d3d11.h>

	#define AL_DEPENDENCY_DIRECT3D_11

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.0
#endif

namespace AL::DirectX
{
	class Direct3D11;
}
