#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "Console.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#include <cstdio>
#endif

namespace AL::OS
{
	class Debug
	{
		inline static Mutex mutex;

		Debug() = delete;

	public:
		static constexpr Bool IsDebug()
		{
#if defined(AL_DEBUG)
			return True;
#else
			return False;
#endif
		}

		static Void Write(String::Char value)
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::Write(
				value
			);
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				"%c",
				value
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			auto string = String::Format(
				"%c",
				value
			);

			::OutputDebugStringA(
				string.GetCString()
			);
	#endif
#endif
		}
		template<typename ... TArgs>
		static Void Write(const String& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::Write(
				format,
				Forward<TArgs>(args) ...
			);
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				format.GetCString(),
				args ...
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			::OutputDebugStringA(
				string.GetCString()
			);
	#endif
#endif
		}
		template<typename ... TArgs>
		static Void Write(const String::Char* format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::Write(
				format,
				Forward<TArgs>(args) ...
			);
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				format,
				args ...
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			::OutputDebugStringA(
				string.GetCString()
			);
	#endif
#endif
		}

		static Void WriteLine()
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::WriteLine();
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				"\n"
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			::OutputDebugStringA(
				"\n"
			);
	#endif
#endif
		}
		template<typename ... TArgs>
		static Void WriteLine(const String& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::WriteLine(
				format,
				Forward<TArgs>(args) ...
			);
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				format.GetCString(),
				args ...
			);

			::std::fprintf(
				stderr,
				"\n"
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			::OutputDebugStringA(
				string.GetCString()
			);

			::OutputDebugStringA(
				"\n"
			);
	#endif
#endif
		}
		template<typename ... TArgs>
		static Void WriteLine(const String::Char* format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_PICO)
			Console::WriteLine(
				format,
				Forward<TArgs>(args) ...
			);
	#elif defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				format,
				args ...
			);

			::std::fprintf(
				stderr,
				"\n"
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			::OutputDebugStringA(
				string.GetCString()
			);
			::OutputDebugStringA(
				"\n"
			);
	#endif
#endif
		}

		static Void WriteException(const Exception& exception)
		{
#if defined(AL_DEBUG)
			WriteLine(
				exception.GetMessage()
			);

			if (auto lpInnerException = exception.GetInnerException())
			{
				do
				{
					WriteLine(
						lpInnerException->GetMessage()
					);
				} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
			}
#endif
		}
	};
}
