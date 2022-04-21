#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/Collections/ArrayList.hpp"
#include "AL/Collections/Dictionary.hpp"

#include "AL/FileSystem/Path.hpp"

#include "AL/OS/SystemException.hpp"

#include <commdlg.h>

namespace AL::OS::Windows
{
	typedef Collections::Dictionary<String, String> FileDialogFilter;

	class FileDialog
	{
		Collections::Array<char> name;
		String                   title;
		Collections::Array<char> filter;
		String                   directory;
		String                   defaultExtension;

		FileDialog(FileDialog&&) = delete;
		FileDialog(const FileDialog&) = delete;

	protected:
		::OPENFILENAMEA openfilename;

	public:
		FileDialog()
			: FileDialog(
				NULL
			)
		{
		}

		explicit FileDialog(::HWND hWND)
			: name(
				AL_MAX_PATH
			),
			openfilename(
				OPENFILENAMEA { 0 }
			)
		{
			openfilename.lStructSize = sizeof(::OPENFILENAMEA);
			openfilename.hwndOwner = hWND;
			openfilename.lpstrFile = &name[0];
			openfilename.nMaxFile = static_cast<::DWORD>(name.GetCapacity());
			openfilename.Flags = OFN_NOCHANGEDIR;
		}

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

		Void SetTitle(String&& value)
		{
			title = Move(
				value
			);

			openfilename.lpstrTitle = title.GetCString();
		}
		Void SetTitle(const String& value)
		{
			SetTitle(
				String(value)
			);
		}

		Void SetDirectory(String&& value)
		{
			directory = Move(
				value
			);

			openfilename.lpstrInitialDir = directory.GetCString();
		}
		Void SetDirectory(const String& value)
		{
			SetDirectory(
				String(value)
			);
		}

		Void SetFilter(const FileDialogFilter& value)
		{
			size_t filterSize = 0;

			for (auto it = value.cbegin(); it != value.cend(); ++it)
			{
				filterSize += it->Key.GetLength() + 1;
				filterSize += it->Value.GetLength() + 1;
			}

			filter.SetCapacity(
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
					it->Key
				);

				appendFilter(
					it->Value
				);
			}

			openfilename.lpstrFilter = &filter[0];
			openfilename.nFilterIndex = 1;
		}

		Void SetCheckFileExists(Bool set = True)
		{
			BitMask<::DWORD> flags(
				openfilename.Flags
			);

			flags.Set(
				OFN_FILEMUSTEXIST,
				set
			);

			openfilename.Flags = flags.Value;
		}

		Void SetCheckPathExists(Bool set = True)
		{
			BitMask<::DWORD> flags(
				openfilename.Flags
			);

			flags.Set(
				OFN_PATHMUSTEXIST,
				set
			);

			openfilename.Flags = flags.Value;
		}

		Void SetDefaultExtension(String&& value)
		{
			defaultExtension = Move(
				value
			);

			openfilename.lpstrDefExt = defaultExtension.GetCString();
		}
		Void SetDefaultExtension(const String& value)
		{
			SetDefaultExtension(
				String(value)
			);
		}

		// @throw AL::Exception
		// @return False if closed
		virtual Bool Show(FileSystem::Path& path) = 0;
	};
}
