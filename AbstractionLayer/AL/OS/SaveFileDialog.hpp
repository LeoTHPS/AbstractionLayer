#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/Exceptions/CommDialogException.hpp"
#endif

namespace AL::OS
{
	class SaveFileDialog
		: public FileDialog
	{
	public:
		using FileDialog::FileDialog;

		// @return false if closed
		// @throw AL::Exceptions::Exception
		virtual bool Show(FileSystem::Path& path) override
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (!GetSaveFileNameA(&openfilename))
			{
				if (!Exceptions::CommDialogException::IsErrorSet())
				{

					return false;
				}

				throw Exceptions::CommDialogException(
					"GetSaveFileNameA"
				);
			}

			path = FileSystem::Path(
				String(openfilename.lpstrFile)
			);

			return true;
#endif
		}
	};
}
