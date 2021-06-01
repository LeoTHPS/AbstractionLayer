#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

#include <sys/stat.h>

#if defined(AL_PLATFORM_LINUX)
	#include <cstdio>

	#include <fcntl.h>
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
#if defined(AL_PLATFORM_LINUX)
		typedef FILE* HFILE;

		static constexpr HFILE INVALID_FILE_HANDLE_VALUE = nullptr;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef HANDLE HFILE;

		static constexpr HFILE INVALID_FILE_HANDLE_VALUE = INVALID_HANDLE_VALUE;
#endif
		
		Path path;
		HFILE hFile = INVALID_FILE_HANDLE_VALUE;

		uint64 readPosition = 0;
		uint64 writePosition = 0;

		File(const File&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		static uint64 GetSize(const Path& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat64 st;

			if (stat64(path.GetString().GetCString(), &st) == -1)
			{

				throw Exceptions::SystemException(
					"stat64"
				);
			}

			return static_cast<uint64>(
				st.st_size
			);
#elif defined(AL_PLATFORM_WINDOWS)
			struct _stat64 st;

			if (_stat64(path.GetString().GetCString(), &st) == -1)
			{

				throw Exceptions::SystemException(
					"_stat64"
				);
			}

			return static_cast<uint64>(
				st.st_size
			);
#endif
		}
		
		// @throw AL::Exceptions::Exception
		static bool Exists(const Path& path)
		{
			return path.Exists() && path.IsFile();
		}

		// @throw AL::Exceptions::Exception
		static void Move(const Path& source, const Path& destination, bool overwrite = false)
		{
#if defined(AL_PLATFORM_LINUX)
			if (rename(source.GetString().GetCString(), destination.GetString().GetCString()) == -1)
			{

				throw Exceptions::SystemException(
					"rename"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (MoveFileA(source.GetString().GetCString(), destination.GetString().GetCString()) == FALSE)
			{

				throw Exceptions::SystemException(
					"MoveFileA"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Copy(const Path& source, const Path& destination, bool overwrite = false)
		{
#if defined(AL_PLATFORM_LINUX)
			FILE* hSource;

			if ((hSource = fopen(source.GetString().GetCString(), "r")) == NULL)
			{

				throw Exceptions::SystemException(
					"fopen"
				);
			}

			FILE* hDestination;

			if ((hDestination = fopen(destination.GetString().GetCString(), "w")) == NULL)
			{
				fclose(hSource);

				throw Exceptions::SystemException(
					"fopen"
				);
			}

			uint8 ch;

			while ((ch = fgetc(hSource)) != EOF)
			{
				if (fputc(ch, hDestination) == EOF)
				{
					fclose(hDestination);
					fclose(hSource);

					throw Exceptions::SystemException(
						"fputc"
					);
				}
			}

			fclose(hDestination);
			fclose(hSource);

#elif defined(AL_PLATFORM_WINDOWS)
			if (CopyFileA(source.GetString().GetCString(), destination.GetString().GetCString(), !overwrite ? TRUE : FALSE) == FALSE)
			{

				throw Exceptions::SystemException(
					"CopyFileA"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Create(const Path& path, bool overwrite = false)
		{
#if defined(AL_PLATFORM_LINUX)
			if (creat(path.GetString().GetCString(), S_IRUSR | S_IWUSR) == -1)
			{

				throw Exceptions::SystemException(
					"creat"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hFile;

			if ((hFile = CreateFileA(
				path.GetString().GetCString(),
				GENERIC_WRITE,
				0,
				nullptr,
				!overwrite ? CREATE_NEW : CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateFileA"
				);
			}

			CloseHandle(hFile);
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Delete(const Path& path)
		{
#if defined(AL_PLATFORM_LINUX)
			if (unlink(path.GetString().GetCString()) == -1)
			{

				throw Exceptions::SystemException(
					"unlink"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (DeleteFileA(path.GetString().GetCString()) == FALSE)
			{

				throw Exceptions::SystemException(
					"DeleteFileA"
				);
			}
#endif
		}

		File(Path&& path)
			: path(
				AL::Move(path)
			)
		{
		}

		File(const Path& path)
			: File(
				Path(path)
			)
		{
		}

		File(File&& file)
			: path(
				AL::Move(file.path)
			),
			hFile(
				file.hFile
			),
			readPosition(
				file.readPosition
			),
			writePosition(
				file.writePosition
			)
		{
			file.hFile = INVALID_FILE_HANDLE_VALUE;

			file.readPosition = 0;
			file.writePosition = 0;
		}

		virtual ~File()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hFile != INVALID_FILE_HANDLE_VALUE;
		}

		auto GetHandle() const
		{
			return hFile;
		}

		auto& GetPath() const
		{
			return path;
		}

		// @throw AL::Exceptions::Exception
		auto GetSize() const
		{
			AL_ASSERT(IsOpen(), "File not open");

#if defined(AL_PLATFORM_LINUX)
			return GetSize(
				GetPath()
			);
#elif defined(AL_PLATFORM_WINDOWS)
			LARGE_INTEGER li;

			if (GetFileSizeEx(GetHandle(), &li) == 0)
			{

				throw Exceptions::SystemException(
					"GetFileSizeEx"
				);
			}

			return static_cast<uint64>(
				li.QuadPart
			);
#endif
		}

		auto GetReadPosition() const
		{
			return readPosition;
		}

		auto GetWritePosition() const
		{
			return writePosition;
		}

		// @throw AL::Exceptions::Exception
		void SetReadPosition(uint64 index)
		{
			AL_ASSERT(IsOpen(), "File not open");

			if (auto fileSize = GetSize())
			{
				if (index > fileSize)
				{

					index = fileSize;
				}

#if defined(AL_PLATFORM_LINUX)
				if (fseek(hFile, static_cast<long>(index), SEEK_SET) == -1)
				{

					throw Exceptions::SystemException(
						"fseek"
					);
				}
#endif

				readPosition = index;
			}
		}

		// @throw AL::Exceptions::Exception
		void SetWritePosition(uint64 index)
		{
			AL_ASSERT(IsOpen(), "File not open");

			if (auto fileSize = GetSize())
			{
				if (index > fileSize)
				{

					index = fileSize;
				}

#if defined(AL_PLATFORM_LINUX)
				throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				LARGE_INTEGER _adjustment = { 0 };
				_adjustment.QuadPart = index;

				if (SetFilePointerEx(GetHandle(), _adjustment, nullptr, FILE_BEGIN) == 0)
				{

					throw Exceptions::SystemException(
						"SetFilePointerEx"
					);
				}
#endif

				writePosition = index;
			}
		}

		// @throw AL::Exceptions::Exception
		bool Exists() const
		{
			return Exists(
				GetPath()
			);
		}

		// @throw AL::Exceptions::Exception
		void Move(const Path& destination, bool overwrite = false)
		{
			Close();

			Move(
				GetPath(),
				destination,
				overwrite
			);
		}

		// @throw AL::Exceptions::Exception
		void Copy(const Path& destination, bool overwrite = false) const
		{
			Copy(
				GetPath(),
				destination,
				overwrite
			);
		}

		// @throw AL::Exceptions::Exception
		void Create(bool overwrite = false)
		{
			Create(
				GetPath(),
				overwrite
			);
		}

		// @throw AL::Exceptions::Exception
		void Delete()
		{
			Close();

			Delete(
				GetPath()
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Open(BitMask<FileOpenModes> mode)
		{
			AL_ASSERT(!IsOpen(), "File already open");

#if defined(AL_PLATFORM_LINUX)
			char _mode[3] = { 0 };

			// w/w+
			if(mode.IsSet(FileOpenModes::Truncate))
			{
				_mode[0] = 'w';

				if (mode.IsSet(FileOpenModes::Read))
				{

					_mode[1] = '+';
				}
			}
			// a/a+
			else if (mode.IsSet(FileOpenModes::Append))
			{
				_mode[0] = 'a';

				if (mode.IsSet(FileOpenModes::Read))
				{

					_mode[1] = '+';
				}
			}
			// r/r+
			else if (!mode.IsSet(FileOpenModes::Binary))
			{
				if (mode.IsSet(FileOpenModes::Read))
				{

					_mode[0] = 'r';
				}

				if (mode.IsSet(FileOpenModes::Write))
				{

					_mode[1] = '+';
				}
			}

			if ((hFile = fopen(GetPath().GetString().GetCString(), _mode)) == nullptr)
			{

				throw Exceptions::SystemException(
					"fopen"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			BitMask<DWORD> accessMask;
			accessMask.Set(GENERIC_READ, mode.IsSet(FileOpenModes::Read));
			accessMask.Set(GENERIC_WRITE, mode.IsSet(FileOpenModes::Write));

			DWORD dwCreationDisposition = OPEN_EXISTING;

			if (mode.IsSet(FileOpenModes::Truncate))
			{

				dwCreationDisposition = CREATE_ALWAYS;
			}

			if ((hFile = CreateFileA(GetPath().GetString().GetCString(), accessMask.Mask, FILE_SHARE_READ, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr)) == INVALID_HANDLE_VALUE)
			{
				hFile = INVALID_FILE_HANDLE_VALUE;

				auto lastErrorCode = GetLastError();

				if (lastErrorCode == ERROR_FILE_NOT_FOUND)
				{

					return false;
				}

				throw Exceptions::SystemException(
					"CreateFileA",
					lastErrorCode
				);
			}
#endif

			if (mode.IsSet(FileOpenModes::Append))
			{
				if (auto fileSize = GetSize())
				{
					try
					{
						SetWritePosition(
							fileSize
						);
					}
					catch (Exceptions::Exception& exception)
					{
#if defined(AL_PLATFORM_LINUX)
						fclose(GetHandle());
#elif defined(AL_PLATFORM_WINDOWS)
						CloseHandle(GetHandle());
#endif

						hFile = INVALID_FILE_HANDLE_VALUE;

						throw Exceptions::Exception(
							AL::Move(exception),
							"Error setting write position to end of file"
						);
					}
				}
			}

			return true;
		}
		
		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				fclose(GetHandle());
#elif defined(AL_PLATFORM_WINDOWS)
				CloseHandle(GetHandle());
#endif

				hFile = INVALID_FILE_HANDLE_VALUE;

				readPosition = 0;
				writePosition = 0;
			}
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		uint64 Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "File not open");

			size_t bytesRead = 0;

#if defined(AL_PLATFORM_LINUX)
			auto hFile = GetHandle();

			if (!feof(hFile))
			{
				if ((bytesRead = fread(lpBuffer, size, 1, hFile)) == 0)
				{
					if (!feof(hFile))
					{

						throw Exceptions::SystemException(
							"fread"
						);
					}
				}
			}
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD _bytesRead = 0;

			OVERLAPPED overlapped;
			ZeroMemory(&overlapped, sizeof(overlapped));

			Integer<uint64> offset;
			offset.Value = GetReadPosition();

			overlapped.Offset = offset.Low.Value;
			overlapped.OffsetHigh = offset.High.Value;

			if (ReadFile(GetHandle(), lpBuffer, static_cast<DWORD>(size), &_bytesRead, &overlapped) == 0)
			{
				if (GetLastError() != ERROR_HANDLE_EOF)
				{

					throw Exceptions::SystemException(
						"ReadFile"
					);
				}
			}

			bytesRead = _bytesRead;
#endif

			readPosition += bytesRead;

			return bytesRead;
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes written
		uint64 Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "File not open");

			size_t bytesWritten = 0;

#if defined(AL_PLATFORM_LINUX)
			if ((bytesWritten = fwrite(lpBuffer, 1, size, GetHandle())) == 0)
			{

				throw Exceptions::SystemException(
					"fwrite"
				);
			}

			if (fflush(GetHandle()) == EOF)
			{

				throw Exceptions::SystemException(
					"fflush"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD _bytesWritten = 0;

			if (WriteFile(GetHandle(), lpBuffer, static_cast<DWORD>(size), &_bytesWritten, nullptr) == 0)
			{

				throw Exceptions::SystemException(
					"WriteFile"
				);
			}

			bytesWritten = _bytesWritten;
#endif

			writePosition += bytesWritten;

			return bytesWritten;
		}

		auto& operator = (File&& file)
		{
			Close();

			path = AL::Move(
				file.path
			);

			hFile = file.hFile;
			file.hFile = INVALID_FILE_HANDLE_VALUE;

			readPosition = file.readPosition;
			file.readPosition = 0;

			writePosition = file.writePosition;
			file.writePosition = 0;

			return *this;
		}
	};
}
