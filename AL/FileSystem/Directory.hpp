#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"
#include "File.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#error Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <dirent.h>

	#include <sys/stat.h>
	#include <sys/types.h>
#endif

namespace AL::FileSystem
{
	class File;
	class Directory;

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(const Path& path)>           DirectoryEnumCallback;
	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(const File& file)>           DirectoryEnumFilesCallback;
	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(const Directory& directory)> DirectoryEnumDirectoriesCallback;

	class Directory
	{
		Path path;

		Directory(const Directory&) = delete;

	public:
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

			if (!Path::IsDirectory(path))
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
			if (::mkdir(path.GetCString(), S_IRUSR | S_IWUSR) == -1)
			{

				throw OS::SystemException(
					"mkdir"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (!::CreateDirectoryA(path.GetCString(), nullptr))
			{

				throw OS::SystemException(
					"CreateDirectoryA"
				);
			}
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
		static Void Delete(const String& path);

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
		static Bool Copy(const String& source, const String& destination);

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
		static Bool Move(const String& source, const String& destination);

		// @throw AL::Exception
		static Bool Contains(const Path& path, const String& name)
		{
			if (!Contains(path.GetString(), name))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		static Bool Contains(const String& path, const String& name)
		{
			auto _path = String::Join(
				'/',
				path,
				name
			);

			if (!Path::Exists(_path))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		static Void Enumerate(const Path& path, const DirectoryEnumCallback& callback)
		{
			Enumerate(
				path.GetString(),
				callback
			);
		}
		// @throw AL::Exception
		static Void Enumerate(const String& path, const DirectoryEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			::DIR* lpDIR;
			::dirent64* lpEntry;

			if ((lpDIR = ::opendir(path.GetCString())) == NULL)
			{

				throw OS::SystemException(
					"opendir"
				);
			}

			while ((lpEntry = ::readdir64(lpDIR)) != NULL)
			{
				auto _path = Path::Combine(
					path,
					lpEntry->d_name
				);

				try
				{
					if (!callback(_path))
					{

						break;
					}
				}
				catch (Exception&)
				{
					::closedir(
						lpDIR
					);

					throw;
				}
			}

			::closedir(
				lpDIR
			);
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetCString()
			);

			::HANDLE hFind;
			::WIN32_FIND_DATAA fileData;

			if ((hFind = ::FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw OS::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				auto _path = Path::Combine(
					path,
					fileData.cFileName
				);

				try
				{
					if (!callback(_path))
					{

						break;
					}
				}
				catch (Exception&)
				{
					::FindClose(
						hFind
					);

					throw;
				}
			} while (::FindNextFileA(hFind, &fileData));

			auto lastError = OS::GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				::FindClose(
					hFind
				);

				throw OS::SystemException(
					"FindNextFileA"
				);
			}

			::FindClose(
				hFind
			);
#endif
		}

		// @throw AL::Exception
		static Void EnumerateFiles(const Path& path, const DirectoryEnumFilesCallback& callback)
		{
			EnumerateFiles(
				path.GetString(),
				callback
			);
		}
		// @throw AL::Exception
		static Void EnumerateFiles(const String& path, const DirectoryEnumFilesCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			::DIR* lpDIR;
			::dirent64* lpEntry;

			if ((lpDIR = ::opendir(path.GetCString())) == NULL)
			{

				throw OS::SystemException(
					"opendir"
				);
			}

			while ((lpEntry = ::readdir64(lpDIR)) != NULL)
			{
				if (lpEntry->d_type != DT_REG)
				{

					continue;
				}

				auto _path = Path::Combine(
					path,
					lpEntry->d_name
				);

				File _file(
					AL::Move(_path)
				);

				try
				{
					if (!callback(_file))
					{

						break;
					}
				}
				catch (Exception&)
				{
					::closedir(
						lpDIR
					);

					throw;
				}
			}

			::closedir(
				lpDIR
			);
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetCString()
			);

			::HANDLE hFind;
			::WIN32_FIND_DATAA fileData;

			if ((hFind = ::FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw OS::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				if (!BitMask<DWORD>::IsSet(fileData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
				{
					auto filePath = Path::Combine(
						path,
						fileData.cFileName
					);

					File file(
						AL::Move(filePath)
					);

					try
					{
						if (!callback(file))
						{

							break;
						}
					}
					catch (Exception&)
					{
						::FindClose(
							hFind
						);

						throw;
					}
				}
			} while (::FindNextFileA(hFind, &fileData));

			auto lastError = OS::GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				::FindClose(
					hFind
				);

				throw OS::SystemException(
					"FindNextFileA"
				);
			}

			::FindClose(
				hFind
			);
#endif
		}

		// @throw AL::Exception
		static Void EnumerateDirectories(const Path& path, const DirectoryEnumDirectoriesCallback& callback)
		{
			EnumerateDirectories(
				path.GetString(),
				callback
			);
		}
		// @throw AL::Exception
		static Void EnumerateDirectories(const String& path, const DirectoryEnumDirectoriesCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			::DIR* lpDIR;
			::dirent64* lpEntry;

			if ((lpDIR = ::opendir(path.GetCString())) == NULL)
			{

				throw OS::SystemException(
					"opendir"
				);
			}

			while ((lpEntry = ::readdir64(lpDIR)) != NULL)
			{
				if (lpEntry->d_type != DT_DIR)
				{

					continue;
				}

				auto _path = Path::Combine(
					path,
					lpEntry->d_name
				);

				Directory _directory(
					AL::Move(_path)
				);

				try
				{
					if (!callback(_directory))
					{

						break;
					}
				}
				catch (Exception&)
				{
					::closedir(
						lpDIR
					);

					throw;
				}
			}

			::closedir(
				lpDIR
			);
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetCString()
			);

			::HANDLE hFind;
			::WIN32_FIND_DATAA fileData;

			if ((hFind = ::FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw OS::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				if (BitMask<DWORD>::IsSet(fileData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
				{
					auto directoryPath = Path::Combine(
						path,
						fileData.cFileName
					);

					Directory directory(
						AL::Move(directoryPath)
					);

					try
					{
						if (!callback(directory))
						{

							break;
						}
					}
					catch (Exception&)
					{
						::FindClose(
							hFind
						);

						throw;
					}
				}
			} while (::FindNextFileA(hFind, &fileData));

			auto lastError = OS::GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				::FindClose(
					hFind
				);

				throw OS::SystemException(
					"FindNextFileA"
				);
			}

			::FindClose(
				hFind
			);
#endif
		}

		Directory(Directory&& directory)
			: path(
				AL::Move(directory.path)
			)
		{
		}

		explicit Directory(Path&& path)
			: path(
				AL::Move(path)
			)
		{
		}
		explicit Directory(const Path& path)
			: Directory(
				Path(path)
			)
		{
		}
		
		explicit Directory(String&& path)
			: Directory(
				Path(path)
			)
		{
		}
		explicit Directory(const String& path)
			: Directory(
				Path(path)
			)
		{
		}

		virtual ~Directory()
		{
		}

		auto& GetPath() const
		{
			return path;
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
		Bool Contains(const String& name) const
		{
			if (!Contains(GetPath(), name))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Void Enumerate(const DirectoryEnumCallback& callback) const
		{
			Enumerate(
				GetPath(),
				callback
			);
		}

		// @throw AL::Exception
		Void EnumerateFiles(const DirectoryEnumFilesCallback& callback) const
		{
			EnumerateFiles(
				GetPath(),
				callback
			);
		}

		// @throw AL::Exception
		Void EnumerateDirectories(const DirectoryEnumDirectoriesCallback& callback) const
		{
			EnumerateDirectories(
				GetPath(),
				callback
			);
		}

		Directory& operator = (Directory&& directory)
		{
			path = AL::Move(
				directory.path
			);

			return *this;
		}

		Bool operator == (const Directory& directory) const
		{
			if (GetPath() != directory.GetPath())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Directory& directory) const
		{
			if (operator==(directory))
			{

				return False;
			}

			return True;
		}
	};
}
