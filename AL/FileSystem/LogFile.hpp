#pragma once
#include "AL/Common.hpp"

#include "TextFile.hpp"

#include "AL/OS/Mutex.hpp"

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

		Bool IsOpen() const
		{
			OS::MutexGuard lock(
				mutex
			);

			if (!file.IsOpen())
			{

				return False;
			}

			return True;
		}

		auto& GetPath() const
		{
			return file.GetPath();
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"LogFile already open"
			);

			OS::MutexGuard lock(
				mutex
			);

			file.Open(
				FileOpenModes::Truncate | FileOpenModes::Write
			);
		}

		Void Close()
		{
			if (IsOpen())
			{
				OS::MutexGuard lock(
					mutex
				);

				file.Close();
			}
		}

		// @format: [Time: $timestamp] [Thread: $threadId] message
		// @throw AL::Exception
		template<typename ... TArgs>
		Void WriteLine(const String& format, TArgs ... args)
		{
			AL_ASSERT(
				IsOpen(),
				"LogFile not open"
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

			OS::MutexGuard lock(
				mutex
			);

			file.WriteLine(
				line
			);
		}
	};
}
