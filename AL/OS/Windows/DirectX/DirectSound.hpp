#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "Resource.hpp"
#include "DirectSoundException.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/File.hpp"

#if __has_include(<dsound.h>)
	#define AL_DEPENDENCY_DSOUND

	#include <MMSystem.h>
	#include <dsound.h>
#else
	typedef void IDirectSound8;
	typedef void IDirectSoundBuffer;
#endif

#if defined(AL_COMPILER_MSVC)
	#if defined(AL_DEPENDENCY_DSOUND)
		#pragma comment(lib, "winmm.lib")
		#pragma comment(lib, "dsound.lib")
		#pragma comment(lib, "windowscodecs.lib")
	#endif
#endif

namespace AL::OS::Windows::DirectX
{
	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using DirectSoundResource = Resource<T, TReleaser>;

	class DirectSoundBuffer
		: public DirectSoundResource<IDirectSoundBuffer>
	{
		Bool isPaused = False;
		Bool isPlaying = False;
		Bool isRepeating = False;

		uint32 pausePosition = 0;

	public:
		using DirectSoundResource<IDirectSoundBuffer>::DirectSoundResource;

		Bool IsPaused() const
		{
			return *this && isPaused;
		}

		Bool IsPlaying() const
		{
			return *this && isPlaying;
		}

		Bool IsRepeating() const
		{
			return *this && isRepeating;
		}

		// @throw AL::Exception
		auto GetVolume() const
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::LONG volume;
			::HRESULT hResult;

			if ((hResult = lpResource->GetVolume(&volume)) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::GetVolume",
					hResult
				);
			}

			return static_cast<float>(volume) / DSBVOLUME_MIN;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		auto GetPosition() const
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;
			::DWORD currentRead, currentWrite;

			if ((hResult = lpResource->GetCurrentPosition(&currentRead, &currentWrite)) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::GetCurrentPosition",
					hResult
				);
			}

			return static_cast<uint32>(
				currentRead
			);
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void SetVolume(float volume)
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			auto _volume = static_cast<::LONG>(
				DSBVOLUME_MIN - (DSBVOLUME_MIN * volume)
			);

			::HRESULT hResult;

			if ((hResult = lpResource->SetVolume(_volume)) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::SetVolume",
					hResult
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void SetPosition(uint32 position)
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;

			if ((hResult = lpResource->SetCurrentPosition(static_cast<::DWORD>(position))) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::SetCurrentPosition",
					hResult
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void Play(Bool repeat = False)
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

			if (IsPaused())
			{

				SetPosition(
					pausePosition
				);
			}

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;

			if ((hResult = lpResource->Play(0, 0, repeat ? DSBPLAY_LOOPING : 0)) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::Play",
					hResult
				);
			}
#else
			throw NotImplementedException();
#endif

			isPaused = False;
			isPlaying = True;
			isRepeating = repeat;
		}

		// @throw AL::Exception
		Void Pause()
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;
			pausePosition = GetPosition();

			if ((hResult = lpResource->Stop()) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::Stop",
					hResult
				);
			}
#else
			throw NotImplementedException();
