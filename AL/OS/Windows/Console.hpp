#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
{
	class Console
	{
		inline static Mutex mutex;

		Console() = delete;

	public:
		static Bool IsOpen()
		{
			if (::GetConsoleWindow() == NULL)
			{

				return False;
			}

			return True;
		}

		static Bool IsQuickEditEnabled()
		{
			if (IsOpen())
			{
				::DWORD mode;
				auto   hInput = ::GetStdHandle(STD_INPUT_HANDLE);

				if (::GetConsoleMode(hInput, &mode) && BitMask<::DWORD>::IsSet(mode, ENABLE_QUICK_EDIT_MODE))
				{

					return True;
				}
			}

			return False;
		}

		// @throw AL::Exception
		static Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"Console already open"
			);

			if (!::AllocConsole())
			{

				throw SystemException(
					"AllocConsole"
				);
			}
		}

		// @throw AL::Exception
		static Void Close()
		{
			if (IsOpen())
			{
				if (!::FreeConsole())
				{

					throw SystemException(
						"FreeConsole"
					);
				}
			}
		}

		// @throw AL::Exception
		static Void EnableQuickEdit()
		{
			AL_ASSERT(
				IsOpen(),
				"Console not open"
			);

			::DWORD mode;
			auto   hInput = ::GetStdHandle(STD_INPUT_HANDLE);

			if (!::GetConsoleMode(hInput, &mode))
			{

				throw SystemException(
					"GetConsoleMode"
				);
			}

			mode = BitMask<::DWORD>::Add(
				mode,
				ENABLE_QUICK_EDIT_MODE
			);

			if (!::SetConsoleMode(hInput, mode))
			{

				throw SystemException(
					"SetConsoleMode"
				);
			}
		}

		// @throw AL::Exception
		static Void DisableQuickEdit()
		{
			AL_ASSERT(
				IsOpen(),
				"Console not open"
			);

			::DWORD mode;
			auto   hInput = ::GetStdHandle(STD_INPUT_HANDLE);

			if (!::GetConsoleMode(hInput, &mode))
			{

				throw SystemException(
					"GetConsoleMode"
				);
			}

			mode = BitMask<::DWORD>::Remove(
				mode,
				ENABLE_QUICK_EDIT_MODE
			);

			if (!::SetConsoleMode(hInput, mode))
			{

				throw SystemException(
					"SetConsoleMode"
				);
			}
		}

		// @throw AL::Exception
		static Void SetTitle(const String& value)
		{
			SetTitle(
				value.GetCString()
			);
		}
		// @throw AL::Exception
		static Void SetTitle(const String::Char* value)
		{
			AL_ASSERT(
				IsOpen(),
				"Console not open"
			);

			MutexGuard lock(
				mutex
			);

			if (::SetConsoleTitleA(value) == 0)
			{

				throw SystemException(
					"SetConsoleTitleA"
				);
			}
		}
		// @throw AL::Exception
		template<typename ... TArgs>
		static Void SetTitle(const String::Char* format, TArgs ... args)
		{
			auto value = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			SetTitle(
				value.GetCString()
			);
		}

		// @throw AL::Exception
		static Void Clear()
		{
			AL_ASSERT(
				IsOpen(),
				"Console not open"
			);

			MutexGuard lock(
				mutex
			);

			::DWORD                    written = 0;
			::COORD                    topLeft = { 0, 0 };
			auto                       hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

			if (!::GetConsoleScreenBufferInfo(hOutput, &bufferInfo))
			{

				throw SystemException(
					"GetConsoleScreenBufferInfo"
				);
			}

			if (!::FillConsoleOutputCharacterA(hOutput, ' ', bufferInfo.dwSize.X * bufferInfo.dwSize.Y, topLeft, &written))
			{

				throw SystemException(
					"FillConsoleOutputCharacterA"
				);
			}

			if (!::FillConsoleOutputAttribute(hOutput, bufferInfo.wAttributes, bufferInfo.dwSize.X * bufferInfo.dwSize.Y, topLeft, &written))
			{

				throw SystemException(
					"FillConsoleOutputAttribute"
				);
			}

			if (!::SetConsoleCursorPosition(hOutput, topLeft))
			{

				throw SystemException(
					"SetConsoleCursorPosition"
				);
			}
		}

		// @throw AL::Exception
		template<typename ... TArgs>
		static Void Read(String::Char& value)
		{
			AL_ASSERT(
				IsOpen(),
				"Console not open"
			);
		
			MutexGuard lock(
				mutex
			);

			auto    hInput            = ::GetStdHandle(STD_INPUT_HANDLE);
			::DWORD numberOfCharsRead = 0;

			if (::ReadConsoleA(hInput, &value, 1, &numberOfCharsRead, nullptr) == 0)
			{

				throw SystemException(
					"ReadConsoleA"
				);
			}
		}

		// @throw AL::Exception
		static Void ReadLine(String& value)
		{
			value.Clear();

			MutexGuard lock(
				mutex
			);

			String::Char c;
			auto         hInput            = ::GetStdHandle(STD_INPUT_HANDLE);
			::DWORD      numberOfCharsRead = 0;

			do
			{
				if (::ReadConsoleA(hInput, &c, 1, &numberOfCharsRead, nullptr) == 0)
				{

					throw SystemException(
						"ReadConsoleA"
					);
				}

				if ((c == '\b') || (c == '\x7f'))
				{
					if (value.GetLength() != 0)
					{
						value.Erase(
							--value.end()
						);
					}
				}
				else if (c == '\r')
				{
				}
				else if (c != '\n')
				{
					if (::isprint(c))
					{
						value.Append(
							c
						);
					}
				}
			} while (c != '\n');
		}

		// @throw AL::Exception
		static Void Write(String::Char value)
		{
			Write(
				"%c",
				value
			);
		}
		// @throw AL::Exception
		static Void Write(const String& value)
		{
			Write(
				value.GetCString()
			);
		}
		// @throw AL::Exception
		template<typename ... TArgs>
		static Void Write(const String::Char* format, TArgs ... args)
		{
			auto Write = [](const String::Char* _value, size_t _length)
			{
				MutexGuard lock(
					mutex
				);

				auto    hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
				::DWORD numberOfCharsWritten;

				for (size_t totalCharsWritten = 0; totalCharsWritten < _length; )
				{
					if (!::WriteConsoleA(hOutput, &_value[totalCharsWritten], static_cast<::DWORD>(_length - totalCharsWritten), &numberOfCharsWritten, nullptr))
					{

						throw SystemException(
							"WriteConsoleA"
						);
					}

					totalCharsWritten += numberOfCharsWritten;
				}
			};

			if constexpr (sizeof ...(TArgs) == 0)
			{
				Write(
					format,
					String::GetLength(format)
				);
			}
			else
			{
				auto string = String::Format(
					format,
					Forward<TArgs>(args) ...
				);

				Write(
					string.GetCString(),
					string.GetLength()
				);
			}
		}

		// @throw AL::Exception
		static Void WriteLine()
		{
			WriteLine(
				""
			);
		}
		// @throw AL::Exception
		static Void WriteLine(const String& value)
		{
			WriteLine(
				value.GetCString()
			);
		}
		// @throw AL::Exception
		template<typename ... TArgs>
		static Void WriteLine(const String::Char* format, TArgs ... args)
		{
			if constexpr (sizeof ...(TArgs) == 0)
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

				Write(
					"%s\n",
					string.GetCString()
				);
			}
		}

		// @throw AL::Exception
		static Void WriteException(const Exception& exception)
		{
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
		}
	};
}