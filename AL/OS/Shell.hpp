#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Shell.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Shell.hpp"
#else
	#error Platform not supported
#endif

namespace AL::OS
{
	class Shell
	{
		Shell() = delete;

	public:
		// @throw AL::Exception
		static Void Execute(const String& file, const String& args)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Shell::Execute(
				AL::String::Format(
					"%s %s",
					file.GetCString(),
					args.GetCString()
				)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Shell::Execute(
				file,
				args
			);
#endif
		}
	};
}
