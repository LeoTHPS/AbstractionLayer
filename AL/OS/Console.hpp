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
		static Bool IsOpen()
		{
#if defined(AL_PLATFORM_PICO) || defined(AL_PLATFORM_LINUX)
			// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
			if (::GetConsoleWindow() == NULL)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool SetOpen(Bool set = True)
		{
#if defined(AL_PLATFORM_PICO)
			// not supported
			return False;
#elif defined(AL_PLATFORM_LINUX)
			if (!set)
			{
				// TODO: implement
				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (set)
			{
				if (!IsOpen() && !::AllocConsole())
				{

					return False;
				}
			}
			else if (IsOpen() && !::FreeConsole())
			{

				return False;
			}
#endif

			return True;
		}

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
			MutexGuard lock(
				mutex
			);

			if (::SetConsoleTitleA(value) == 0)
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
			MutexGuard lock(
				mutex
			);

			::DWORD written = 0;
			::COORD topLeft = { 0, 0 };

			auto hOutput = ::GetStdHandle(
				STD_OUTPUT_HANDLE
			);

			CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

			if (!GetConsoleScreenBufferInfo(hOutput, &bufferInfo))
			{

				return False;
			}

			if (!::FillConsoleOutputCharacterA(hOutput, ' ', bufferInfo.dwSize.X * bufferInfo.dwSize.Y, topLeft, &written))
			{

				return False;
			}

			if (!::FillConsoleOutputAttribute(hOutput, bufferInfo.wAttributes, bufferInfo.dwSize.X * bufferInfo.dwSize.Y, topLeft, &written))
			{

				return False;
			}

			if (!::SetConsoleCursorPosition(hOutput, topLeft))
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
			auto hInput = ::GetStdHandle(
				STD_INPUT_HANDLE
			);

			::DWORD numberOfCharsRead = 0;

			if (::ReadConsoleA(hInput, &value, 1, &numberOfCharsRead, nullptr) == 0)
			{

				return False;
			}
#endif

			return True;
		}

		static Bool ReadLine(String& value)
		{
			value.Clear();

#if defined(AL_PLATFORM_WINDOWS)
			auto hInput = ::GetStdHandle(
				STD_INPUT_HANDLE
			);

			::DWORD numberOfCharsRead = 0;
#endif

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
#elif defined(AL_PLATFORM_WINDOWS)
				if (::ReadConsoleA(hInput, &c, 1, &numberOfCharsRead, nullptr) == 0)
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
		template<typename ... TArgs>
		static Bool Write(const String& format, TArgs ... args)
		{
			if (!Write(format.GetCString(), Forward<TArgs>(args) ...))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool Write(const String::Char* format, TArgs ... args)
		{
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

			return True;
		}

		static Bool WriteLine()
		{
			if (!Write("\n"))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool WriteLine(const String& format, TArgs ... args)
		{
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!Write("%s\n", string.GetCString()))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool WriteLine(const String::Char* format, TArgs ... args)
		{
			auto string = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!Write("%s\n", string.GetCString()))
			{

				return False;
			}

			return True;
		}

		static Bool WriteException(const Exception& exception)
		{
			if (!WriteLine(exception.GetMessage()))
			{

				return False;
			}

			if (auto lpInnerException = exception.GetInnerException())
			{
				do
				{
					if (!WriteLine(lpInnerException->GetMessage()))
					{

						return False;
					}
				} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
			}

			return True;
		}
	};
}
