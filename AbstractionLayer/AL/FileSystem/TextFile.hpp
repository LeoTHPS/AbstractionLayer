#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

namespace AL::FileSystem
{
	class TextFile
		: public File
	{
#if defined(AL_PLATFORM_LINUX)
		static constexpr const char LINE_TERMINATOR[] = "\n";
#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr const char LINE_TERMINATOR[] = "\r\n";
#endif

	public:
		using File::File;

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Open(BitMask<FileOpenModes> mode)
		{
			mode.Remove(
				FileOpenModes::Binary
			);

			return File::Open(
				mode
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadAll(String& value)
		{
			auto valueLength = static_cast<size_t>(
				(GetSize() * sizeof(String::Char)) - (GetReadPosition() * sizeof(String::Char))
			);

			value.SetSize(
				valueLength + 1
			);

			if (!File::Read(&value[0], valueLength))
			{

				return false;
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool Read(char& value)
		{
			if (!File::Read(&value, sizeof(char)))
			{

				return false;
			}

			return true;
		}
		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool Read(String& value, size_t length)
		{
			value.SetSize(
				length
			);

			uint64 bytesRead;

			if ((bytesRead = File::Read(&value[0], length)) == 0)
			{

				return false;
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false if end of file
		bool ReadLine(String& value, size_t chunkSize = 128)
		{
			uint64 bytesRead;
			uint64 totalBytesRead = 0;
			
			String lineChunkBuffer(
				'\0',
				chunkSize
			);

			if (value.GetLength())
			{

				value.Clear();
			}

			while ((bytesRead = File::Read(&lineChunkBuffer[0], chunkSize * sizeof(String::Char))) != 0)
			{
				totalBytesRead += bytesRead;
				lineChunkBuffer[static_cast<size_t>(bytesRead / sizeof(String::Char))] = String::END;

				auto endOfLineChunk = lineChunkBuffer.IndexOf(
					LINE_TERMINATOR[0]
				);

				if (endOfLineChunk == String::NPOS)
				{
					value.Append(
						lineChunkBuffer
					);

					continue;
				}

				value.Append(
					lineChunkBuffer.SubString(
						0,
						endOfLineChunk
					)
				);

#if defined(AL_PLATFORM_WINDOWS)
				if (auto endOfLineChunk2 = lineChunkBuffer.IndexOf(LINE_TERMINATOR[1]); endOfLineChunk2 > endOfLineChunk)
				{

					endOfLineChunk = endOfLineChunk2;
				}
#endif

				if (auto diff = (bytesRead - endOfLineChunk))
				{
					SetReadPosition(
						(GetReadPosition() - diff) + 1
					);
				}

				break;
			}

			return totalBytesRead > 0;
		}

		// @throw AL::Exceptions::Exception
		void Write(char value)
		{
			File::Write(
				&value,
				sizeof(char)
			);
		}
		// @throw AL::Exceptions::Exception
		void Write(const String& value)
		{
			uint64 start = 0;
			uint64 length = value.GetLength();

			do
			{
				start += File::Write(
					&value[static_cast<size_t>(start)],
					static_cast<size_t>(length - start)
				);
			} while (length - start);
		}

		// @throw AL::Exceptions::Exception
		void WriteLine(const String& value)
		{
			auto line = String::Format(
				"%s%s",
				value.GetCString(),
				LINE_TERMINATOR
			);

			Write(
				line
			);
		}
	};
}
