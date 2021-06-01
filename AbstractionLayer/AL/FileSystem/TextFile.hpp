#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

namespace AL::FileSystem
{
	class TextFile
		: public File
	{
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

			value.SetCapacity(
				valueLength
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
			value.SetCapacity(
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

				size_t endOfLineChunk;
				
				if (!lineChunkBuffer.Find(endOfLineChunk, '\n'))
				{
					value.Append(
						lineChunkBuffer
					);

					continue;
				}

				size_t endOfLineChunk2;
				bool endOfLineChunk2Found;

				if (endOfLineChunk2Found = lineChunkBuffer.Find(endOfLineChunk2, '\r'))
				{
					if ((endOfLineChunk - endOfLineChunk2) == 1)
					{
						endOfLineChunk = endOfLineChunk2;
					}
				}

				value.Append(
					lineChunkBuffer.SubString(
						0,
						endOfLineChunk
					)
				);

				if (auto diff = (bytesRead - endOfLineChunk))
				{

					SetReadPosition(
						(GetReadPosition() - diff) + (endOfLineChunk2Found ? 2 : 1)
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
			Write(value);

#if defined(AL_PLATFORM_WINDOWS)
			Write('\r');
#endif

			Write('\n');
		}
	};
}
