#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "FileDialog.hpp"
#include "CommDialogException.hpp"

namespace AL::OS::Windows
{
	class OpenFileDialog
		: public FileDialog
	{
	public:
		using FileDialog::FileDialog;

		// @throw AL::Exception
		// @return AL::False if closed
		virtual Bool Show(FileSystem::Path& path) override
		{
			if (!::GetOpenFileNameA(&openfilename))
			{
				if (!CommDialogException::IsErrorSet())
				{

					return False;
				}

				throw CommDialogException(
					"GetOpenFileNameA"
				);
			}

			path = FileSystem::Path(
				String(openfilename.lpstrFile)
			);

			return True;
		}
	};
}
