#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

namespace AL::OpenGL
{
	class OpenGLException
		: public OS::SystemException
	{
	public:
		using SystemException::SystemException;
	};
}
