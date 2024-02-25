#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include <cstdio>

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/stat.h>
	#include <sys/types.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <fileapi.h>
#else
	#error Platform not supported
#endif

namespace AL::FileSystem
{
	enum class FileOpenModes : uint8
	{
		Binary   = 0x1,

		Read     = 0x2,
		Write    = 0x4,

		Append   = 0x8,
		Truncate = 0x10
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(FileOpenModes);

	class File
	{
		Bool     isOpen = False;

#if defined(AL_PLATFORM_LINUX)
		FILE*    lpFile;
#elif defined(AL_PLATFORM_WINDOWS)
		::HANDLE hFile;
#endif

		Path     path;
		uint64   readPosition = 0;
		uint64   writePosition = 0;

		File(const File&) = delete;

	public:
		// @throw AL::Exception
		static uint64 GetSize(const Path& path)
		{
			auto fileSize = GetSize(
				path.GetString()
			);

			return fileSize;
		}
		// @throw AL::Exception
		static uint64 GetSize(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat64 st;

			if (::stat64(path.GetCString(), &st) == -1)
			{

				throw OS::SystemException(
					"stat64"
				);
			}

			return static_cast<uint64>(
				st.st_size
			);
#elif defined(AL_PLATFORM_WINDOWS)
			::WIN32_FILE_ATTRIBUTE_DATA fileInfo;

			if (!::GetFileAttributesExA(path.GetCString(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &fileInfo))
			{

				throw OS::SystemException(
					"GetFileAttributesExA"
				);
			}

			Integer<uint64> integer;
			integer.Low.Value  = fileInfo.nFileSizeLow;
			integer.High.Value = fileInfo.nFileSizeHigh;

			return integer.Value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		static Bool Exists(const Path& path)
		{
			if (!Exists(path.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		static Bool Exists(const String& path)
		{
			if (!Path::Exists(path))
			{

				return False;
			}

			if (!Path::IsFile(path))
			{
				
				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Create(const Path& path)
		{
			if (!Create(path.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Create(const String& path)
		{
			if (Path::Exists(path))
			{

				return False;
			}

#if defined(AL_PLATFORM_LINUX)
			if (::creat(path.GetCString(), S_IRUSR | S_IWUSR) == -1)
			{

				throw OS::SystemException(
					"creat"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::HANDLE hFile;

			if ((hFile = ::CreateFileA(path.GetCString(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr)) == INVALID_HANDLE_VALUE)
			{

				throw OS::SystemException(
					"CreateFileA"
				);
			}

			::CloseHandle(
				hFile
			);
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		static Void Delete(const Path& path)
		{
			Delete(
				path.GetString()
			);
		}
		// @throw AL::Exception
		static Void Delete(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			if (::unlink(path.GetCString()) == -1)
			{

				throw OS::SystemException(
					"unlink"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::DeleteFileA(path.GetCString()) == FALSE)
			{

				throw OS::SystemException(
					"DeleteFileA"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Copy(const Path& source, const Path& destination)
		{
			if (!Copy(source.GetString(), destination.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Copy(const String& source, const String& destination)
		{
			if (Path::Exists(destination))
			{

				return False;
			}

			File sourceFile(
				source
			);

			try
			{
				if (!sourceFile.Open(FileOpenModes::Read | FileOpenModes::Binary))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					AL::Move(exception),
					"Error opening source file"
				);
			};

			File destinationFile(
				destination
			);

			try
			{
				destinationFile.Open(
					FileOpenModes::Write | FileOpenModes::Truncate | FileOpenModes::Binary
				);
			}
			catch (Exception& exception)
			{
				sourceFile.Close();

				throw Exception(
					AL::Move(exception),
					"Error opening destination file"
				);
			};

			static constexpr uint16 BUFFER_SIZE = 0x400;

			auto   lpBuffer   = new uint8[BUFFER_SIZE];
			size_t bufferSize = 0;

			try
			{
				while ((bufferSize = sourceFile.Read(lpBuffer, BUFFER_SIZE)) != 0)
				{
					for (size_t i = 0; i < bufferSize; )
					{
						i += destinationFile.Write(
							lpBuffer,
							bufferSize - i
						);
					}
				}
			}
			catch (Exception& exception)
			{
				delete[] lpBuffer;

				sourceFile.Close();
				destinationFile.Close();

				throw Exception(
					AL::Move(exception),
					"Error copying %u bytes",
					BUFFER_SIZE
				);
			}

			delete[] lpBuffer;

			sourceFile.Close();
			destinationFile.Close();

			return True;
		}
		
		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Move(const Path& source, const Path& destination)
		{
			if (!Move(source.GetString(), destination.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if already exists
		static Bool Move(const String& source, const String& destination)
		{
			if (Path::Exists(destination))
			{

				return False;
			}

#if defined(AL_PLATFORM_LINUX)
			if (::std::rename(source.GetCString(), destination.GetCString()) == -1)
			{

				throw OS::SystemException(
					"rename"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::MoveFileA(source.GetCString(), destination.GetCString()) == FALSE)
			{

				throw OS::SystemException(
					"MoveFileA"
				);
			}
#else
			throw NotImplementedException();
#endif

			return True;
		}

		File(File&& file)
			: isOpen(
				file.isOpen
			),
#if defined(AL_PLATFORM_LINUX)
			lpFile(
				file.lpFile
			),
#elif defined(AL_PLATFORM_WINDOWS)
			hFile(
				file.hFile
			),
#endif
			path(
				AL::Move(file.path)
			),
			readPosition(
				file.readPosition
			),
			writePosition(
				file.writePosition
			)
		{
			file.isOpen = False;
		}

		explicit File(Path&& path)
			: path(
				AL::Move(path)
			)
		{
		}
		explicit File(const Path& path)
			: File(
				Path(path)
			)
		{
		}

		explicit File(String&& path)
			: File(
				Path(
					AL::Move(path)
				)
			)
		{
		}
		explicit File(const String& path)
			: File(
				Path(path)
			)
		{
		}

		virtual ~File()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		const Path& GetPath() const
		{
			return path;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)
			return lpFile;
#elif defined(AL_PLATFORM_WINDOWS)
			return hFile;
#endif
		}

		// @throw AL::Exception
		uint64 GetSize() const
		{
			auto fileSize = GetSize(
				GetPath()
			);

			return fileSize;
		}

		uint64 GetReadPosition() const
		{
			return readPosition;
		}

		uint64 GetWritePosition() const
		{
			return writePosition;
		}

		// @throw AL::Exception
		Bool Exists() const
		{
			if (!Exists(GetPath()))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Void Delete()
		{
			Delete(
				GetPath()
			);
		}

		// @throw AL::Exception
		Void Copy(const Path& path) const
		{
			Copy(
				path.GetString()
			);
		}
		// @throw AL::Exception
		Void Copy(const String& path) const
		{
			Copy(
				GetPath().GetString(),
				path
			);
		}

		// @throw AL::Exception
		// @return AL::False if not found
		Bool Open(FileOpenModes mode)
		{
			AL_ASSERT(
				!IsOpen(),
				"File already open"
			);

			typedef BitMask<FileOpenModes> FileOpenModeMask;

			AL_ASSERT(
				!(FileOpenModeMask::IsSet(mode, FileOpenModes::Append) && FileOpenModeMask::IsSet(mode, FileOpenModes::Truncate)),
				"FileOpenModes::Append and FileOpenModes::Truncate are exclusive"
			);

#if defined(AL_PLATFORM_LINUX)
			char _mode[4] = { 0 };

			if (FileOpenModeMask::IsSet(mode, FileOpenModes::Read | FileOpenModes::Write))
			{
				if (FileOpenModeMask::IsSet(mode, FileOpenModes::Append))
				{
					_mode[0] = 'a';
					_mode[1] = '+';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[2] = 'b';
					}
				}
				else if (FileOpenModeMask::IsSet(mode, FileOpenModes::Truncate))
				{
					_mode[0] = 'w';
					_mode[1] = '+';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[2] = 'b';
					}
				}
				else
				{
					_mode[0] = 'r';
					_mode[1] = '+';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[2] = 'b';
					}
				}
			}
			else if (FileOpenModeMask::IsSet(mode, FileOpenModes::Read))
			{
				_mode[0] = 'r';

				if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
				{
					_mode[1] = 'b';
				}
			}
			else if (FileOpenModeMask::IsSet(mode, FileOpenModes::Write))
			{
				if (FileOpenModeMask::IsSet(mode, FileOpenModes::Append))
				{
					_mode[0] = 'a';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[1] = 'b';
					}
				}
				else if (FileOpenModeMask::IsSet(mode, FileOpenModes::Truncate))
				{
					_mode[0] = 'w';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[1] = 'b';
					}
				}
				else
				{
					_mode[0] = 'a';

					if (FileOpenModeMask::IsSet(mode, FileOpenModes::Binary))
					{
						_mode[1] = 'b';
					}

					mode = FileOpenModeMask::Add(
						mode,
						FileOpenModes::Truncate
					);
				}
			}

			if ((lpFile = ::std::fopen(GetPath().GetString().GetCString(), _mode)) == nullptr)
			{
				auto errorCode = OS::GetLastError();

				if (errorCode == ENOENT)
				{

					return False;
				}

				throw OS::SystemException(
					"fopen",
					errorCode
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			BitMask<::DWORD> access;
			access.Set(FILE_GENERIC_READ,  FileOpenModeMask::IsSet(mode, FileOpenModes::Read));
			access.Set(FILE_GENERIC_WRITE, FileOpenModeMask::IsSet(mode, FileOpenModes::Write));

			if ((hFile = ::CreateFileA(GetPath().GetString().GetCString(), access.Value, FILE_SHARE_READ, nullptr, FileOpenModeMask::IsSet(mode, FileOpenModes::Truncate) ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			{
				auto errorCode = OS::GetLastError();

				if (errorCode == ERROR_FILE_NOT_FOUND)
				{

					return False;
				}

				throw OS::SystemException(
					"CreateFileA",
					errorCode
				);
			}
#else
			throw NotImplementedException();
#endif

			if (FileOpenModeMask::IsSet(mode, FileOpenModes::Append))
			{
				try
				{
					if (auto fileSize = GetSize())
					{

						SetWritePosition(
							fileSize
						);
					}
				}
				catch (Exception&)
				{
					Close();

					throw;
				}
			}

			isOpen = True;

			return True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				::std::fclose(
					GetHandle()
				);
#elif defined(AL_PLATFORM_WINDOWS)
				::CloseHandle(
					GetHandle()
				);
#endif

				isOpen = False;
			}
		}

		Void SetReadPosition(uint64 value)
		{
			if (auto fileSize = GetSize())
			{
				if (value > fileSize)
				{
					
					value = fileSize;
				}

				readPosition = value;
			}
		}

		Void SetWritePosition(uint64 value)
		{
			if (auto fileSize = GetSize())
			{
				if (value > fileSize)
				{
					
					value = fileSize;
				}

				writePosition = value;
			}
		}

		// @throw AL::Exception
		size_t Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"File not open"
			);

			size_t bytesRead = 0;

#if defined(AL_PLATFORM_LINUX)
			if (::std::fseek(GetHandle(), static_cast<long>(GetReadPosition()), SEEK_SET) == -1)
			{

				throw OS::SystemException(
					"fseek"
				);
			}

			if ((bytesRead = ::std::fread(lpBuffer, 1, size, GetHandle())) == 0)
			{
				if (!::feof(GetHandle()))
				{

					throw OS::SystemException(
						"fread"
					);
				}
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER li;

			if (!::SetFilePointerEx(GetHandle(), { .QuadPart = static_cast<::LONGLONG>(GetReadPosition()) }, &li, FILE_BEGIN))
			{

				throw OS::SystemException(
					"SetFilePointerEx"
				);
			}

			for (::DWORD numberOfBytesRead = 0; bytesRead < size; bytesRead += numberOfBytesRead)
			{
				auto numberOfBytesToRead = size - bytesRead;

				if (numberOfBytesToRead > Integer<::DWORD>::Maximum)
					numberOfBytesToRead = Integer<::DWORD>::Maximum;

				if (!::ReadFile(GetHandle(), &reinterpret_cast<uint8*>(lpBuffer)[bytesRead], static_cast<::DWORD>(numberOfBytesToRead), &numberOfBytesRead, nullptr))
				{

					throw OS::SystemException(
						"ReadFile"
					);
				}

				if (numberOfBytesRead == 0)
				{
					if (bytesRead == 0)
					{

						return False;
					}

					break;
				}
			}
#else
			throw NotImplementedException();
#endif

			readPosition += bytesRead;
				
			return bytesRead;
		}

		// @throw AL::Exception
		size_t Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"File not open"
			);

			size_t bytesWritten = 0;

#if defined(AL_PLATFORM_LINUX)
			if (::std::fseek(GetHandle(), static_cast<long>(GetWritePosition()), SEEK_SET) == -1)
			{

				throw OS::SystemException(
					"fseek"
				);
			}

			if ((bytesWritten = ::std::fwrite(lpBuffer, 1, size, GetHandle())) == 0)
			{

				throw OS::SystemException(
					"fwrite"
				);
			}

			if (::std::fflush(GetHandle()) == EOF)
			{

				throw OS::SystemException(
					"fflush"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::LARGE_INTEGER li;

			if (!::SetFilePointerEx(GetHandle(), { .QuadPart = static_cast<::LONGLONG>(GetWritePosition()) }, &li, FILE_BEGIN))
			{

				throw OS::SystemException(
					"SetFilePointerEx"
				);
			}

			for (::DWORD numberOfBytesWritten = 0; bytesWritten < size; bytesWritten += numberOfBytesWritten)
			{
				auto numberOfBytesToWrite = size - bytesWritten;

				if (numberOfBytesToWrite > Integer<::DWORD>::Maximum)
					numberOfBytesToWrite = Integer<::DWORD>::Maximum;

				if (!::WriteFile(GetHandle(), &reinterpret_cast<const uint8*>(lpBuffer)[bytesWritten], static_cast<::DWORD>(numberOfBytesToWrite), &numberOfBytesWritten, nullptr))
				{

					throw OS::SystemException(
						"WriteFile"
					);
				}
			}
#else
			throw NotImplementedException();
#endif

			writePosition += bytesWritten;

			return bytesWritten;
		}

		File& operator = (File&& file)
		{
			Close();

			isOpen      = file.isOpen;
			file.isOpen = False;

#if defined(AL_PLATFORM_LINUX)
			lpFile = file.lpFile;
#elif defined(AL_PLATFORM_WINDOWS)
			hFile = file.hFile;
#endif

			path = AL::Move(
				file.path
			);

			readPosition  = file.readPosition;
			writePosition = file.writePosition;

			return *this;
		}

		Bool operator == (const File& file) const
		{
			if (IsOpen() != file.IsOpen())
			{

				return False;
			}

			if (IsOpen() && (GetHandle() != file.GetHandle()))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const File& file) const
		{
			if (operator==(file))
			{

				return False;
			}

			return True;
		}
	};
}
