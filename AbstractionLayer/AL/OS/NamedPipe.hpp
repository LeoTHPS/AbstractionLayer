#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::OS
{
	class NamedPipe
	{
		String name;
		HANDLE handle;
		
		NamedPipe(NamedPipe&&) = delete;
		NamedPipe(const NamedPipe&) = delete;

	public:
		explicit NamedPipe(String&& name)
			: name(
				Move(name)
			),
			handle(
				INVALID_HANDLE_VALUE
			)
		{
		}

		explicit NamedPipe(const String& name)
			: NamedPipe(
				String(name)
			)
		{
		}

		bool IsOpen() const
		{
			return handle != INVALID_HANDLE_VALUE;
		}

		auto& GetName() const
		{
			return name;
		}

		auto GetHandle() const
		{
			return handle;
		}

		// @throw AL::Exceptions::Exception
		// @return false on ERROR_PIPE_BUSY
		bool Connect()
		{
			AL_ASSERT(!IsOpen(), "NamedPipe already open");

			if ((handle = CreateFileA(
				GetName().GetCString(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				nullptr,
				OPEN_EXISTING,
				0,
				nullptr
			)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateFileA"
				);
			}

			else if (auto lastError = GetLastError())
			{
				handle = INVALID_HANDLE_VALUE;

				if (lastError == ERROR_PIPE_BUSY)
				{

					return false;
				}

				throw Exceptions::SystemException(
					"CreateFileA",
					lastError
				);
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool Accept(size_t writeBufferSize, size_t readBufferSize)
		{
			return Accept(
				writeBufferSize,
				readBufferSize,
				TimeSpan::FromMilliseconds(
					NMPWAIT_USE_DEFAULT_WAIT
				)
			);
		}
		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool Accept(size_t writeBufferSize, size_t readBufferSize, TimeSpan waitTime)
		{
			AL_ASSERT(!IsOpen(), "NamedPipe already open");

			if ((handle = CreateNamedPipeA(
				GetName().GetCString(),
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_REJECT_REMOTE_CLIENTS | PIPE_NOWAIT,
				PIPE_UNLIMITED_INSTANCES,
				static_cast<DWORD>(writeBufferSize),
				static_cast<DWORD>(readBufferSize),
				static_cast<DWORD>(waitTime.ToMilliseconds()),
				nullptr
			)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateNamedPipeA"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool AcceptAny(size_t writeBufferSize, size_t readBufferSize)
		{
			return AcceptAny(
				writeBufferSize,
				readBufferSize,
				TimeSpan::FromMilliseconds(
					NMPWAIT_USE_DEFAULT_WAIT
				)
			);
		}
		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool AcceptAny(size_t writeBufferSize, size_t readBufferSize, TimeSpan waitTime)
		{
			AL_ASSERT(!IsOpen(), "NamedPipe already open");

			if ((handle = CreateNamedPipeA(
				GetName().GetCString(),
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
				PIPE_UNLIMITED_INSTANCES,
				static_cast<DWORD>(writeBufferSize),
				static_cast<DWORD>(readBufferSize),
				static_cast<DWORD>(waitTime.ToMilliseconds()),
				nullptr
			)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateNamedPipeA"
				);
			}
		}

		void Close()
		{
			if (IsOpen())
			{
				CloseHandle(handle);
				handle = INVALID_HANDLE_VALUE;
			}
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		size_t Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "NamedPipe not open");
			
			DWORD bytesAvailable = 0;

			if (!PeekNamedPipe(GetHandle(), nullptr, 0, nullptr, &bytesAvailable, nullptr))
			{

				throw Exceptions::SystemException(
					"PeekNamedPipe"
				);
			}

			bytesAvailable = Math::Clamp<DWORD>(
				bytesAvailable,
				0,
				static_cast<DWORD>(size)
			);

			DWORD bytesRead = 0;

			if (!ReadFile(GetHandle(), lpBuffer, bytesAvailable, &bytesRead, nullptr))
			{
				switch (auto errorCode = GetLastError())
				{
					case ERROR_NO_DATA:
						break;

					default:
						throw Exceptions::SystemException(
							"ReadFile",
							errorCode
						);
						break;
				}
			}

			return bytesRead;
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes written
		size_t Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "NamedPipe not open");

			DWORD bytesWritten = 0;

			if (!WriteFile(GetHandle(), lpBuffer, static_cast<DWORD>(size), &bytesWritten, nullptr))
			{

				throw Exceptions::SystemException(
					"WriteFile"
				);
			}

			return bytesWritten;
		}
	};
}
