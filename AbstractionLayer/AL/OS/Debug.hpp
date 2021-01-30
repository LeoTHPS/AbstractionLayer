#pragma once
#include "AL/Common.hpp"

namespace AL::OS
{
	class Debug
	{
		Debug() = delete;

	public:
		static constexpr bool IsDebug()
		{
#if defined(AL_DEBUG)
			return true;
#else
			return false;
#endif
		}

		template<typename ... TArgs>
		static void Write(const String& format, TArgs ... args)
		{
	#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

		#if defined(AL_PLATFORM_LINUX)
			fprintf(
				stderr,
				message.GetCString()
			);
		#elif defined(AL_PLATFORM_WINDOWS)
			OutputDebugStringA(
				message.GetCString()
			);
		#endif
	#endif
		}
		template<typename ... TArgs>
		static void Write(const WString& format, TArgs ... args)
		{
	#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

		#if defined(AL_PLATFORM_LINUX)
			fprintf(
				stderr,
				"%S",
				message.GetCString()
			);
		#elif defined(AL_PLATFORM_WINDOWS)
			OutputDebugStringW(
				message.GetCString()
			);
		#endif
	#endif
		}

		template<typename ... TArgs>
		static void WriteLine(const String& format, TArgs ... args)
		{
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				"%s\n",
				message.GetCString()
			);
		}
		template<typename ... TArgs>
		static void WriteLine(const WString& format, TArgs ... args)
		{
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

			Write(
				L"%s\n",
				message.GetCString()
			);
		}

		template<size_t S, typename ... TArgs>
		static void Write(const char(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

	#if defined(AL_PLATFORM_LINUX)
			fprintf(
				stderr,
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			OutputDebugStringA(
				message.GetCString()
			);
	#endif
#endif
		}
		template<size_t S, typename ... TArgs>
		static void Write(const wchar_t(&format)[S], TArgs ... args)
		{
#if defined(AL_DEBUG)
			auto message = WString::Format(
				format,
				Forward<TArgs>(args) ...
			);

	#if defined(AL_PLATFORM_LINUX)
			fprintf(
				stderr,
				"%S",
				message.GetCString()
			);
	#elif defined(AL_PLATFORM_WINDOWS)
			OutputDebugStringW(
				message.GetCString()
			);
	#endif
#endif
		}

		static void WriteLine()
		{
			Write("\n");
		}

		template<size_t S, typename ... TArgs>
		static void WriteLine(const char(&format)[S], TArgs ... args)
		{
			Write(format, Forward<TArgs>(args) ...);
			Write("\n");
		}
		template<size_t S, typename ... TArgs>
		static void WriteLine(const wchar_t(&format)[S], TArgs ... args)
		{
			Write(format, Forward<TArgs>(args) ...);
			Write("\n");
		}
	};
}
