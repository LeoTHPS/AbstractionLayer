#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::FileSystem
{
	enum class TextFileLineEndings
	{
		Auto, LF, CRLF
	};

	class TextFile
		: public File
	{
		TextFileLineEndings lineEnding = TextFileLineEndings::Auto;

	public:
		static constexpr String::Char LF[]   = "\n";
		static constexpr String::Char CRLF[] = "\r\n";

		using File::File;

		auto GetLineEnding() const
		{
			return lineEnding;
		}

		Void SetLineEnding(TextFileLineEndings value)
		{
			lineEnding = value;
		}

		// @throw AL::Exception
		// @return AL::False if not found
		Bool Open(FileOpenModes mode)
		{
			mode = BitMask<FileOpenModes>::Remove(
				mode,
				FileOpenModes::Binary
			);

			if (!File::Open(mode))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if end of file
		Bool Read(String::Char& value)
		{
			if (!Read(&value, 1))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if end of file
		template<size_t S>
		Bool Read(String::Char(&value)[S])
		{
			if (!Read(&value[0], S - 1))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if end of file
		Bool Read(String& value, size_t length)
		{
			Collections::Array<String::Char> buffer(
				length
			);

			size_t bytesRead;

			if ((bytesRead = File::Read(&buffer[0], length * sizeof(String::Char))) == 0)
			{

				return False;
			}

			value.Assign(
				&buffer[0],
				bytesRead / sizeof(String::Char)
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if end of file
		Bool Read(String::Char* lpValue, size_t length)
		{
			size_t bytesRead;

			if ((bytesRead = File::Read(lpValue, length * sizeof(String::Char))) == 0)
			{

				return False;
			}

			lpValue[bytesRead] = String::END;

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if end of file
		Bool ReadAll(String& value)
		{
			auto remainingCharCount = static_cast<size_t>(
				(GetSize() / sizeof(String::Char)) - (GetReadPosition() / sizeof(String::Char))
			);

			if (!Read(value, remainingCharCount))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if end of file
		Bool ReadLine(String& value, size_t chunkSize = 128)
		{
			Collections::Array<String::Char> chunkBuffer(
				chunkSize
			);

			size_t bytesRead;
			size_t totalBytesRead = 0;

			auto chunkBuffer_IndexOfEOL = [&chunkBuffer, &bytesRead](TextFileLineEndings _lineEnding, String::Char* _lpPrevChunkFirstChar)->size_t
			{
				auto chunkBuffer_IndexOf_LF = [&chunkBuffer, &bytesRead]()->size_t
				{
					auto lpBuffer = &chunkBuffer[0];
					auto charsRead = bytesRead / sizeof(String::Char);

					for (size_t i = 0; i < charsRead; ++i, ++lpBuffer)
					{
						if (*lpBuffer == LF[0])
						{

							return i;
						}
					}

					return String::END;
				};

				auto chunkBuffer_IndexOf_CRLF = [&chunkBuffer, &bytesRead](String::Char* __lpPrevChunkFirstChar)->size_t
				{
					auto lpBuffer = &chunkBuffer[0];
					auto charsRead = bytesRead / sizeof(String::Char);

					for (size_t i = 0; i < charsRead; ++i, ++lpBuffer)
					{
						if (*lpBuffer == CRLF[1])
						{
							if (__lpPrevChunkFirstChar && (*__lpPrevChunkFirstChar == CRLF[0]))
							{

								return i;
							}
						}

						if (*lpBuffer == CRLF[0])
						{
							if (((i + 1) < charsRead) && (*(lpBuffer + 1) == CRLF[1]))
							{

								return i;
							}
						}
					}

					return String::END;
				};

				switch (_lineEnding)
				{
					case TextFileLineEndings::Auto:
#if defined(AL_PLATFORM_LINUX)
						return chunkBuffer_IndexOf_LF();
#elif defined(AL_PLATFORM_WINDOWS)
						return chunkBuffer_IndexOf_CRLF(_lpPrevChunkFirstChar);
#endif

					case TextFileLineEndings::LF:
						return chunkBuffer_IndexOf_LF();

					case TextFileLineEndings::CRLF:
						return chunkBuffer_IndexOf_CRLF(_lpPrevChunkFirstChar);
				}
			};

			auto lineEnding = GetLineEnding();

			while ((bytesRead = File::Read(&chunkBuffer[0], chunkSize * sizeof(String::Char))) != 0)
			{
				size_t chunkBufferEOL;

				auto prevTotalBytesRead = totalBytesRead;

				totalBytesRead += bytesRead;

				if (prevTotalBytesRead == 0)
				{

					value.Clear();
				}

				if ((chunkBufferEOL = chunkBuffer_IndexOfEOL(lineEnding, prevTotalBytesRead ? &chunkBuffer[prevTotalBytesRead - 1] : nullptr)) != String::END)
				{
					value.Append(
						&chunkBuffer[0],
						chunkBufferEOL / sizeof(String::Char)
					);

					if (auto diff = (bytesRead - chunkBufferEOL))
					{
						switch (lineEnding)
						{
							case TextFileLineEndings::Auto:
#if defined(AL_PLATFORM_LINUX)
								SetReadPosition((GetReadPosition() - diff) + 1);
#elif defined(AL_PLATFORM_WINDOWS)
								SetReadPosition((GetReadPosition() - diff) + 2);
#endif
								break;

							case TextFileLineEndings::LF:
								SetReadPosition((GetReadPosition() - diff) + 1);
								break;

							case TextFileLineEndings::CRLF:
								SetReadPosition((GetReadPosition() - diff) + 2);
								break;
						}
					}

					break;
				}

				value.Append(
					&chunkBuffer[0],
					bytesRead / sizeof(String::Char)
				);
			}

			return totalBytesRead != 0;
		}

		// @throw AL::Exception
		Void Write(String::Char value)
		{
			File::Write(
				&value,
				sizeof(String::Char)
			);
		}
		// @throw AL::Exception
		Void Write(const String& value)
		{
			Write(
				value.GetCString(),
				value.GetLength()
			);
		}
		// @throw AL::Exception
		template<size_t S>
		Void Write(const String::Char(&value)[S])
		{
			Write(
				&value[0],
				S - 1
			);
		}
		// @throw AL::Exception
		Void Write(const String::Char* lpValue, size_t length)
		{
			for (size_t totalBytesWritten = 0; totalBytesWritten < length; )
			{
				auto bytesWritten = File::Write(
					&lpValue[totalBytesWritten],
					length - totalBytesWritten
				);

				totalBytesWritten += bytesWritten;
			}
		}

		// @throw AL::Exception
		Void WriteLine(String::Char value)
		{
			WriteLine(
				&value,
				1
			);
		}
		// @throw AL::Exception
		Void WriteLine(const String& value)
		{
			WriteLine(
				value.GetCString(),
				value.GetLength()
			);
		}
		// @throw AL::Exception
		template<size_t S>
		Void WriteLine(const String::Char(&value)[S])
		{
			WriteLine(
				&value[0],
				S - 1
			);
		}
		// @throw AL::Exception
		Void WriteLine(const String::Char* lpValue, size_t length)
		{
			Write(
				lpValue,
				length
			);

			switch (GetLineEnding())
			{
				case TextFileLineEndings::Auto:
#if defined(AL_PLATFORM_LINUX)
					Write(LF);
#elif defined(AL_PLATFORM_WINDOWS)
					Write(CRLF);
#endif
					break;

				case TextFileLineEndings::LF:
					Write(LF);
					break;

				case TextFileLineEndings::CRLF:
					Write(CRLF);
					break;
			}
		}
	};
}
