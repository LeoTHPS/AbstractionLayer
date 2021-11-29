#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"

#if defined(AL_PLATFORM_LINUX)
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

		template<typename ... TArgs>
		static Void Write(const String& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			::OutputDebugStringA(
				message.GetCString()
			);
	#endif
#endif
		}
		template<typename ... TArgs>
		static Void Write(const WString& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				"%S",
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			::OutputDebugStringW(
				message.GetCString()
			);
	#endif
#endif
		}

		template<typename ... TArgs>
		static Void WriteLine(const String& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				"%s\n",
				message.GetCString()
			);
#endif
		}
		template<typename ... TArgs>
		static Void WriteLine(const WString& format, TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				L"%s\n",
				message.GetCString()
			);
#endif
		}

		template<size_t S, typename ... TArgs>
		static Void Write(const char(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			::OutputDebugStringA(
				message.GetCString()
			);
	#endif
#endif
		}
		template<size_t S, typename ... TArgs>
		static Void Write(const wchar_t(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

			MutexGuard lock(
				mutex
			);

	#if defined(AL_PLATFORM_LINUX)
			::std::fprintf(
				stderr,
				"%S",
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			::OutputDebugStringW(
				message.GetCString()
			);
	#endif
#endif
		}

		static Void WriteLine()
		{
#if defined(AL_DEBUG)
			Write(
				"\n"
			);
#endif
		}

		template<size_t S, typename ... TArgs>
		static Void WriteLine(const char(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				"%s\n",
				message.GetCString()
			);
#endif
		}
		template<size_t S, typename ... TArgs>
		static Void WriteLine(const wchar_t(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				L"%s\n",
				message.GetCString()
			);
#endif
		}
	};
}
