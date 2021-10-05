#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <GL/gl.h>
	#include <GL/glu.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <gl/GL.h>
	#include <gl/GLU.h>
#endif

namespace AL::OpenGL
{
	class Context;
}
