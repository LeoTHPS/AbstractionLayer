#pragma once
#include "AL/Common.hpp"

#include "TextFile.hpp"

namespace AL::FileSystem
{
	class LogFile
	{
		TextFile file;

		mutable OS::Mutex mutex;

	public:
		explicit LogFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}
		
		explicit LogFile(const Path& path)
			: file(
				path
			)
		{
		}

		bool IsOpen() const
		{
			OS::MutexGuard lock(
				mutex
			);

			return file.IsOpen();
		}

		auto& GetPath() const
		{
			return file.GetPath();
		}

		// @throw AL::Exceptions::Exception
		void Open()
		{
			AL_ASSERT(!IsOpen(), "LogFile already open");

			OS::MutexGuard lock(
				mutex
			);

			file.Open(
				FileOpenModes::Truncate | FileOpenModes::Write
			);
		}

		void Close()
		{
			if (IsOpen())
			{
				OS::MutexGuard lock(
					mutex
				);

				file.Close();
			}
		}

		// @throw AL::Exceptions::Exception
		template<typename ... TArgs>
		void WriteLine(const String& format, TArgs ... args)
		{
			AL_ASSERT(IsOpen(), "LogFile not open");

			OS::MutexGuard lock(
				mutex
			);

			auto input = String::Format(
				format,
				Forward<TArgs>(args) ...
			);

			auto line = String::Format(
				"[Time: %llu] [Thread: %lu] %s",
				OS::System::GetTimestamp().ToSeconds(),
				OS::GetCurrentThreadId(),
				input.GetCString()
			);

			file.WriteLine(
				line
			);
		}
	};
}
