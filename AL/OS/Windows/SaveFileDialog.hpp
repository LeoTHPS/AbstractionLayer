#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

#include "FileDialog.hpp"
#include "CommDialogException.hpp"

namespace AL::OS::Windows
{
	class SaveFileDialog
		: public FileDialog
	{
	public:
		using FileDialog::FileDialog;

		// @throw AL::Exception
		// @return AL::False if closed
		virtual Bool Show(FileSystem::Path& path) override
		{
			if (!::GetSaveFileNameA(&openfilename))
			{
				if (!CommDialogException::IsErrorSet())
				{

					return False;
				}

				throw CommDialogException(
					"GetSaveFileNameA"
				);
			}

			path = FileSystem::Path(
				String(openfilename.lpstrFile)
			);

			return True;
		}
	};
}
