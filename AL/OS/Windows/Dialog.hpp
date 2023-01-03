#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
{
	enum class DialogIcons : uint32
	{
		Hand        = MB_ICONHAND,
		Stop        = MB_ICONSTOP,
		Error       = MB_ICONERROR,
		UserIcon    = MB_USERICON,
		Warning     = MB_ICONWARNING,
		Question    = MB_ICONQUESTION,
		Asterisk    = MB_ICONASTERISK,
		Exclamation = MB_ICONEXCLAMATION,
		Information = MB_ICONINFORMATION
	};

	enum class DialogButtons : uint32
	{
		OK                = MB_OK,
		OkCancel          = MB_OKCANCEL,

		YesNo             = MB_YESNO,
		YesNoCancel       = MB_YESNOCANCEL,

		RetryCancel       = MB_RETRYCANCEL,

		AbortRetryIgnore  = MB_ABORTRETRYIGNORE,

		CancelTryContinue = MB_CANCELTRYCONTINUE
	};

	enum class DialogDefaultButtons : uint32
	{
		Button1 = MB_DEFBUTTON1,
		Button2 = MB_DEFBUTTON2,
		Button3 = MB_DEFBUTTON3,
		Button4 = MB_DEFBUTTON4
	};

	enum class DialogResults : uint32
	{
		Yes      = IDYES,
		No       = IDNO,

		OK       = IDOK,

		Help     = IDHELP,

		Abort    = IDABORT,
		Cancel   = IDCANCEL,

		Retry    = IDRETRY,
		TryAgain = IDTRYAGAIN,

		Ignore   = IDIGNORE,
		Close    = IDCLOSE,

		Continue = IDCONTINUE,

		Timeout  = IDTIMEOUT
	};

	class Dialog
	{
		Bool                 isTopMost = False;

		::HWND               hWND = NULL;

		DialogIcons          icon;
		String               title;
		String               message;
		DialogButtons        buttons;
		DialogDefaultButtons defaultButton;

		Dialog(const Dialog&) = delete;

	public:
		// @throw AL::Exception
		static DialogResults Show(const String& title, const String& message)
		{
			Dialog dialog;
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon, DialogButtons buttons)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);
			dialog.SetDefaultButton(defaultButton);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton, Bool topMost)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);
			dialog.SetDefaultButton(defaultButton);
			dialog.SetTopMost(topMost);

			return dialog.Show();
		}

		// @throw AL::Exception
		static DialogResults Show(::HWND hWND, const String& title, const String& message)
		{
			Dialog dialog(hWND);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(::HWND hWND, const String& title, const String& message, DialogIcons icon)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(::HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(::HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);
			dialog.SetDefaultButton(defaultButton);

			return dialog.Show();
		}
		// @throw AL::Exception
		static DialogResults Show(::HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton, Bool topMost)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);
			dialog.SetDefaultButton(defaultButton);
			dialog.SetTopMost(topMost);

			return dialog.Show();
		}

		Dialog()
		{
		}

		Dialog(Dialog&& dialog)
			: isTopMost(
				dialog.isTopMost
			),
			hWND(
				dialog.hWND
			),
			icon(
				dialog.icon
			),
			title(
				Move(dialog.title)
			),
			message(
				Move(dialog.message)
			),
			buttons(
				dialog.buttons
			),
			defaultButton(
				dialog.defaultButton
			)
		{
			dialog.hWND = NULL;
		}

		explicit Dialog(HWND hWND)
			: hWND(
				hWND
			)
		{
		}

		virtual ~Dialog()
		{
		}

		Bool IsTopMost() const
		{
			return isTopMost;
		}

		auto GetIcon() const
		{
			return icon;
		}

		auto& GetTitle() const
		{
			return title;
		}

		auto& GetMessage() const
		{
			return message;
		}

		auto GetButtons() const
		{
			return buttons;
		}

		auto GetDefaultButton() const
		{
			return defaultButton;
		}

		Void SetTopMost(Bool set = True)
		{
			isTopMost = set;
		}

		Void SetIcon(DialogIcons value)
		{
			icon = value;
		}

		Void SetTitle(String&& value)
		{
			title = Move(
				value
			);
		}
		Void SetTitle(const String& value)
		{
			SetTitle(
				String(value)
			);
		}

		Void SetMessage(String&& value)
		{
			message = Move(
				value
			);
		}
		Void SetMessage(const String& value)
		{
			SetMessage(
				String(value)
			);
		}

		Void SetButtons(DialogButtons value)
		{
			buttons = value;
		}

		Void SetDefaultButton(DialogDefaultButtons value)
		{
			defaultButton = value;
		}

		// @throw AL::Exception
		DialogResults Show() const
		{
			BitMask<::UINT> flags(
				hWND ? MB_APPLMODAL : MB_TASKMODAL
			);

			flags.Add(static_cast<::UINT>(GetIcon()));
			flags.Add(static_cast<::UINT>(GetButtons()));
			flags.Add(static_cast<::UINT>(GetDefaultButton()));
			
			flags.Set(MB_TOPMOST, IsTopMost());

			::MSGBOXPARAMSA params = { };
			params.cbSize = sizeof(::MSGBOXPARAMSA);
			params.hwndOwner = hWND;
			params.dwStyle = flags.Value;
			params.lpszText = GetMessage().GetCString();
			params.lpszCaption = GetTitle().GetCString();

			if (auto result = ::MessageBoxIndirectA(&params))
			{

				return static_cast<DialogResults>(
					result
				);
			}

			throw SystemException(
				"MessageBoxIndirectA"
			);
		}
	};
}