#endif

			isPaused = True;
			isPlaying = False;
		}

		// @throw AL::Exception
		Void Stop()
		{
			auto lpResource = GetResource();

			AL_ASSERT(
				lpResource != nullptr,
				"Resource not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;

			if ((hResult = lpResource->Stop()) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSoundBuffer::Stop",
					hResult
				);
			}
#else
			throw NotImplementedException();
#endif

			isPaused = False;
			isPlaying = False;
			isRepeating = False;
		}
	};

	struct DirectSoundDevice
	{
		::GUID GUID;
		String Name;
		String Module;
	};

	typedef Function<bool(const DirectSoundDevice&)> DirectSoundDeviceEnumCallback;

	class DirectSound
	{
		typedef DirectSoundResource<IDirectSound8> OutputDevice;

		OutputDevice output;

		DirectSound(DirectSound&&) = delete;
		DirectSound(const DirectSound&) = delete;

	public:
		// @throw AL::Exception
		static Void EnumerateDevices(const DirectSoundDeviceEnumCallback& callback)
		{
#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;

			if ((hResult = ::DirectSoundEnumerateA(&DirectSound::EnumDevicesProc, const_cast<DirectSoundDeviceEnumCallback*>(&callback))) != DS_OK)
			{

				throw DirectSoundException(
					"DirectSoundEnumerateA",
					hResult
				);
			}
#else
			throw DependencyMissingException(
				"DSound"
			);
#endif
		}

		DirectSound()
		{
		}

		Bool IsCreated() const
		{
			return output.IsCreated();
		}

		// @throw AL::Exception
		Void Create(::HWND hWnd)
		{
			AL_ASSERT(
				!IsCreated(),
				"DirectSound already created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT           hResult;
			OutputDevice::Type* lpOutputDevice;

			if ((hResult = ::DirectSoundCreate8(nullptr, &lpOutputDevice, nullptr)) != DS_OK)
			{

				throw DirectSoundException(
					"DirectSoundCreate8",
					hResult
				);
			}

			if ((hResult = lpOutputDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) != DS_OK)
			{
				lpOutputDevice->Release();

				throw DirectSoundException(
					"IDirectSound8::SetCooperativeLevel",
					hResult
				);
			}

			output = lpOutputDevice;
#else
			throw DependencyMissingException(
				"DSound"
			);
#endif
		}
		// @throw AL::Exception
		Void Create(HWND hWnd, const DirectSoundDevice& device)
		{
			AL_ASSERT(
				!IsCreated(),
				"DirectSound already created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			::HRESULT hResult;
			OutputDevice::Type* lpOutputDevice;

			if ((hResult = ::DirectSoundCreate8(&device.GUID, &lpOutputDevice, nullptr)) != DS_OK)
			{

				throw DirectSoundException(
					"DirectSoundCreate8",
					hResult
				);
			}

			if ((hResult = lpOutputDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) != DS_OK)
			{
				lpOutputDevice->Release();

				throw DirectSoundException(
					"IDirectSound8::SetCooperativeLevel",
					hResult
				);
			}

			output = lpOutputDevice;
#else
			throw DependencyMissingException(
				"DSound"
			);
#endif
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				output.Release();
			}
		}

		// @throw AL::Exception
		Void CreateBuffer(DirectSoundBuffer& buffer, const FileSystem::Path& file)
		{
			AL_ASSERT(
				!IsCreated(),
				"DirectSound not created"
			);

			FileSystem::File _file(
				file
			);

			_file.Open(
				FileSystem::FileOpenModes::Read
			);

			try
			{
				if (auto fileSize = _file.GetSize())
				{
					Collections::Array<uint8> _buffer(
						static_cast<size_t>(fileSize)
					);

					try
					{
						_file.Read(
							&_buffer[0],
							_buffer.GetSize()
						);
					}
					catch (Exception& exception)
					{

						throw Exception(
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
			}
			catch (Exception&)
			{
				_file.Close();

				throw;
			}

			_file.Close();
		}
		// @throw AL::Exception
		Void CreateBuffer(DirectSoundBuffer& buffer, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				!IsCreated(),
				"DirectSound not created"
			);

#if defined(AL_DEPENDENCY_DSOUND)
			struct FileHeader
			{
				char   ChunkID[4];     // RIFF
				uint32 ChunkSize;      // 
				char   Format[4];      // WAVE

				char   SubChunk1Id[4]; // fmt | data
				uint32 SubChunk1Size;  // PCM == 16
				uint16 AudioFormat;    // PCM == 1
				uint16 NumChannels;    // Mono == 1 | Stereo = 2
				uint32 SampleRate;     // 44100
				uint32 ByteRate;       // SampleRate * NumChannels * BitsPerSample / 8
				uint16 BlockAlign;     // NumChannels * BitsPerSample / 8
				uint16 BitsPerSample;  // 8/16/32/64

				char   SubChunk2Id[4]; // data
				uint32 SubChunk2Size;  // NumSamples * NumChannels * BitsPerSample / 8
			};

			static_assert(
				sizeof(FileHeader) == 44,
				"Invalid FileHeader size"
			);

			if (size < sizeof(FileHeader))
			{

				throw Exception(
					"Buffer too small"
				);
			}

			auto lpHeader = reinterpret_cast<const FileHeader*>(lpBuffer);

			if (lpHeader->Format[0] != 'W' || lpHeader->Format[1] != 'A' ||
				lpHeader->Format[2] != 'V' || lpHeader->Format[3] != 'E')
			{

				throw Exception(
					"Invalid file format"
				);
			}

			if (lpHeader->ChunkID[0] != 'R' || lpHeader->ChunkID[1] != 'I' ||
				lpHeader->ChunkID[2] != 'F' || lpHeader->ChunkID[3] != 'F')
			{

				throw Exception(
					"Invalid file chunk"
				);
			}

			if (lpHeader->SubChunk1Id[0] == 'f' || lpHeader->SubChunk1Id[1] == 'm' ||
				lpHeader->SubChunk1Id[2] == 't' || lpHeader->SubChunk1Id[3] == ' ')
			{
				if (lpHeader->AudioFormat != WAVE_FORMAT_PCM)
				{

					throw Exception(
						"Invalid file sub chunk"
					);
				}
			}

			::WAVEFORMATEX format;
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nSamplesPerSec = lpHeader->SampleRate;
			format.wBitsPerSample = lpHeader->BitsPerSample;
			format.nChannels = lpHeader->NumChannels;
			format.nBlockAlign = (format.wBitsPerSample / 8) * format.nChannels;
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			format.cbSize = 0;

			::DSBUFFERDESC desc;
			desc.dwSize = sizeof(::DSBUFFERDESC);
			desc.dwFlags = DSBCAPS_CTRLVOLUME;
			desc.dwBufferBytes = (lpHeader->ChunkSize + 8) - sizeof(FileHeader);
			desc.dwReserved = 0;
			desc.lpwfxFormat = &format;
			desc.guid3DAlgorithm = GUID_NULL;

			::HRESULT hResult;
			DirectSoundBuffer::Type* _buffer;

			if ((hResult = output->CreateSoundBuffer(&desc, &_buffer, nullptr)) != DS_OK)
			{

				throw DirectSoundException(
					"IDirectSound8::CreateSoundBuffer",
					hResult
				);
			}

			auto lpBufferData = new uint8[desc.dwBufferBytes];

			memcpy(
				lpBufferData,
				reinterpret_cast<const Void*>(
					reinterpret_cast<size_t>(lpBuffer) + sizeof(FileHeader)
				),
				desc.dwBufferBytes
			);

			::LPVOID _bufferPtr;

			if ((hResult = _buffer->Lock(0, desc.dwBufferBytes, &_bufferPtr, &desc.dwBufferBytes, nullptr, nullptr, 0)) != DS_OK)
			{
				delete[] lpBufferData;
				_buffer->Release();

				throw DirectSoundException(
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

				throw DirectSoundException(
					"IDirectSoundBuffer::Unlock",
					hResult
				);
			}

			buffer = _buffer;
#else
			throw NotImplementedException();
#endif
		}

	private:
#if defined(AL_DEPENDENCY_DSOUND)
		// Return FALSE to stop enumerating drivers
		static ::BOOL CALLBACK EnumDevicesProc(::LPGUID lpGuid, ::LPCSTR lpDescription, ::LPCSTR lpModule, ::LPVOID lpParam)
		{
			DirectSoundDevice device;
			device.Name.Assign(
				lpDescription
			);
			device.Module.Assign(
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
#endif
	};
}
