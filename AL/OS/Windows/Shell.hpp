#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <shellapi.h>

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "Shell32.lib")
#endif

namespace AL::OS::Windows
{
	class Shell
	{
		Shell() = delete;

	public:
		// @throw AL::Exception
		static Void Execute(const String& file, const String& args)
		{
			::HINSTANCE hInstance;

			if ((hInstance = ::ShellExecuteA(NULL, "open", file.GetCString(), args.GetCString(), nullptr, SW_SHOW)) <= reinterpret_cast<::HINSTANCE>(32))
			{

				throw SystemException(
					"ShellExecuteA"
				);
			}
		}
		// @throw AL::Exception
		static Void Execute(const String& file, const String& args, const String& directory)
		{
			::HINSTANCE hInstance;

			if ((hInstance = ::ShellExecuteA(NULL, "open", file.GetCString(), args.GetCString(), directory.GetCString(), SW_SHOW)) <= reinterpret_cast<::HINSTANCE>(32))
			{

				throw SystemException(
					"ShellExecuteA"
				);
			}
		}
	};
}
