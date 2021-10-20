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
#else
			throw PlatformNotSupportedException();
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
#else
			throw PlatformNotSupportedException();
#endif

			return True;
		}

		static Bool Read(String::Char& c)
		{
			MutexGuard lock(
				mutex
			);

#if defined(AL_PLATFORM_LINUX)
			if (::read(STDIN_FILENO, &c, sizeof(String::Char)) == -1)
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			auto hInput = ::GetStdHandle(
				STD_INPUT_HANDLE
			);

			::DWORD numberOfCharsRead = 0;

			if (::ReadConsoleA(hInput, &c, 1, &numberOfCharsRead, nullptr) == 0)
			{

				return False;
			}
#else
			throw PlatformNotSupportedException();
#endif

			return True;
		}

		static Bool ReadLine(String& line)
		{
			line.Clear();

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
#else
				throw PlatformNotSupportedException();
#endif
				{

					return False;
				}

				if (c != '\n')
				{

					line.Append(
						c
					);
				}
			} while (c != '\n');

			return True;
		}

		static Bool Write(String::Char value)
		{
			MutexGuard lock(
				mutex
			);

#if defined(AL_PLATFORM_LINUX)
			if (::write(STDOUT_FILENO, &value, sizeof(String::Char)) == -1)
			{

				return False;
			}

			if (::write(STDOUT_FILENO, &String::END, sizeof(String::Char)) == -1)
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::DWORD numberOfCharsWritten = 0;

			auto hOutput = ::GetStdHandle(
				STD_OUTPUT_HANDLE
			);

			if (::WriteConsoleA(hOutput, &value, sizeof(String::Char), &numberOfCharsWritten, nullptr) == 0)
			{

				return False;
			}

			if (::WriteConsoleA(hOutput, &String::END, sizeof(String::Char), &numberOfCharsWritten, nullptr) == 0)
			{

				return False;
			}
#else
			throw PlatformNotSupportedException();
#endif

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
		static Bool Write(const String::Char* value)
		{
			auto valueLength = String::GetLength(
				value
			);

			MutexGuard lock(
				mutex
			);

#if defined(AL_PLATFORM_LINUX)
			if (::write(STDOUT_FILENO, value, valueLength + sizeof(String::Char)) == -1)
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			auto hOutput = ::GetStdHandle(
				STD_OUTPUT_HANDLE
			);

			for (size_t totalCharsWritten = 0; totalCharsWritten < valueLength; )
			{
				::DWORD numberOfCharsWritten = 0;

				if (!::WriteConsoleA(hOutput, &value[totalCharsWritten], static_cast<::DWORD>(valueLength - totalCharsWritten), &numberOfCharsWritten, nullptr))
				{

					return False;
				}

				totalCharsWritten += numberOfCharsWritten;
			}
#else
			throw PlatformNotSupportedException();
#endif

			return True;
		}

		template<typename ... TArgs>
		static Bool Write(const String& format, TArgs ... args)
		{
			auto value = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!Write(value))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool Write(const String::Char* format, TArgs ... args)
		{
			auto value = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!Write(value.GetCString()))
			{

				return False;
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
		static Bool WriteLine(const String::Char* value)
		{
			auto message = String::Format(
				"%s\n",
				value
			);

			if (!Write(message))
			{

				return False;
			}

			return True;
		}
		static Bool WriteLine(const String& value)
		{
			auto message = String::Format(
				"%s\n",
				value.GetCString()
			);

			if (!Write(message))
			{

				return False;
			}

			return True;
		}

		template<typename ... TArgs>
		static Bool WriteLine(const String& format, TArgs ... args)
		{
			auto value = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!WriteLine(value))
			{

				return False;
			}

			return True;
		}
		template<typename ... TArgs>
		static Bool WriteLine(const String::Char* format, TArgs ... args)
		{
			auto value = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			if (!WriteLine(value))
			{

				return False;
			}

			return True;
		}
	};
}
