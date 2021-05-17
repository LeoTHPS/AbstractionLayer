#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include <cderr.h>
#include <commdlg.h>

namespace AL::Exceptions
{
	class CommDialogException
		: public Exception
	{
		typedef uint32 ErrorCode;

	public:
		static bool IsErrorSet()
		{
			return GetLastError() != 0;
		}

		CommDialogException()
			: Exception(
				GetLastErrorString()
			)
		{
		}

		template<size_t S>
		explicit CommDialogException(const char(&function)[S])
			: Exception(
				"Error calling '%s': %s",
				&function[0],
				GetLastErrorString().GetCString()
			)
		{
		}

	private:
		static String GetErrorString(ErrorCode errorCode)
		{
			switch (errorCode)
			{
				case CDERR_DIALOGFAILURE:
					return "The dialog box could not be created.";

				case CDERR_FINDRESFAILURE:
					return "The common dialog box function failed to find a specified resource.";

				case CDERR_INITIALIZATION:
					return "The common dialog box function failed during initialization.";

				case CDERR_LOADRESFAILURE:
					return "The common dialog box function failed to load a specified resource.";

				case CDERR_LOADSTRFAILURE:
					return "The common dialog box function failed to load a specified string.";

				case CDERR_LOCKRESFAILURE:
					return "The common dialog box function failed to lock a specified resource.";

				case CDERR_MEMALLOCFAILURE:
					return "The common dialog box function was unable to allocate memory for internal structures.";

				case CDERR_MEMLOCKFAILURE:
					return "The common dialog box function was unable to lock the memory associated with a handle.";

				case CDERR_NOHINSTANCE:
					return "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding instance handle.";

				case CDERR_NOHOOK:
					return "The ENABLEHOOK flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a pointer to a corresponding hook procedure.";

				case CDERR_NOTEMPLATE:
					return "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding template.";

				case CDERR_REGISTERMSGFAIL:
					return "The RegisterWindowMessage function returned an error code when it was called by the common dialog box function.";

				case CDERR_STRUCTSIZE:
					return "The lStructSize member of the initialization structure for the corresponding common dialog box is invalid.";

				case PDERR_CREATEICFAILURE:
					return "The PrintDlg function failed when it attempted to create an information context.";

				case PDERR_DEFAULTDIFFERENT:
					return "You called the PrintDlg function with the DN_DEFAULTPRN flag specified in the wDefault member of the DEVNAMES structure, but the printer described by the other structure members did not match the current default printer.";

				case PDERR_DNDMMISMATCH:
					return "The data in the DEVMODE and DEVNAMES structures describes two different printers.";

				case PDERR_GETDEVMODEFAIL:
					return "The printer driver failed to initialize a DEVMODE structure.";

				case PDERR_INITFAILURE:
					return "The PrintDlg function failed during initialization, and there is no more specific extended error code to describe the failure. ";

				case PDERR_LOADDRVFAILURE:
					return "The PrintDlg function failed to load the device driver for the specified printer.";

				case PDERR_NODEFAULTPRN:
					return "A default printer does not exist.";

				case PDERR_NODEVICES:
					return "No printer drivers were found.";

				case PDERR_PARSEFAILURE:
					return "The PrintDlg function failed to parse the strings in the [devices] section of the WIN.INI file.";

				case PDERR_PRINTERNOTFOUND:
					return "The [devices] section of the WIN.INI file did not contain an entry for the requested printer.";

				case PDERR_RETDEFFAILURE:
					return "The PD_RETURNDEFAULT flag was specified in the Flags member of the PRINTDLG structure, but the hDevMode or hDevNames member was not NULL.";

				case PDERR_SETUPFAILURE:
					return "The PrintDlg function failed to load the required resources.";

				case CFERR_MAXLESSTHANMIN:
					return "The size specified in the nSizeMax member of the CHOOSEFONT structure is less than the size specified in the nSizeMin member.";

				case CFERR_NOFONTS:
					return "No fonts exist.";

				case FNERR_BUFFERTOOSMALL:
					return "The buffer pointed to by the lpstrFile member of the OPENFILENAME structure is too small for the file name specified by the user. ";

				case FNERR_INVALIDFILENAME:
					return "A file name is invalid.";

				case FNERR_SUBCLASSFAILURE:
					return "An attempt to subclass a list box failed because sufficient memory was not available.";

				case FRERR_BUFFERLENGTHZERO:
					return "A member of the FINDREPLACE structure points to an invalid buffer.";
			}

			return "Undefined";
		}

		static ErrorCode GetLastError()
		{
			return static_cast<ErrorCode>(
				::CommDlgExtendedError()
			);
		}

		static String GetLastErrorString()
		{
			return GetErrorString(
				GetLastError()
			);
		}
	};
}
