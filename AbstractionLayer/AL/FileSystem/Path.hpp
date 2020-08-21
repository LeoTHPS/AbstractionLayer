#pragma once
#include "AL/Common.hpp"

#include <cstdio>

#if defined(AL_PLATFORM_LINUX)
	#include <sys/stat.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <Shlwapi.h>

	#pragma comment(lib, "Shlwapi.lib")
#endif

#define AL_MAX_PATH FILENAME_MAX

namespace AL::FileSystem
{
	class Path
	{
		String string;

	public:
		static bool IsValid(const String& path)
		{
			size_t length = 0;

			for (auto c : path)
			{
				if (++length > AL_MAX_PATH)
				{

					return false;
				}

				if ((c >= '\0') && (c <= '\31'))
				{

					return false;
				}

				switch (c)
				{
					case '<':
					case '>':
					case ':':
					case '"':
					case '/':
					case '\\':
					case '|':
					case '?':
					case '*':
						return false;
				}
			}

			return true;
		}

		// @throw AL::Exceptions::Exception
		static bool IsFile(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat s;

			if (lstat(path.GetCString(), &s) == -1)
			{

				throw Exceptions::SystemException(
					"lstat"
				);
			}

			return S_ISREG(s.st_mode);
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD attributes;

			if ((attributes = GetFileAttributesA(path.GetCString())) == INVALID_FILE_ATTRIBUTES)
			{

				throw Exceptions::SystemException(
					"GetFileAttributesA"
				);
			}

			return !BitMask<DWORD>::IsSet<FILE_ATTRIBUTE_DIRECTORY>(
				attributes
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		static bool IsDirectory(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat s;

			if (lstat(path.GetCString(), &s) == -1)
			{

				throw Exceptions::SystemException(
					"lstat"
				);
			}

			return S_ISDIR(s.st_mode);
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD attributes;

			if ((attributes = GetFileAttributesA(path.GetCString())) == INVALID_FILE_ATTRIBUTES)
			{

				throw Exceptions::SystemException(
					"GetFileAttributesA"
				);
			}

			return BitMask<DWORD>::IsSet<FILE_ATTRIBUTE_DIRECTORY>(
				attributes
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		static bool Exists(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			return access(path.GetCString(), F_OK) != -1;
#elif defined(AL_PLATFORM_WINDOWS)
			if (PathFileExistsA(path.GetCString()))
			{

				return true;
			}

			else if (GetLastError() != ERROR_FILE_NOT_FOUND)
			{

				throw Exceptions::SystemException(
					"PathFileExistsA"
				);
			}

			return false;
#endif
		}

		template<typename ... CHUNK_TYPES>
		static Path Combine(CHUNK_TYPES ... chunks)
		{
			return String::Combine<CHUNK_TYPES ...>(
				chunks ...
			);
		}

		Path(String&& path)
			: string(
				Move(path)
			)
		{
		}

		Path(const String& path)
			: Path(
				String(path)
			)
		{
		}

		bool IsValid() const
		{
			return IsValid(
				string
			);
		}

		// @throw AL::Exceptions::Exception
		bool IsFile() const
		{
			return IsFile(
				string
			);
		}

		// @throw AL::Exceptions::Exception
		bool IsDirectory() const
		{
			return IsDirectory(
				string
			);
		}

		auto& GetString() const
		{
			return string;
		}

		auto GetRootPath() const;

		auto GetFileName() const
		{
			auto& path = GetString();

			size_t fileNameStartIndex;

			if ((fileNameStartIndex = path.IndexOfLast('/')) == String::NPOS)
			{
				if ((fileNameStartIndex = path.IndexOfLast('\\')) == String::NPOS)
				{

					fileNameStartIndex = 0;
				}
			}

			size_t fileNameEndIndex = 0;
			
			for (auto pathLength = path.GetLength(); fileNameEndIndex < pathLength; ++fileNameEndIndex)
			{
				if (path[fileNameEndIndex] == '.')
				{

					break;
				}
			}

			return path.SubString(
				++fileNameStartIndex,
				fileNameEndIndex
			);
		}

		auto GetFileNameExtension() const
		{
			auto& path = GetString();

			size_t fileNameStartIndex;
			
			if ((fileNameStartIndex = path.IndexOfLast('/')) == String::NPOS)
			{
				if ((fileNameStartIndex = path.IndexOfLast('\\')) == String::NPOS)
				{

					fileNameStartIndex = 0;
				}
			}

			return path.SubString(
				++fileNameStartIndex
			);
		}

		auto GetFileExtension() const;

		auto GetDirectoryName() const;

		// @throw AL::Exceptions::Exception
		bool Exists() const
		{
			return Exists(
				GetString()
			);
		}
	};
}
