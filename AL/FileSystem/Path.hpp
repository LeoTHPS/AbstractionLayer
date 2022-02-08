#pragma once
#include "AL/Common.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#error Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <sys/stat.h>
	#include <sys/types.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <Shlwapi.h>

	#if defined(AL_COMPILER_MSVC)
		#pragma comment(lib, "Shlwapi.lib")
	#endif
#endif

#define AL_MAX_PATH FILENAME_MAX

namespace AL::FileSystem
{
	class Path
	{
		String string;

		static Bool String_StartsWith(const char* string, char delimiter)
		{
			if (!String_StartsWith(string, { delimiter, String::END }))
			{

				return False;
			}

			return True;
		}
		static Bool String_StartsWith(const char* string, const char* delimiter)
		{
			size_t delimiterLength;

			if (String::GetLength(string) < (delimiterLength = String::GetLength(delimiter)))
			{

				return False;
			}

			auto lpString = string;
			auto lpDelimiter = delimiter;

			for (size_t i = 0; i < delimiterLength; ++i, ++lpString, ++lpDelimiter)
			{
				if (*lpString != *lpDelimiter)
				{

					return False;
				}
			}

			return True;
		}
		template<size_t S_DELIMITER>
		static Bool String_StartsWith(const char* string, const char(&delimiter)[S_DELIMITER])
		{
			if (!String_StartsWith(string, &delimiter[0]))
			{

				return False;
			}

			return True;
		}
		template<typename T_DELIMITER>
		static Bool String_StartsWith(const String& string, T_DELIMITER delimiter)
		{
			if (!string.StartsWith(delimiter))
			{

				return False;
			}

			return True;
		}
		template<size_t S_STRING, typename T_DELIMITER>
		static Bool String_StartsWith(const char(&string)[S_STRING], T_DELIMITER delimiter)
		{
			if (!String_StartsWith(&string[0], delimiter))
			{

				return False;
			}

			return True;
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
			// TODO: debug this
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

		static Bool IsValid(const String& path)
		{
			size_t length = 0;

			for (auto it = path.cbegin(); it != path.cend(); ++it)
			{
				auto c = *it;

				if (++length > AL_MAX_PATH)
				{

					return False;
				}

				if ((c >= '\0') && (c <= '\31'))
				{

					return False;
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
						return False;
				}
			}

			return True;
		}

		// @throw AL::Exception
		static Bool IsFile(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat s;

			if (::lstat(path.GetCString(), &s) == -1)
			{

				throw OS::SystemException(
					"lstat"
				);
			}

			if (!S_ISREG(s.st_mode))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::DWORD attributes;

			if ((attributes = ::GetFileAttributesA(path.GetCString())) == INVALID_FILE_ATTRIBUTES)
			{

				throw OS::SystemException(
					"GetFileAttributesA"
				);
			}

			if (BitMask<::DWORD>::IsSet(attributes, FILE_ATTRIBUTE_DIRECTORY))
			{

				return False;
			}
#endif

			return True;
		}

		// @throw AL::Exception
		static Bool IsDirectory(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			struct stat s;

			if (::lstat(path.GetCString(), &s) == -1)
			{

				throw OS::SystemException(
					"lstat"
				);
			}

			if (!S_ISDIR(s.st_mode))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			::DWORD attributes;

			if ((attributes = ::GetFileAttributesA(path.GetCString())) == INVALID_FILE_ATTRIBUTES)
			{

				throw OS::SystemException(
					"GetFileAttributesA"
				);
			}

			if (!BitMask<DWORD>::IsSet(attributes, FILE_ATTRIBUTE_DIRECTORY))
			{

				return False;
			}
#endif

			return True;
		}

		// @throw AL::Exception
		static Bool Exists(const String& path)
		{
#if defined(AL_PLATFORM_LINUX)
			if (::access(path.GetCString(), F_OK) == -1)
			{
				auto errorCode = OS::GetLastError();
				
				if (errorCode == ENOENT)
				{

					return False;
				}

				throw OS::SystemException(
					"access",
					errorCode
				);
			}

			return True;
#elif defined(AL_PLATFORM_WINDOWS)
			if (::PathFileExistsA(path.GetCString()))
			{

				return True;
			}

			else if (auto errorCode = OS::GetLastError(); (errorCode != ERROR_FILE_NOT_FOUND) && (errorCode != ERROR_PATH_NOT_FOUND))
			{

				throw OS::SystemException(
					"PathFileExistsA",
					errorCode
				);
			}

			return False;
#endif
		}

		Path(Path&& path)
			: string(
				Move(path.string)
			)
		{
		}
		Path(const Path& path)
			: string(
				path.string
			)
		{
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

		Bool IsValid() const
		{
			if (!IsValid(string))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool IsFile() const
		{
			if (!IsFile(string))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool IsDirectory() const
		{
			if (!IsDirectory(string))
			{

				return False;
			}

			return True;
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
				rootPathEnd - rootPathBegin
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
				fileNameEndIndex - fileNameStartIndex
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

		// @throw AL::Exception
		Bool Exists() const
		{
			if (!Exists(GetString()))
			{

				return False;
			}

			return True;
		}

		Path& operator = (Path&& path)
		{
			string = Move(
				path.string
			);

			return *this;
		}
		Path& operator = (const Path& path)
		{
			string = path.string;

			return *this;
		}
		
		Path& operator = (String&& string)
		{
			this->string = Move(
				string
			);

			return *this;
		}
		Path& operator = (const String& string)
		{
			this->string = string;

			return *this;
		}

		Bool operator == (const Path& path) const
		{
			if (GetString() != path.GetString())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Path& path) const
		{
			if (operator==(path))
			{
				
				return False;
			}

			return True;
		}
	};
}
