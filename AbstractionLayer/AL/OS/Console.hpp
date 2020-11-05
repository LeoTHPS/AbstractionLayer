#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <cstdio>

	#include <unistd.h>
#endif

namespace AL::OS
{
	enum class ConsoleColors : uint32
	{
#if defined(AL_PLATFORM_WINDOWS)
		Blue        = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		DarkBlue    = FOREGROUND_BLUE,

		Green       = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		DarkGreen   = FOREGROUND_GREEN,

		Cyan        = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkCyan    = FOREGROUND_GREEN | FOREGROUND_BLUE,

		Red         = FOREGROUND_INTENSITY | FOREGROUND_RED,
		DarkRed     = FOREGROUND_RED,

		Magenta     = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		DarkMagenta = FOREGROUND_RED | FOREGROUND_BLUE,

		Yellow      = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		DarkYellow  = FOREGROUND_RED | FOREGROUND_GREEN,

		Gray        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkGray    = FOREGROUND_INTENSITY,

		White       = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

		Default     = White
#endif
	};

	class Console
	{
		enum class Flags
		{
			Created           = 0x1,
			Allocated         = 0x2,
			ExitButtonEnabled = 0x4
		};

#if defined(AL_PLATFORM_WINDOWS)
		inline static HWND hWindow;
		inline static HANDLE hInput;
		inline static HANDLE hOutput;
#endif

		inline static ConsoleColors color;
		inline static BitMask<Flags> flags;

		Console() = delete;

	public:
		static bool IsCreated()
		{
			return flags.IsSet(
				Flags::Created
			);
		}

		static bool IsExitButtonEnabled()
		{
			return flags.IsSet(
				Flags::ExitButtonEnabled
			);
		}

