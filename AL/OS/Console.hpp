#pragma once
#include "AL/Common.hpp"

#include "Mutex.hpp"
#include "SystemException.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <cstdio>

	#include <unistd.h>
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
#if defined(AL_PLATFORM_LINUX)
			auto _value = String::Format(
				"\033]0;%s\007",
				value
			);

			MutexGuard lock(
				mutex
			);

			if (::write(STDOUT_FILENO, _value.GetCString(), _value.GetSize()) == -1)
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
#if defined(AL_PLATFORM_LINUX)
			static constexpr String::Char VALUE[] = "\033[H\033[J";

			MutexGuard lock(
				mutex
			);

			if (::write(STDOUT_FILENO, VALUE, sizeof(VALUE)) == -1)
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
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

#if defined(AL_PLATFORM_LINUX)
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
#if defined(AL_PLATFORM_LINUX)
				if (::read(STDIN_FILENO, &c, sizeof(String::Char)) == -1)
#elif defined(AL_PLATFORM_WINDOWS)
				if (::ReadConsoleA(hInput, &c, 1, &numberOfCharsRead, nullptr) == 0)
#endif
				{

					return False;
				}

				if (c != '\n')
				{

					value.Append(
						c
					);
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
	};
}
