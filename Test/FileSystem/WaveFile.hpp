#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/FileSystem/WaveFile.hpp>

#pragma pack(push, 0)
template<typename T>
struct AL_FileSystem_WaveFile_Channel
{
	T Value;
};

template<typename T, AL::size_t CHANNEL_COUNT>
struct AL_FileSystem_WaveFile_Frame
{
	AL_FileSystem_WaveFile_Channel<T> Channels[CHANNEL_COUNT];
};

union AL_FileSystem_WaveFile_Frames
{
	AL_FileSystem_WaveFile_Frame<AL::uint8, 1> Mono_8;
	AL_FileSystem_WaveFile_Frame<AL::int16, 1> Mono_16;
	AL_FileSystem_WaveFile_Frame<AL::int32, 1> Mono_32;

	AL_FileSystem_WaveFile_Frame<AL::uint8, 2> Stereo_8;
	AL_FileSystem_WaveFile_Frame<AL::int16, 2> Stereo_16;
	AL_FileSystem_WaveFile_Frame<AL::int32, 2> Stereo_32;
};
#pragma pack(pop)

template<typename T, AL::size_t CHANNEL_COUNT>
static void AL_FileSystem_WaveFile_DumpFrameToConsole(const AL_FileSystem_WaveFile_Frame<T, CHANNEL_COUNT>& frame)
{
	AL::OS::Console::Write(
		"Frame { .Channels = [ "
	);

	for (AL::size_t i = 0; i < CHANNEL_COUNT; ++i)
	{
		if (i != 0)
		{
			AL::OS::Console::Write(
				", "
			);
		}

		AL::OS::Console::Write(
			"{ .Value = "
		);

		if constexpr (AL::Is_Type<T, AL::uint8>::Value)
			AL::OS::Console::Write("%u", frame.Channels[i].Value);
		else if constexpr (AL::Is_Type<T, AL::int16>::Value)
			AL::OS::Console::Write("%i", frame.Channels[i].Value);
		else if constexpr (AL::Is_Type<T, AL::int32>::Value)
			AL::OS::Console::Write("%li", frame.Channels[i].Value);

		AL::OS::Console::Write(
			" }"
		);
	}

	AL::OS::Console::WriteLine(
		" ] }"
	);
}

template<typename T, AL::size_t CHANNEL_COUNT>
static void AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(AL::FileSystem::WaveFile& file, AL_FileSystem_WaveFile_Frame<T, CHANNEL_COUNT>& frame)
{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	#undef AL_TEST_SHOW_CONSOLE_OUTPUT
	#define _AL_TEST_SHOW_CONSOLE_OUTPUT
#endif

	typedef AL_FileSystem_WaveFile_Frame<T, CHANNEL_COUNT> Frame;

	for (AL::size_t numberOfBytesRead, i = 0; file.GetRemainingBufferSize() != 0; ++i)
	{
		try
		{
			if (!file.Read(&frame, sizeof(Frame), numberOfBytesRead) ||
				(numberOfBytesRead != sizeof(Frame)))
			{

				throw AL::Exception(
					"Unexpected end of file"
				);
			}
		}
		catch (AL::Exception& exception)
		{

			throw AL::Exception(
				AL::Move(exception),
				"Error reading Frame"
			);
		}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		AL_FileSystem_WaveFile_DumpFrameToConsole(
			frame
		);
#endif
	}

#if defined(_AL_TEST_SHOW_CONSOLE_OUTPUT)
	#undef _AL_TEST_SHOW_CONSOLE_OUTPUT
	#define AL_TEST_SHOW_CONSOLE_OUTPUT
#endif
}

// @throw AL::Exception
static void AL_FileSystem_WaveFile()
{
	using namespace AL;
	using namespace AL::FileSystem;

	WaveFile file;

	try
	{
		if (!WaveFile::Open(file, Path("test.wav")))
		{

			throw AL::Exception(
				"File not found"
			);
		}
	}
	catch (AL::Exception& exception)
	{

		throw AL::Exception(
			AL::Move(exception),
			"Error opening WaveFile"
		);
	}

	auto& format = file.GetFormat();

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine("Sample Rate:     %lu", format.SampleRate);
	OS::Console::WriteLine("Channel Count:   %u", format.ChannelCount);
	OS::Console::WriteLine("Bits Per Sample: %u", format.BitsPerSample);
#endif

	AL_FileSystem_WaveFile_Frames frames;

	auto TryReadFrame = [&file, &format, &frames]()
	{
		switch (format.BitsPerSample)
		{
			case 8:
			{
				switch (format.ChannelCount)
				{
					case 1:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Mono_8);
						return true;

					case 2:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Stereo_8);
						return true;
				}
			}
			break;

			case 16:
			{
				switch (format.ChannelCount)
				{
					case 1:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Mono_16);
						return true;

					case 2:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Stereo_16);
						return true;
				}
			}
			break;

			case 32:
			{
				switch (format.ChannelCount)
				{
					case 1:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Mono_32);
						return true;

					case 2:
						AL_FileSystem_WaveFile_ReadAndDumpFramesToConsole(file, frames.Stereo_32);
						return true;
				}
			}
			break;
		}

		return false;
	};

	try
	{
		if (!TryReadFrame())
		{

			throw AL::Exception(
				"Format not supported"
			);
		}
	}
	catch (AL::Exception& exception)
	{
		file.Close();

		throw AL::Exception(
			AL::Move(exception),
			"Error reading frames"
		);
	}

	file.Close();
}