		// @throw AL::Exceptions::Exception
		static String GetTitle()
		{
#if defined(AL_PLATFORM_WINDOWS)
			String title(
				'\0',
				512
			);

			if (IsCreated() && !GetConsoleTitleA(&title[0], static_cast<DWORD>(title.GetSize() - 1)))
			{

				throw Exceptions::SystemException(
					"GetConsoleTitleA"
				);
			}
			
			return title;
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		static ConsoleColors GetTextColor()
		{
			return color;
		}

		// @throw AL::Exceptions::Exception
		static void Create()
		{
#if defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(!IsCreated(), "Console already created");

			if (!(hInput = GetStdHandle(STD_INPUT_HANDLE)) || !(hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
			{
				if (!AllocConsole())
				{
					hInput = INVALID_HANDLE_VALUE;
					hOutput = INVALID_HANDLE_VALUE;

					throw Exceptions::SystemException(
						"AllocConsole"
					);
				}

				flags.Add(
					Flags::Allocated
				);

				hInput = GetStdHandle(STD_INPUT_HANDLE);
				hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			}

			if ((hInput == INVALID_HANDLE_VALUE) ||
				(hOutput == INVALID_HANDLE_VALUE))
			{
				if (flags.IsSet(Flags::Allocated))
				{
					FreeConsole();

					flags.Remove(
						Flags::Allocated
					);
				}

				hInput = INVALID_HANDLE_VALUE;
				hOutput = INVALID_HANDLE_VALUE;

				throw Exceptions::SystemException(
					"GetStdHandle"
				);
			}

			if (!(hWindow = GetConsoleWindow()))
			{
				if (flags.IsSet(Flags::Allocated))
				{
					FreeConsole();

					flags.Remove(
						Flags::Allocated
					);
				}

				hInput = INVALID_HANDLE_VALUE;
				hOutput = INVALID_HANDLE_VALUE;

				throw Exceptions::SystemException(
					"GetConsoleWindow"
				);
			}

			if (!SetConsoleCtrlHandler(&Console::OnEvent, TRUE))
			{
				if (flags.IsSet(Flags::Allocated))
				{
					FreeConsole();

					flags.Remove(
						Flags::Allocated
					);
				}

				hInput = INVALID_HANDLE_VALUE;
				hOutput = INVALID_HANDLE_VALUE;

				throw Exceptions::SystemException(
					"SetConsoleCtrlHandler"
				);
			}
#endif

			flags.Add(
				Flags::Created
			);
		}

		static void Destroy()
		{
			if (IsCreated())
			{
#if defined(AL_PLATFORM_WINDOWS)
				SetConsoleCtrlHandler(
					&Console::OnEvent,
					FALSE
				);

				if (flags.IsSet(Flags::Allocated))
				{
					FreeConsole();

					flags.Remove(
						Flags::Allocated
					);
				}

				hWindow = NULL;
				hInput = INVALID_HANDLE_VALUE;
				hOutput = INVALID_HANDLE_VALUE;

				flags.Add(
					Flags::ExitButtonEnabled
				);

#endif

				flags.Remove(
					Flags::Created
				);
			}
		}

		// @throw AL::Exceptions::Exception
		static void EnableExitButton(bool set = true)
		{
#if defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(IsCreated(), "Console not created");

			if (EnableMenuItem(
				GetSystemMenu(
					hWindow,
					FALSE
				),
				SC_CLOSE,
				set ? MF_ENABLED : MF_GRAYED
			) == -1)
			{

				throw Exceptions::SystemException(
					"EnableMenuItem"
				);
			}

			flags.Set(
				Flags::ExitButtonEnabled,
				set
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		static void SetTextColor(ConsoleColors value)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (!SetConsoleTextAttribute(
				hOutput,
				static_cast<WORD>(
					value
				)
			))
			{

				throw Exceptions::SystemException(
					"SetConsoleTextAttribute"
				);
			}

			color = value;
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		static void SetTitle(const String& value)
		{
#if defined(AL_PLATFORM_LINUX)
			printf(
				"\033]0;%s\007",
				value.GetCString()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(IsCreated(), "Console not created");

			if (!SetConsoleTitleA(value.GetCString()))
			{

				throw Exceptions::SystemException(
					"SetConsoleTitleA"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Clear()
		{
#if defined(AL_PLATFORM_LINUX)
			printf("\033[H\033[J");
#elif defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(IsCreated(), "Console not created");

			DWORD written = 0;
			COORD topLeft = { 0, 0 };

			CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

			if (!GetConsoleScreenBufferInfo(hOutput, &bufferInfo))
			{

				throw Exceptions::SystemException(
					"GetConsoleScreenBufferInfo"
				);
			}

			if (!FillConsoleOutputCharacterA(
				hOutput,
				' ',
				bufferInfo.dwSize.X * bufferInfo.dwSize.Y,
				topLeft,
				&written
			))
			{

				throw Exceptions::SystemException(
					"FillConsoleOutputCharacterA"
				);
			}

			if (!FillConsoleOutputAttribute(
				hOutput,
				bufferInfo.wAttributes,
				bufferInfo.dwSize.X * bufferInfo.dwSize.Y,
				topLeft,
				&written
			))
			{

				throw Exceptions::SystemException(
					"FillConsoleOutputAttribute"
				);
			}

			if (!SetConsoleCursorPosition(hOutput, topLeft))
			{

				throw Exceptions::SystemException(
					"SetConsoleCursorPosition"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		static char Read()
		{
			char c = '\0';

#if defined(AL_PLATFORM_LINUX)
			read(STDIN_FILENO, &c, sizeof(c));
#elif defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(IsCreated(), "Console not created");

			DWORD charsRead = 0;

			if (!ReadConsoleA(hInput, &c, 1, &charsRead, nullptr))
			{

				throw Exceptions::SystemException(
					"ReadConsoleA"
				);
			}
#endif

			return c;
		}

		// @throw AL::Exceptions::Exception
		static String ReadLine()
		{
			char c;
			String line;

			while ((c = Read()) != '\n')
			{
#if defined(AL_PLATFORM_WINDOWS)
				if (c == '\r')
				{

					continue;
				}
#endif

				line.Append(
					{ c, '\0' }
				);
			}

			return line;
		}

		// @throw AL::Exceptions::Exception
		static void Write(const String& value)
		{
#if defined(AL_PLATFORM_LINUX)
			printf(value.GetCString());
#elif defined(AL_PLATFORM_WINDOWS)
			AL_ASSERT(IsCreated(), "Console not created");

			for (size_t totalBytesWritten = 0; totalBytesWritten < value.GetLength(); )
			{
				DWORD bytesWritten = 0;

				if (!WriteConsoleA(
					hOutput,
					&value[totalBytesWritten],
					static_cast<DWORD>(
						value.GetLength() - totalBytesWritten
					),
					&bytesWritten,
					nullptr
				))
				{

					throw Exceptions::SystemException(
						"WriteConsoleA"
					);
				}

				totalBytesWritten += bytesWritten;
			}
#endif
		}
		// @throw AL::Exceptions::Exception
		template<typename ... TArgs>
		static void Write(const String& value, TArgs ... args)
		{
			Write(
				String::Format(
					value,
					Forward<TArgs>(args) ...
				)
			);
		}

		// @throw AL::Exceptions::Exception
		static void WriteLine(const String& value)
		{
			auto line = String::Format(
				"%s\n",
				value.GetCString()
			);

			Write(
				line
			);
		}
		// @throw AL::Exceptions::Exception
		template<typename ... TArgs>
		static void WriteLine(const String& value, TArgs ... args)
		{
			WriteLine(
				String::Format(
					value,
					Forward<TArgs>(args) ...
				)
			);
		}

	private:
#if defined(AL_PLATFORM_WINDOWS)
		static BOOL WINAPI OnEvent(DWORD dwCtrlType)
		{
			switch (dwCtrlType)
			{
				case CTRL_C_EVENT:
				case CTRL_BREAK_EVENT:
					return TRUE;

				case CTRL_CLOSE_EVENT:
				case CTRL_LOGOFF_EVENT:
				case CTRL_SHUTDOWN_EVENT:
					Destroy();
					return TRUE;
			}

			return FALSE;
		}
#endif
	};
}
