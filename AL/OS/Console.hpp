#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"
#include "SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include <pico/stdio.h>
#elif defined(AL_PLATFORM_LINUX)
	#include <cstdio>

	#include <unistd.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Console.hpp"
#else
	#error Platform not supported
#endif

namespace AL::OS
{
	class Console
	{
		inline static Mutex mutex;

		Console() = delete;

	public:
		static Bool SetTitle(const String& value)
		{
			if (!SetTitle(value.GetCString()))
			{

				return False;
			}

			return True;
		}
		static Bool SetTitle(const String::Char* value)
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
			auto _value = String::Format(
				"\033]0;%s\007",
				value
			);

			if (!Write(_value))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::SetTitle(
					value
				);
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool Clear()
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
			static constexpr String::Char VALUE[] = "\033[H\033[J";

			if (!Write(VALUE))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::Clear();
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool Read(String::Char& value)
		{
			MutexGuard lock(
				mutex
			);

#if defined(AL_PLATFORM_PICO)
			value = static_cast<typename String::Char>(
				::getchar()
			);

			::putchar(
				value
			);

			return True;
#elif defined(AL_PLATFORM_LINUX)
			if (::read(STDIN_FILENO, &value, sizeof(String::Char)) == -1)
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::Read(
					value
				);
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool ReadLine(String& value)
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
			value.Clear();

			String::Char c;

			MutexGuard lock(
				mutex
			);

			do
			{
	#if defined(AL_PLATFORM_PICO)
				c = static_cast<typename String::Char>(
					::getchar()
				);
	#elif defined(AL_PLATFORM_LINUX)
				if (::read(STDIN_FILENO, &c, sizeof(String::Char)) == -1)
				{

					return False;
				}
	#endif

				if ((c == '\b') || (c == '\x7f'))
				{
					if (value.GetLength() != 0)
					{
	#if defined(AL_PLATFORM_PICO)
						::putchar_raw(
							c
						);
	#endif

						value.Erase(
							--value.end()
						);
					}
				}
				else if (c == '\r')
				{
	#if defined(AL_PLATFORM_PICO)
					c = '\n';

					::putchar(
						c
					);
	#endif
				}
				else if (c != '\n')
				{
	#if defined(AL_PLATFORM_PICO)
					::putchar_raw(
						c
					);
	#endif

					if (::isprint(c))
					{
						value.Append(
							c
						);
					}
				}
			} while (c != '\n');
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::ReadLine(
					value
				);
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool Write(String::Char value)
		{
			auto string = String::Format(
				"%c",
				value
			);

			if (!Write(string))
			{

				return False;
			}

			return True;
		}
		static Bool Write(const String& value)
		{
			if (!Write(value.GetCString()))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool Write(const String::Char* format, TArgs ... args)
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
	#if defined(AL_PLATFORM_PICO)
			MutexGuard lock(
				mutex
			);

			::printf(
				format,
				Forward<TArgs>(args) ...
			);
	#else
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

		#if defined(AL_PLATFORM_LINUX)
			::ssize_t numberOfCharsWritten;
		#elif defined(AL_PLATFORM_WINDOWS)
			auto hOutput = ::GetStdHandle(
				STD_OUTPUT_HANDLE
			);

			::DWORD numberOfCharsWritten;
		#endif

			MutexGuard lock(
				mutex
			);

			for (size_t totalCharsWritten = 0; totalCharsWritten < string.GetLength(); )
			{
		#if defined(AL_PLATFORM_LINUX)
				if ((numberOfCharsWritten = ::write(STDOUT_FILENO, &string[totalCharsWritten], string.GetLength() - totalCharsWritten)) == -1)
				{

					return False;
				}
		#elif defined(AL_PLATFORM_WINDOWS)
				if (!::WriteConsoleA(hOutput, &string[totalCharsWritten], static_cast<::DWORD>(string.GetLength() - totalCharsWritten), &numberOfCharsWritten, nullptr))
				{

					return False;
				}
		#endif

				totalCharsWritten += numberOfCharsWritten;
			}
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::Write(
					format,
					Forward<TArgs>(args) ...
				);
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool WriteLine()
		{
			if (!WriteLine(""))
			{

				return False;
			}

			return True;
		}
		static Bool WriteLine(const String& value)
		{
			if (!WriteLine(value.GetCString()))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool WriteLine(const String::Char* format, TArgs ... args)
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
			if (sizeof ...(TArgs) == 0)
			{
				Write(
					"%s\n",
					format
				);
			}
			else
			{
				auto string = String::Format(
					format,
					Forward<TArgs>(args) ...
				);

				if (!Write("%s\n", string.GetCString()))
				{

					return False;
				}
			}
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				Windows::Console::WriteLine(
					format,
					Forward<TArgs>(args) ...
				);
			}
			catch (const Exception&)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool WriteException(const Exception& exception)
		{
			for (auto lpException = &exception; lpException != nullptr; lpException = lpException->GetInnerException())
			{
				if (!WriteLine(lpException->GetMessage()))
				{

					return False;
				}
			}

			return True;
		}
	};
}
