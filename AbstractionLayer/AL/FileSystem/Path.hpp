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

		template<typename T_DELIMITER>
		static bool String_StartsWith(const String& string, T_DELIMITER delimiter)
		{
			return string.StartsWith(delimiter);
		}
		static bool String_StartsWith(const String::Char* string, String::Char delimiter)
		{
			return String_StartsWith(
				string,
				{ delimiter, Collections::__String_Constants<String::Char>::END }
			);
		}
		static bool String_StartsWith(const String::Char* string, const String::Char* delimiter)
		{
			size_t delimiterLength;

			if (String::GetLength(string) < (delimiterLength = String::GetLength(delimiter)))
			{

				return false;
			}
			
			for (size_t i = 0; i < delimiterLength; ++i)
			{
				if (string[i] != delimiter[i])
				{

					return false;
				}
			}

			return true;
		}
		template<size_t S_DELIMITER>
		static bool String_StartsWith(const String::Char* string, const String::Char(&delimiter)[S_DELIMITER])
		{
			return String_StartsWith(
				string,
				&delimiter[0]
			);
		}
		template<size_t S_STRING, typename T_DELIMITER>
		static bool String_StartsWith(const String::Char(&string)[S_STRING], T_DELIMITER delimiter)
		{
			return String_StartsWith(
				&string[0],
				delimiter
			);
		}

		static String Combine2(String& buffer)
		{
			return String(
				Move(buffer)
			);
		}
		template<typename T_CHUNK, typename ... T_CHUNKS>
		static String Combine2(String& buffer, const T_CHUNK& chunk, const T_CHUNKS& ... chunks)
		{
			if (!buffer.EndsWith('/') && !buffer.EndsWith('\\') &&
				!String_StartsWith(chunk, '/') && !String_StartsWith(chunk, '\\'))
			{

				buffer.Append(
					'/'
				);
			}

			buffer.Append(
				chunk
			);

			return Combine2(
				buffer,
				chunks ...
			);
		}

	public:
		template<typename T_CHUNK, typename ... T_CHUNKS>
		static Path Combine(const T_CHUNK& chunk, const T_CHUNKS& ... chunks)
		{
			String buffer(
				chunk
			);

			return Combine2(
				buffer,
				chunks ...
			);
		}

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

			else if (auto lastErrorCode = GetLastError(); (lastErrorCode != ERROR_FILE_NOT_FOUND) && (lastErrorCode != ERROR_PATH_NOT_FOUND))
			{

				throw Exceptions::SystemException(
					"PathFileExistsA"
				);
			}

			return false;
#endif
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

		auto GetRootPath() const
		{
			auto& path = GetString();
			auto pathLength = path.GetLength();

			size_t rootPathEnd = 0;

			for (size_t i = 0; i < pathLength; ++i)
			{
				auto c = path[i];

				if ((c == '/') || (c == '\\'))
				{
					if ((rootPathEnd = i) > 0)
					{

						--rootPathEnd;
					}
				}
			}

			size_t rootPathBegin = 0;

			for (size_t i = rootPathEnd; i >= 0; --i)
			{
				auto c = path[i];

				if ((c == '/') || (c == '\\'))
				{

					rootPathBegin = i + 1;
				}
			}

			auto rootPath = path.SubString(
				rootPathBegin,
				rootPathEnd
			);

			return Path(
				Move(rootPath)
			);
		}

		auto GetFileName() const
		{
			auto& path = GetString();
			auto pathLength = path.GetLength();

			size_t fileNameStartIndex = 0;

			for (size_t i = 0; i < pathLength; ++i)
			{
				auto c = path[i];

				if ((c == '/') || (c == '\\'))
				{

					fileNameStartIndex = i + 1;
				}
			}

			size_t fileNameEndIndex = pathLength;

			for (size_t i = fileNameStartIndex; i < pathLength; ++i)
			{
				if (path[i] == '.')
				{
					fileNameEndIndex = i - 1;

					break;
				}
			}

			if (fileNameEndIndex < fileNameStartIndex)
			{

				fileNameEndIndex = fileNameStartIndex;
			}
			
			return path.SubString(
				fileNameStartIndex,
				fileNameEndIndex
			);
		}

		auto GetFileExtension() const
		{
			auto& path = GetString();
			auto pathLength = path.GetLength();

			size_t fileNameStartIndex = 0;

			for (size_t i = 0; i < pathLength; ++i)
			{
				auto c = path[i];

				if ((c == '/') || (c == '\\'))
				{

					fileNameStartIndex = i + 1;
				}
			}

			size_t fileExtensionStartIndex = 0;

			for (size_t i = fileNameStartIndex; i < pathLength; ++i)
			{
				if (path[i] == '.')
				{
					fileExtensionStartIndex = i;

					break;
				}
			}

			return path.SubString(
				fileExtensionStartIndex
			);
		}
		
		auto GetFileOrDirectoryName() const
		{
			auto& path = GetString();
			auto pathLength = path.GetLength();

			size_t nameIndexStart = 0;

			for (size_t i = 0; i < pathLength; ++i)
			{
				auto c = path[i];

				if ((c == '/') || (c == '\\'))
				{

					nameIndexStart = i + 1;
				}
			}

			return path.SubString(
				nameIndexStart
			);
		}

		// @throw AL::Exceptions::Exception
		bool Exists() const
		{
			return Exists(
				GetString()
			);
		}
	};
}
