#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Map.hpp"
#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/Path.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <commdlg.h>
#endif

namespace AL::OS
{
	typedef Collections::Map<String, String> FileDialogFilter;
	
	class FileDialog
	{
		String title;
		String directory;
		String defaultExtension;
		Collections::Array<char> name;
		Collections::Array<char> filter;

		FileDialog(FileDialog&&) = delete;
		FileDialog(const FileDialog&) = delete;

	protected:
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		OPENFILENAMEA openfilename;
#endif

	public:
		FileDialog()
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			: FileDialog(
				NULL
			)
#endif
		{
		}

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		explicit FileDialog(HWND hWND)
			: name(
				AL_MAX_PATH
			)
		{
			ZeroMemory(
				&openfilename,
				sizeof(OPENFILENAMEA)
			);

			openfilename.lStructSize = sizeof(OPENFILENAMEA);
			openfilename.hwndOwner = hWND;
			openfilename.lpstrFile = &name[0];
			openfilename.nMaxFile = static_cast<DWORD>(name.GetCapacity());
			openfilename.Flags = OFN_NOCHANGEDIR;
		}
#endif

		auto& GetTitle() const
		{
			return title;
		}

		auto& GetDirectory() const
		{
			return directory;
		}

		auto& GetDefaultExtension() const
		{
			return defaultExtension;
		}

		void SetTitle(String&& value)
		{
			title = Move(
				value
			);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			openfilename.lpstrTitle = title.GetCString();
#endif
		}
		void SetTitle(const String& value)
		{
			SetTitle(
				String(value)
			);
		}

		void SetDirectory(String&& value)
		{
			directory = Move(
				value
			);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			openfilename.lpstrInitialDir = directory.GetCString();
#endif
		}
		void SetDirectory(const String& value)
		{
			SetDirectory(
				String(value)
			);
		}

		void SetFilter(const FileDialogFilter& value)
		{
			size_t filterSize = 0;

			for (auto it = value.cbegin(); it != value.cend(); ++it)
			{
				filterSize += it->first.GetLength() + 1;
				filterSize += it->second.GetLength() + 1;
			}

			filter.SetSize(
				filterSize + 2
			);

			filter[filterSize] = '\0';
			filter[filterSize + 1] = '\0';

			size_t filterIndex = 0;

			auto appendFilter = [this, &filterIndex](const String& _buffer)
			{
				for (size_t i = 0; i < _buffer.GetLength(); ++i, ++filterIndex)
				{
					filter[filterIndex] = _buffer[i];
				}

				filter[filterIndex++] = '\0';
			};

			for (auto it = value.cbegin(); it != value.cend(); ++it)
			{
				appendFilter(
					it->first
				);

				appendFilter(
					it->second
				);
			}

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			openfilename.lpstrFilter = &filter[0];
			openfilename.nFilterIndex = 1;
#endif
		}

		void SetCheckFileExists(bool set = true)
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			BitMask<DWORD> flags(
				openfilename.Flags
			);

			flags.Set(
				OFN_FILEMUSTEXIST,
				set
			);

			openfilename.Flags = flags.Mask;
#endif
		}

		void SetCheckPathExists(bool set = true)
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			BitMask<DWORD> flags(
				openfilename.Flags
			);

			flags.Set(
				OFN_PATHMUSTEXIST,
				set
			);

			openfilename.Flags = flags.Mask;
#endif
		}

		void SetDefaultExtension(String&& value)
		{
			defaultExtension = Move(
				value
			);

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			openfilename.lpstrDefExt = defaultExtension.GetCString();
#endif
		}
		void SetDefaultExtension(const String& value)
		{
			SetDefaultExtension(
				String(value)
			);
		}

		// @return false if closed
		// @throw AL::Exceptions::Exception
		virtual bool Show(FileSystem::Path& path) = 0;
	};
}
