#pragma once
#include "AL/Common.hpp"

#include "File.hpp"

namespace AL::FileSystem
{
	class TextFile
		: public File
	{
#if defined(AL_PLATFORM_LINUX)
		static constexpr const char LINE_TERMINATOR[] = "\n";
#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr const char LINE_TERMINATOR[] = "\r\n";
#endif

	public:
		using File::File;

		// @throw AL::Exceptions::Exception
		bool Read(char& value);

		// @throw AL::Exceptions::Exception
		bool ReadLine(String& value);

		// @throw AL::Exceptions::Exception
		void Write(char value)
		{
			auto string = String::Format(
				"%c",
				value
			);

			Write(
				string
			);
		}
		// @throw AL::Exceptions::Exception
		void Write(const String& value);

		// @throw AL::Exceptions::Exception
		void WriteLine(const String& value)
		{
			auto line = String::Format(
				"%s%s",
				value.GetCString(),
				LINE_TERMINATOR
			);

			Write(
				line
			);
		}
	};
}
