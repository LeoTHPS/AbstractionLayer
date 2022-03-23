#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

namespace AL::OS::OpenGL
{
	class OpenGLException
		: public OS::SystemException
	{
	public:
		using SystemException::SystemException;
	};
}
