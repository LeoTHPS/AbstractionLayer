#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include <cstdio>

#if defined(AL_PLATFOMR_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/stat.h>
	#include <sys/types.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <fileapi.h>
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
		Bool   isOpen = False;

		FILE*  lpFile;

		Path   path;
		uint64 readPosition = 0;
		uint64 writePosition = 0;

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
			throw PlatformNotSupportedException();
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
		// @return False if already exists
		static Bool Create(const Path& path)
		{
			if (!Create(path.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return False if already exists
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
			throw PlatformNotSupportedException();
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
			throw PlatformNotSupportedException();
#endif
		}

		// @throw AL::Exception
		// @return False if already exists
		static Bool Copy(const Path& source, const Path& destination)
		{
			if (!Copy(source.GetString(), destination.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return False if already exists
		static Bool Copy(const String& source, const String& destination)
		{
			if (Path::Exists(destination))
			{

				return False;
			}

			::FILE* lpSource;

			if ((lpSource = ::std::fopen(source.GetCString(), "rb")) == NULL)
			{

				throw OS::SystemException(
					"fopen"
				);
			}

			::FILE* lpDestination;

			if ((lpDestination = ::std::fopen(destination.GetCString(), "wb")) == NULL)
			{
				::std::fclose(
					lpSource
				);

				throw OS::SystemException(
					"fopen"
				);
			}

			static constexpr size_t BUFFER_SIZE = 0x400;

			auto lpBuffer = new uint8[BUFFER_SIZE];

			while (!::std::feof(lpSource))
			{
				size_t bytesRead;

				if ((bytesRead = ::std::fread(lpBuffer, 1, BUFFER_SIZE, lpSource)) == 0)
				{
					if (!::std::feof(lpSource))
					{
						delete[] lpBuffer;

						::std::fclose(
							lpDestination
						);
							
						::std::fclose(
							lpSource
						);

						throw OS::SystemException(
							"fread"
						);
					}
				}

				if (::std::fwrite(lpBuffer, 1, bytesRead, lpDestination) == 0)
				{
					delete[] lpBuffer;

					::std::fclose(
						lpDestination
					);
						
					::std::fclose(
						lpSource
					);

					throw OS::SystemException(
						"fwrite"
					);
				}

				::std::clearerr(
					lpDestination
				);
			}

			delete[] lpBuffer;

			if (::std::fflush(lpDestination) == EOF)
			{
				::std::fclose(
					lpDestination
				);
					
				::std::fclose(
					lpSource
				);

				throw OS::SystemException(
					"fflush"
				);
			}

			::std::fclose(
				lpDestination
			);

			::std::fclose(
				lpSource
			);

			return True;
		}
		
		// @throw AL::Exception
		// @return False if already exists
		static Bool Move(const Path& source, const Path& destination)
		{
			if (!Move(source.GetString(), destination.GetString()))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return False if already exists
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
			throw PlatformNotSupportedException();
#endif

			return True;
		}

		File(File&& file)
			: isOpen(
				file.isOpen
			),
			lpFile(
				file.lpFile
			),
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
		// @return False if not found
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

			return True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::std::fclose(
					lpFile
				);

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

			if (::std::fseek(lpFile, static_cast<long>(GetReadPosition()), SEEK_SET) == -1)
			{

				throw OS::SystemException(
					"fseek"
				);
			}

			size_t bytesRead;

			if ((bytesRead = ::std::fread(lpBuffer, 1, size, lpFile)) == 0)
			{
				if (!::feof(lpFile))
				{

					throw OS::SystemException(
						"fread"
					);
				}
			}

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

			if (::std::fseek(lpFile, static_cast<long>(GetWritePosition()), SEEK_SET) == -1)
			{

				throw OS::SystemException(
					"fseek"
				);
			}

			size_t bytesWritten;

			if ((bytesWritten = ::std::fwrite(lpBuffer, 1, size, lpFile)) == 0)
			{

				throw OS::SystemException(
					"fwrite"
				);
			}

			if (::std::fflush(lpFile) == EOF)
			{

				throw OS::SystemException(
					"fflush"
				);
			}

			writePosition += bytesWritten;

			return bytesWritten;
		}

		File& operator = (File&& file)
		{
			Close();

			isOpen = file.isOpen;
			file.isOpen = False;

			lpFile = file.lpFile;

			path = AL::Move(
				file.path
			);

			readPosition = file.readPosition;
			writePosition = file.writePosition;

			return *this;
		}

		Bool operator == (const File& file) const;
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
