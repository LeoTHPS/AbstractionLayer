#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

namespace AL::FileSystem
{
	typedef Function<void(const Path& path)> DirectoryEnumCallback;

	class Directory
	{
		Path path;

	public:
		// @throw AL::Exceptions::Exception
		static bool Exists(const Path& path)
		{
			return path.Exists() && path.IsDirectory();
		}

		// @throw AL::Exceptions::Exception
		static void Create(const Path& path)
		{
#if defined(AL_PLATFORM_LINUX)
			if (mkdir(path.GetString().GetCString(), S_IRUSR | S_IWUSR) == -1)
			{

				throw Exceptions::SystemException(
					"mkdir"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (!CreateDirectoryA(path.GetString().GetCString(), nullptr))
			{

				throw Exceptions::SystemException(
					"CreateDirectoryA"
				);
			}
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Move(const Path& source, const Path& destination);

		// @throw AL::Exceptions::Exception
		static void Copy(const Path& source, const Path& destination);

		// @throw AL::Exceptions::Exception
		static void Delete(const Path& path);

		// @throw AL::Exceptions::Exception
		static bool Contains(const Path& path, const String& fileOrDirectoryName)
		{
			Path fileOrDirectoryPath = String::Combine(
				path.GetString(),
				fileOrDirectoryName
			);

			return fileOrDirectoryPath.Exists();
		}

		// @throw AL::Exceptions::Exception
		static void Enumerate(const Path& path, const DirectoryEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetString().GetCString()
			);

			HANDLE hFind;
			WIN32_FIND_DATAA fileData;

			if ((hFind = FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				Path _path = String::Format(
					"%s/%s",
					path.GetString().GetCString(),
					fileData.cFileName
				);

				callback(_path);
			} while (FindNextFileA(hFind, &fileData));

			auto lastError = GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				FindClose(hFind);

				throw Exceptions::SystemException(
					"FindNextFileA"
				);
			}

			FindClose(hFind);
#endif
		}
		// @throw AL::Exceptions::Exception
		static void EnumerateFiles(const Path& path, const DirectoryEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetString().GetCString()
			);

			HANDLE hFind;
			WIN32_FIND_DATAA fileData;

			if ((hFind = FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				if (!BitMask<DWORD>::IsSet<FILE_ATTRIBUTE_DIRECTORY>(fileData.dwFileAttributes))
				{
					Path _path = String::Format(
						"%s/%s",
						path.GetString().GetCString(),
						fileData.cFileName
					);

					callback(_path);
				}
			} while (FindNextFileA(hFind, &fileData));

			auto lastError = GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				FindClose(hFind);

				throw Exceptions::SystemException(
					"FindNextFileA"
				);
			}

			FindClose(hFind);
#endif
		}
		// @throw AL::Exceptions::Exception
		static void EnumerateDirectories(const Path& path, const DirectoryEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto pattern = String::Format(
				"%s/*",
				path.GetString().GetCString()
			);

			HANDLE hFind;
			WIN32_FIND_DATAA fileData;

			if ((hFind = FindFirstFileA(pattern.GetCString(), &fileData)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"FindFirstFileA"
				);
			}

			do
			{
				if (BitMask<DWORD>::IsSet<FILE_ATTRIBUTE_DIRECTORY>(fileData.dwFileAttributes))
				{
					Path _path = String::Format(
						"%s/%s",
						path.GetString().GetCString(),
						fileData.cFileName
					);

					callback(_path);
				}
			} while (FindNextFileA(hFind, &fileData));

			auto lastError = GetLastError();

			if ((lastError != ERROR_SUCCESS) && (lastError != ERROR_NO_MORE_FILES))
			{
				FindClose(hFind);

				throw Exceptions::SystemException(
					"FindNextFileA"
				);
			}

			FindClose(hFind);
#endif
		}

		Directory(Path&& path)
			: path(
				AL::Move(path)
			)
		{
		}

		Directory(const Path& path)
			: Directory(
				Path(path)
			)
		{
		}

		auto& GetPath() const
		{
			return path;
		}

		// @throw AL::Exceptions::Exception
		bool Exists() const
		{
			return Exists(
				GetPath()
			);
		}

		// @throw AL::Exceptions::Exception
		void Create()
		{
			Create(
				GetPath()
			);
		}

		// @throw AL::Exceptions::Exception
		void Move(const Path& destination)
		{
			Move(
				GetPath(),
				destination
			);
		}

		// @throw AL::Exceptions::Exception
		void Copy(const Path& destination)
		{
			Copy(
				GetPath(),
				destination
			);
		}
		
		// @throw AL::Exceptions::Exception
		void Delete()
		{
			Delete(
				GetPath()
			);
		}

		// @throw AL::Exceptions::Exception
		bool Contains(const String& fileOrDirectoryName) const
		{
			return Contains(
				GetPath(),
				fileOrDirectoryName
			);
		}

		// @throw AL::Exceptions::Exception
		void Enumerate(const DirectoryEnumCallback& callback) const
		{
			Enumerate(
				GetPath(),
				callback
			);
		}
		// @throw AL::Exceptions::Exception
		void EnumerateFiles(const DirectoryEnumCallback& callback) const
		{
			EnumerateFiles(
				GetPath(),
				callback
			);
		}
		// @throw AL::Exceptions::Exception
		void EnumerateDirectories(const DirectoryEnumCallback& callback) const
		{
			EnumerateDirectories(
				GetPath(),
				callback
			);
		}
	};
}
