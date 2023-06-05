#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

namespace AL::FileSystem
{
	struct WaveFileFormat
	{
		uint32 SampleRate;
		uint16 ChannelCount;
		uint16 BitsPerSample;
	};

	class WaveFile
	{
#pragma pack(push, 1)
		struct Header
		{
			struct FileHeader
			{
				uint32 ChunkID;   // big endian
				uint32 ChunkSize; // little endian
				uint32 Format;    // big endian
			} File;

			struct FormatHeader
			{
				uint32 SubChunkID;    // big endian
				uint32 SubChunkSize;  // little endian
				uint16 AudioFormat;   // little endian
				uint16 NumChannels;   // little endina
				uint32 SampleRate;    // little endian
				uint32 ByteRate;      // little endian
				uint16 BlockAlign;    // little endian
				uint16 BitsPerSample; // little endian
			} Format;

			struct DataHeader
			{
				uint32 SubChunkID;   // big endian
				uint32 SubChunkSize; // little endian
			} Data;
		};
#pragma pack(pop)

		static constexpr uint32 HEADER_FORMAT          = 0x57415645;
		static constexpr uint32 HEADER_CHUNK_ID        = 0x52494646;
		static constexpr uint32 HEADER_DATA_CHUNK_ID   = 0x64617461;
		static constexpr uint32 HEADER_FORMAT_CHUNK_ID = 0x666D7420;

		File           file;
		WaveFileFormat format;
		uint32         bufferSize          = 0;
		uint32         bufferSizeRemaining = 0;

		WaveFile(const WaveFile&) = delete;

	public:
		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Open(WaveFile& file, Path&& path)
		{
			File _file(
				Move(path)
			);

			try
			{
				if (!_file.Open(FileOpenModes::Binary | FileOpenModes::Read | FileOpenModes::Write | FileOpenModes::Append))
				{

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening File"
				);
			}

			uint32 bufferSize;
			uint32 sampleRate;
			uint16 channelCount;
			uint16 bitsPerSample;

			try
			{
				ReadFileHeader(
					_file,
					sampleRate,
					channelCount,
					bitsPerSample,
					bufferSize
				);
			}
			catch (Exception&)
			{
				_file.Close();

				throw;
			}

			file.file = Move(
				_file
			);

			file.format.SampleRate    = sampleRate;
			file.format.ChannelCount  = channelCount;
			file.format.BitsPerSample = bitsPerSample;
			file.bufferSize           = bufferSize;
			file.bufferSizeRemaining  = bufferSize;

			return True;
		}

		// @throw AL::Exception
		static Void Create(WaveFile& file, Path&& path, const WaveFileFormat& format)
		{
			File _file(
				Move(path)
			);

			try
			{
				_file.Open(
					FileOpenModes::Binary | FileOpenModes::Read | FileOpenModes::Write | FileOpenModes::Truncate
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening File"
				);
			}

			try
			{
				WriteFileHeader(
					_file,
					format.SampleRate,
					format.ChannelCount,
					format.BitsPerSample
				);

				_file.SetReadPosition(
					sizeof(Header)
				);
			}
			catch (Exception&)
			{
				_file.Close();

				throw;
			}

			file.file = Move(
				_file
			);

			file.format              = format;
			file.bufferSize          = 0;
			file.bufferSizeRemaining = 0;
		}

		WaveFile()
			: file(
				""
			)
		{
		}

		WaveFile(WaveFile&& waveFile)
			: file(
				Move(waveFile.file)
			),
			format(
				Move(waveFile.format)
			),
			bufferSize(
				waveFile.bufferSize
			),
			bufferSizeRemaining(
				waveFile.bufferSizeRemaining
			)
		{
			waveFile.bufferSize          = 0;
			waveFile.bufferSizeRemaining = 0;
		}

		virtual ~WaveFile()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return file.IsOpen();
		}

		auto& GetPath() const
		{
			return file.GetPath();
		}

		auto& GetFormat() const
		{
			return format;
		}

		auto GetBufferSize() const
		{
			return bufferSize;
		}

		auto GetRemainingBufferSize() const
		{
			return bufferSizeRemaining;
		}

		Void Close()
		{
			if (IsOpen())
			{

				file.Close();
			}
		}

