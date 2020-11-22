#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>

#if __has_include(<d2d1_3.h>)
	#include <d2d1_3.h>

	#define AL_DEPENDENCY_DIRECT2D_3

	#define AL_DEPENDENCY_DIRECT2D_VERSION 3
#elif __has_include(<d2d1_2.h>)
	#include <d2d1_2.h>

	#define AL_DEPENDENCY_DIRECT2D_2

	#define AL_DEPENDENCY_DIRECT2D_VERSION 2
#elif __has_include(<d2d1_1.h>)
	#include <d2d1_1.h>

	#define AL_DEPENDENCY_DIRECT2D_1

	#define AL_DEPENDENCY_DIRECT2D_VERSION 1
#elif __has_include(<d2d1.h>)
	#include <d2d1.h>

	#define AL_DEPENDENCY_DIRECT2D

	#define AL_DEPENDENCY_DIRECT2D_VERSION 0
#endif

#if __has_include(<dwrite_3.h>)
	#include <dwrite_3.h>

	#define AL_DEPENDENCY_DWRITE_3

	#define AL_DEPENDENCY_DWRITE_VERSION 3
#elif __has_include(<dwrite_2.h>)
	#include <dwrite_2.h>

	#define AL_DEPENDENCY_DWRITE_2

	#define AL_DEPENDENCY_DWRITE_VERSION 2
#elif __has_include(<dwrite_1.h>)
	#include <dwrite_1.h>

	#define AL_DEPENDENCY_DWRITE_1

	#define AL_DEPENDENCY_DWRITE_VERSION 1
#elif __has_include(<dwrite.h>)
	#include <dwrite.h>

	#define AL_DEPENDENCY_DWRITE

	#define AL_DEPENDENCY_DWRITE_VERSION 0
#endif

#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace AL::DirectX
{
	class Direct2D;
}
