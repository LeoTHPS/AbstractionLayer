#pragma once
#include "AL/Common.hpp"

#if __has_include(<gl/GL.h>)
	#include <gl/GL.h>
	#include <gl/GLU.h>

	#define AL_DEPENDENCY_OPENGL
#endif

namespace AL::OpenGL
{
	class Device;
}