		// @throw AL::Exception
		// @return AL::False if end of file
		Bool Read(Void* lpBuffer, size_t size, size_t& numberOfBytesRead)
		{
			AL_ASSERT(
				IsOpen(),
				"WaveFile not open"
			);

			if ((GetRemainingBufferSize() == 0) || !ReadFileData(file, lpBuffer, size, numberOfBytesRead))
			{

				return False;
			}

			bufferSizeRemaining -= numberOfBytesRead;

			return True;
		}

		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"WaveFile not open"
			);

			AppendFileData(
				file,
				lpBuffer,
				size
			);

			bufferSizeRemaining += size;
		}

		WaveFile& operator = (WaveFile&& waveFile)
		{
			Close();

			file = Move(
				waveFile.file
			);

			format = Move(
				waveFile.format
			);

			bufferSize = waveFile.bufferSize;
			waveFile.bufferSize = 0;

			bufferSizeRemaining = waveFile.bufferSizeRemaining;
			waveFile.bufferSizeRemaining = 0;

			return *this;
		}

	private:
		// @throw AL::Exception
		static Void ReadFileHeader(File& file, Header& header)
		{
			try
			{
				if (file.Read(&header, sizeof(Header)) != sizeof(Header))
				{

					throw Exception(
						"Unexpected end of file"
					);
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error error reading Header"
				);
			}

			header.File.ChunkID         = BitConverter::FromBigEndian(header.File.ChunkID);
			header.File.ChunkSize       = BitConverter::FromLittleEndian(header.File.ChunkSize);
			header.File.Format          = BitConverter::FromBigEndian(header.File.Format);
			header.Format.SubChunkID    = BitConverter::FromBigEndian(header.Format.SubChunkID);
			header.Format.SubChunkSize  = BitConverter::FromLittleEndian(header.Format.SubChunkSize);
			header.Format.AudioFormat   = BitConverter::FromLittleEndian(header.Format.AudioFormat);
			header.Format.NumChannels   = BitConverter::FromLittleEndian(header.Format.NumChannels);
			header.Format.SampleRate    = BitConverter::FromLittleEndian(header.Format.SampleRate);
			header.Format.ByteRate      = BitConverter::FromLittleEndian(header.Format.ByteRate);
			header.Format.BlockAlign    = BitConverter::FromLittleEndian(header.Format.BlockAlign);
			header.Format.BitsPerSample = BitConverter::FromLittleEndian(header.Format.BitsPerSample);
			header.Data.SubChunkID      = BitConverter::FromBigEndian(header.Data.SubChunkID);
			header.Data.SubChunkSize    = BitConverter::FromLittleEndian(header.Data.SubChunkSize);

			if ((header.File.ChunkID != HEADER_CHUNK_ID) ||
				(header.File.Format != HEADER_FORMAT) ||
				(header.Format.SubChunkID != HEADER_FORMAT_CHUNK_ID) ||
				(header.Data.SubChunkID != HEADER_DATA_CHUNK_ID))
			{

				throw Exception(
					"Invalid header"
				);
			}
		}
		// @throw AL::Exception
		static Void ReadFileHeader(File& file, uint32& sampleRate, uint16& channelCount, uint16& bitsPerSample, uint32& bufferSize)
		{
			Header header;
			ReadFileHeader(file, header);

			bufferSize    = header.Data.SubChunkSize;
			sampleRate    = header.Format.SampleRate;
			channelCount  = header.Format.NumChannels;
			bitsPerSample = header.Format.BitsPerSample;
		}

		// @throw AL::Exception
		static Void WriteFileHeader(File& file, uint32 sampleRate, uint16 channelCount, uint16 bitsPerSample)
		{
			Header header =
			{
				.File =
				{
					.ChunkID   = HEADER_CHUNK_ID,
					.ChunkSize = BitConverter::ToLittleEndian(sizeof(Header) - 8),
					.Format    = HEADER_FORMAT
				},
				.Format =
				{
					.SubChunkID    = HEADER_FORMAT_CHUNK_ID,
					.SubChunkSize  = BitConverter::ToLittleEndian(sizeof(Header::FormatHeader) - 8),
					.AudioFormat   = BitConverter::ToLittleEndian(1),
					.NumChannels   = BitConverter::ToLittleEndian(channelCount),
					.SampleRate    = BitConverter::ToLittleEndian(sampleRate),
					.ByteRate      = BitConverter::ToLittleEndian(sampleRate * channelCount * bitsPerSample / 8),
					.BlockAlign    = BitConverter::ToLittleEndian<uint16>(channelCount * bitsPerSample / 8),
					.BitsPerSample = BitConverter::ToLittleEndian(bitsPerSample)
				},
				.Data =
				{
					.SubChunkID   = HEADER_DATA_CHUNK_ID,
					.SubChunkSize = BitConverter::ToLittleEndian(0)
				}
			};

			try
			{
				file.Write(
					&header,
					sizeof(Header)
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing Header"
				);
			}
		}

		// @throw AL::Exception
		// @return AL::False if end of file
		static Bool ReadFileData(File& file, Void* lpBuffer, size_t size, size_t& numberOfBytesRead)
		{
			if ((numberOfBytesRead = file.Read(lpBuffer, size)) == 0)
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		static Void AppendFileData(File& file, const Void* lpBuffer, size_t size)
		{
			try
			{
				file.Write(
					lpBuffer,
					size
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing buffer"
				);
			}

			auto prevReadPosition  = file.GetReadPosition();
			auto prevWritePosition = file.GetWritePosition();

			try
			{
				file.SetReadPosition(
					(sizeof(Header::FileHeader) + sizeof(Header::FormatHeader) + sizeof(Header::DataHeader)) - 4
				);

				file.SetWritePosition(
					(sizeof(Header::FileHeader) + sizeof(Header::FormatHeader) + sizeof(Header::DataHeader)) - 4
				);

				decltype(Header::DataHeader::SubChunkSize) subChunkSize;

				file.Read(
					&subChunkSize,
					sizeof(subChunkSize)
				);

				subChunkSize = BitConverter::FromLittleEndian(
					subChunkSize
				);

				subChunkSize = BitConverter::ToLittleEndian(
					subChunkSize + size
				);

				file.Write(
					&subChunkSize,
					sizeof(subChunkSize)
				);

				file.SetReadPosition(
					prevReadPosition
				);

				file.SetWritePosition(
					prevWritePosition
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating data header"
				);
			}
		}
	};
}
