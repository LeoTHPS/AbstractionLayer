#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#if __has_include(<GL/gl.h>)
		#define AL_DEPENDENCY_OPENGL

		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#if __has_include(<gl/GL.h>)
		#define AL_DEPENDENCY_OPENGL

		#include <gl/GL.h>
		#include <gl/GLU.h>
	#endif
#endif

namespace AL::OpenGL
{
	// TODO: implement
	class Context;
}
