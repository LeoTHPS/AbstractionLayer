#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
{
	class NamedPipe
	{
		String   name;
		::HANDLE handle;

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

		Bool IsOpen() const
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

		// @throw AL::Exception
		// @return AL::False on ERROR_PIPE_BUSY
		Bool Connect()
		{
			AL_ASSERT(
				!IsOpen(),
				"NamedPipe already open"
			);

			if ((handle = ::CreateFileA(GetName().GetCString(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr)) == INVALID_HANDLE_VALUE)
			{
				auto errorCode = GetLastError();

				switch (errorCode)
				{
					case ERROR_PIPE_BUSY:
					case ERROR_FILE_NOT_FOUND:
						return False;
				}

				throw SystemException(
					"CreateFileA",
					errorCode
				);
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Accept(size_t writeBufferSize, size_t readBufferSize)
		{
			auto waitTime = TimeSpan::FromMilliseconds(
				NMPWAIT_USE_DEFAULT_WAIT
			);

			if (!Accept(writeBufferSize, readBufferSize, waitTime))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Accept(size_t writeBufferSize, size_t readBufferSize, TimeSpan waitTime)
		{
			AL_ASSERT(
				!IsOpen(),
				"NamedPipe already open"
			);

			if ((handle = ::CreateNamedPipeA(GetName().GetCString(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_REJECT_REMOTE_CLIENTS | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, static_cast<::DWORD>(writeBufferSize), static_cast<::DWORD>(readBufferSize), static_cast<::DWORD>(waitTime.ToMilliseconds()), nullptr)) == INVALID_HANDLE_VALUE)
			{

				throw SystemException(
					"CreateNamedPipeA"
				);
			}
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool AcceptAny(size_t writeBufferSize, size_t readBufferSize)
		{
			auto waitTime = TimeSpan::FromMilliseconds(
				NMPWAIT_USE_DEFAULT_WAIT
			);

			if (!AcceptAny(writeBufferSize, readBufferSize, waitTime))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		Bool AcceptAny(size_t writeBufferSize, size_t readBufferSize, TimeSpan waitTime)
		{
			AL_ASSERT(
				!IsOpen(),
				"NamedPipe already open"
			);

			if ((handle = ::CreateNamedPipeA(GetName().GetCString(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, static_cast<::DWORD>(writeBufferSize), static_cast<::DWORD>(readBufferSize), static_cast<::DWORD>(waitTime.ToMilliseconds()), nullptr)) == INVALID_HANDLE_VALUE)
			{

				throw SystemException(
					"CreateNamedPipeA"
				);
			}
		}

		Void Close()
		{
			if (IsOpen())
			{
				::CloseHandle(
					handle
				);

				handle = INVALID_HANDLE_VALUE;
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Read(Void* lpBuffer, size_t size, size_t& numberOfBytesRead)
		{
			AL_ASSERT(
				IsOpen(),
				"NamedPipe not open"
			);

			::DWORD bytesAvailable = 0;

			if (!::PeekNamedPipe(GetHandle(), nullptr, 0, nullptr, &bytesAvailable, nullptr))
			{
				auto errorCode = GetLastError();

				switch (errorCode)
				{
					case ERROR_BROKEN_PIPE:
					case ERROR_INVALID_HANDLE:
						Close();
						return False;
				}

				throw SystemException(
					"PeekNamedPipe",
					errorCode
				);
			}

			::DWORD bytesRead = 0;

			if ((bytesAvailable = Math::Clamp<::DWORD>(bytesAvailable, 0, static_cast<::DWORD>(size))) != 0)
			{
				if (!::ReadFile(GetHandle(), lpBuffer, bytesAvailable, &bytesRead, nullptr))
				{
					switch (auto errorCode = GetLastError())
					{
						case ERROR_NO_DATA:
							break;

						case ERROR_BROKEN_PIPE:
						case ERROR_INVALID_HANDLE:
							Close();
							return False;

						default:
							throw SystemException(
								"ReadFile",
								errorCode
							);
							break;
					}
				}
			}

			numberOfBytesRead = bytesRead;

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Write(const Void* lpBuffer, size_t size, size_t& numberOfBytesWritten)
		{
			AL_ASSERT(
				IsOpen(),
				"NamedPipe not open"
			);

			::DWORD bytesWritten = 0;

			if (!::WriteFile(GetHandle(), lpBuffer, static_cast<::DWORD>(size), &bytesWritten, nullptr))
			{
				auto errorCode = GetLastError();

				switch (errorCode)
				{
					case ERROR_BROKEN_PIPE:
					case ERROR_INVALID_HANDLE:
						Close();
						return False;
				}

				throw SystemException(
					"WriteFile"
				);
			}

			numberOfBytesWritten = bytesWritten;

			return True;
		}
	};
}
