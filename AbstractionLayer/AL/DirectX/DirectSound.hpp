#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#include "AL/FileSystem/File.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/Exceptions/DirectSoundException.hpp"

#include <MMSystem.h>
#include <dsound.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace AL::DirectX
{
	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using DirectSoundResource = Resource<T, TReleaser>;

	class DirectSoundBuffer
		: public DirectSoundResource<IDirectSoundBuffer>
	{
		bool isPaused = false;
		bool isPlaying = false;
		bool isRepeating = false;

		uint32 pausePosition = 0;

	public:
		using DirectSoundResource<IDirectSoundBuffer>::DirectSoundResource;

		bool IsPaused() const
		{
			return *this && isPaused;
		}

		bool IsPlaying() const
		{
			return *this && isPlaying;
		}

		bool IsRepeating() const
		{
			return *this && isRepeating;
		}

		// @throw AL::Exceptions::Exception
		auto GetVolume() const
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			LONG volume;
			HRESULT hResult;

			if ((hResult = lpResource->GetVolume(&volume)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::GetVolume",
					hResult
				);
			}

			return static_cast<float>(volume) / DSBVOLUME_MIN;
		}

		// @throw AL::Exceptions::Exception
		auto GetPosition() const
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			HRESULT hResult;
			DWORD currentRead, currentWrite;

			if ((hResult = lpResource->GetCurrentPosition(&currentRead, &currentWrite)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::GetCurrentPosition",
					hResult
				);
			}

			return static_cast<uint32>(
				currentRead
			);
		}

		// @throw AL::Exceptions::Exception
		void SetVolume(float volume)
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");
			
			auto _volume = static_cast<LONG>(
				DSBVOLUME_MIN - (DSBVOLUME_MIN * volume)
			);

			HRESULT hResult;

			if ((hResult = lpResource->SetVolume(_volume)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::SetVolume",
					hResult
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetPosition(uint32 position)
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			HRESULT hResult;

			if ((hResult = lpResource->SetCurrentPosition(static_cast<DWORD>(position))) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::SetCurrentPosition",
					hResult
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void Play(bool repeat = false)
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			if (IsPaused())
			{

				SetPosition(
					pausePosition
				);
			}

			HRESULT hResult;

			if ((hResult = lpResource->Play(0, 0, repeat ? DSBPLAY_LOOPING : 0)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::Play",
					hResult
				);
			}

			isPaused = false;
			isPlaying = true;
			isRepeating = repeat;
		}

		// @throw AL::Exceptions::Exception
		void Pause()
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			HRESULT hResult;
			pausePosition = GetPosition();

			if ((hResult = lpResource->Stop()) != DS_OK)
			{

				throw AL::Exceptions::DirectSoundException(
					"IDirectSoundBuffer::Stop",
					hResult
				);
			}

			isPaused = true;
			isPlaying = false;
		}

		// @throw AL::Exceptions::Exception
		void Stop()
		{
			IDirectSoundBuffer* lpResource;

			AL_ASSERT((lpResource = GetResource()), "Resource not created");

			HRESULT hResult;

			if ((hResult = lpResource->Stop()) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::Stop",
					hResult
				);
			}

			isPaused = false;
			isPlaying = false;
			isRepeating = false;
		}
	};

	struct DirectSoundDevice
	{
		GUID GUID;
		String Name;
		String Module;
	};

	typedef Function<bool(const DirectSoundDevice&)> DirectSoundDeviceEnumCallback;

	class DirectSound
	{
		typedef DirectSoundResource<IDirectSound8> OutputDevice;

		OutputDevice output;

	public:
		bool IsCreated() const
		{
			return output.IsCreated();
		}

		// @throw AL::Exceptions::Exception
		void Create(HWND hWnd)
		{
			AL_ASSERT(!IsCreated(), "DirectSound already created");

			HRESULT hResult;
			OutputDevice::Type* lpOutputDevice;

			if ((hResult = DirectSoundCreate8(nullptr, &lpOutputDevice, nullptr)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"DirectSoundCreate8",
					hResult
				);
			}

			if ((hResult = lpOutputDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) != DS_OK)
			{
				lpOutputDevice->Release();

				throw Exceptions::DirectSoundException(
					"IDirectSound8::SetCooperativeLevel",
					hResult
				);
			}

			output = lpOutputDevice;
		}
		// @throw AL::Exceptions::Exception
		void Create(HWND hWnd, const DirectSoundDevice& device)
		{
			AL_ASSERT(!IsCreated(), "DirectSound already created");

			HRESULT hResult;
			OutputDevice::Type* lpOutputDevice;

			if ((hResult = DirectSoundCreate8(&device.GUID, &lpOutputDevice, nullptr)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"DirectSoundCreate8",
					hResult
				);
			}

			if ((hResult = lpOutputDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) != DS_OK)
			{
				lpOutputDevice->Release();

				throw Exceptions::DirectSoundException(
					"IDirectSound8::SetCooperativeLevel",
					hResult
				);
			}

			output = lpOutputDevice;
		}
		
		void Destroy()
		{
			if (IsCreated())
			{
				output.Release();
			}
		}

		// @throw AL::Exceptions::Exception
		void CreateBuffer(DirectSoundBuffer& buffer, const FileSystem::Path& file)
		{
			AL_ASSERT(!IsCreated(), "DirectSound not created");

			FileSystem::File _file(
				file
			);

			_file.Open(
				FileSystem::FileOpenModes::Read
			);

			Collections::Array<uint8> _buffer;

			try
			{
				if (auto fileSize = _file.GetSize())
				{
					_buffer.SetSize(
						static_cast<size_t>(fileSize)
					);

					_file.Read(
						&_buffer[0],
						_buffer.GetSize()
					);
				}
			}
			catch (Exceptions::Exception& exception)
			{
				_file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error reading file"
				);
			}

			CreateBuffer(
				buffer,
				&_buffer[0],
				_buffer.GetSize()
			);
		}
		// @throw AL::Exceptions::Exception
		void CreateBuffer(DirectSoundBuffer& buffer, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(!IsCreated(), "DirectSound not created");

			struct FileHeader
			{
				char ChunkID[4]; // RIFF
				uint32 ChunkSize; // 
				char Format[4]; // WAVE

				char SubChunk1Id[4]; // fmt | data
				uint32 SubChunk1Size; // PCM == 16
				uint16 AudioFormat; // PCM == 1
				uint16 NumChannels; // Mono == 1 | Stereo = 2
				uint32 SampleRate; // 44100
				uint32 ByteRate; // SampleRate * NumChannels * BitsPerSample / 8
				uint16 BlockAlign; // NumChannels * BitsPerSample / 8
				uint16 BitsPerSample; // 8/16/32/64

				char SubChunk2Id[4]; // data
				uint32 SubChunk2Size; // NumSamples * NumChannels * BitsPerSample / 8
			};

			static_assert(
				sizeof(FileHeader) == 44,
				"Invalid FileHeader size"
			);

			if (size < sizeof(FileHeader))
			{

				throw Exceptions::Exception(
					"Buffer too small"
				);
			}

			auto lpHeader = reinterpret_cast<const FileHeader*>(lpBuffer);

			if (lpHeader->Format[0] != 'W' || lpHeader->Format[1] != 'A' ||
				lpHeader->Format[2] != 'V' || lpHeader->Format[3] != 'E')
			{

				throw Exceptions::Exception(
					"Invalid file format"
				);
			}

			if (lpHeader->ChunkID[0] != 'R' || lpHeader->ChunkID[1] != 'I' ||
				lpHeader->ChunkID[2] != 'F' || lpHeader->ChunkID[3] != 'F')
			{

				throw Exceptions::Exception(
					"Invalid file chunk"
				);
			}

			if (lpHeader->SubChunk1Id[0] == 'f' || lpHeader->SubChunk1Id[1] == 'm' ||
				lpHeader->SubChunk1Id[2] == 't' || lpHeader->SubChunk1Id[3] == ' ')
			{
				if (lpHeader->AudioFormat != WAVE_FORMAT_PCM)
				{

					throw Exceptions::Exception(
						"Invalid file sub chunk"
					);
				}
			}

			WAVEFORMATEX format;
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nSamplesPerSec = lpHeader->SampleRate;
			format.wBitsPerSample = lpHeader->BitsPerSample;
			format.nChannels = lpHeader->NumChannels;
			format.nBlockAlign = (format.wBitsPerSample / 8) * format.nChannels;
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			format.cbSize = 0;

			DSBUFFERDESC desc;
			desc.dwSize = sizeof(DSBUFFERDESC);
			desc.dwFlags = DSBCAPS_CTRLVOLUME;
			desc.dwBufferBytes = (lpHeader->ChunkSize + 8) - sizeof(FileHeader);
			desc.dwReserved = 0;
			desc.lpwfxFormat = &format;
			desc.guid3DAlgorithm = GUID_NULL;

			HRESULT hResult;
			DirectSoundBuffer::Type* _buffer;

			if ((hResult = output->CreateSoundBuffer(&desc, &_buffer, nullptr)) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"IDirectSound8::CreateSoundBuffer",
					hResult
				);
			}

			auto lpBufferData = new uint8[desc.dwBufferBytes];

			memcpy(
				lpBufferData,
				reinterpret_cast<const void*>(
					reinterpret_cast<size_t>(lpBuffer) + sizeof(FileHeader)
				),
				desc.dwBufferBytes
			);

			LPVOID _bufferPtr;

			if ((hResult = _buffer->Lock(0, desc.dwBufferBytes, &_bufferPtr, &desc.dwBufferBytes, nullptr, nullptr, 0)) != DS_OK)
			{
				delete[] lpBufferData;
				_buffer->Release();

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::Lock",
					hResult
				);
			}

			memcpy(
				_bufferPtr,
				lpBufferData,
				desc.dwBufferBytes
			);

			delete[] lpBufferData;

			if ((hResult = _buffer->Unlock(_bufferPtr, desc.dwBufferBytes, nullptr, 0)) != DS_OK)
			{
				_buffer->Release();

				throw Exceptions::DirectSoundException(
					"IDirectSoundBuffer::Unlock",
					hResult
				);
			}

			buffer = _buffer;
		}

		// @throw AL::Exceptions::Exception
		static void EnumerateDevices(const DirectSoundDeviceEnumCallback& callback)
		{
			HRESULT hResult;

			if ((hResult = DirectSoundEnumerateA(&DirectSound::EnumDevicesProc, const_cast<DirectSoundDeviceEnumCallback*>(&callback))) != DS_OK)
			{

				throw Exceptions::DirectSoundException(
					"DirectSoundEnumerateA",
					hResult
				);
			}
		}

	private:
		// Returns TRUE to continue enumerating drivers, or FALSE to stop
		static BOOL CALLBACK EnumDevicesProc(LPGUID lpGuid, LPCSTR lpDescription, LPCSTR lpModule, LPVOID lpParam)
		{
			DirectSoundDevice device;
			device.Name = String(
				lpDescription
			);
			device.Module = String(
				lpModule
			);

			if (lpGuid)
			{
				AL::memcpy(
					&device.GUID,
					lpGuid,
					sizeof(device.GUID)
				);
			}

			auto lpCallback = reinterpret_cast<DirectSoundDeviceEnumCallback*>(
				lpParam
			);

			return (*lpCallback)(device) ? TRUE : FALSE;
		}
	};
}
