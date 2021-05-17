#pragma once
#include "AL/Common.hpp"

namespace AL::OS
{
	enum class DialogIcons : uint32
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Hand        = MB_ICONHAND,
		Stop        = MB_ICONSTOP,
		Error       = MB_ICONERROR,
		UserIcon    = MB_USERICON,
		Warning     = MB_ICONWARNING,
		Question    = MB_ICONQUESTION,
		Asterisk    = MB_ICONASTERISK,
		Exclamation = MB_ICONEXCLAMATION,
		Information = MB_ICONINFORMATION
#endif
	};

	enum class DialogButtons : uint32
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		OK                = MB_OK,
		OkCancel          = MB_OKCANCEL,

		YesNo             = MB_YESNO,
		YesNoCancel       = MB_YESNOCANCEL,

		RetryCancel       = MB_RETRYCANCEL,

		AbortRetryIgnore  = MB_ABORTRETRYIGNORE,

		CancelTryContinue = MB_CANCELTRYCONTINUE
#endif
	};

	enum class DialogDefaultButtons : uint32
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Button1 = MB_DEFBUTTON1,
		Button2 = MB_DEFBUTTON2,
		Button3 = MB_DEFBUTTON3,
		Button4 = MB_DEFBUTTON4
#endif
	};

	enum class DialogResults : uint32
	{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
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
#endif
	};

	class Dialog
	{
		bool isTopMost = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		HWND hWND = NULL;
#endif

		String title;
		String message;
		DialogIcons icon;
		DialogButtons buttons;
		DialogDefaultButtons defaultButton;

		Dialog(const Dialog&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		static DialogResults Show(const String& title, const String& message)
		{
			Dialog dialog;
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon, DialogButtons buttons)
		{
			Dialog dialog;
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
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
		// @throw AL::Exceptions::Exception
		static DialogResults Show(const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton, bool topMost)
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

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		static DialogResults Show(HWND hWND, const String& title, const String& message)
		{
			Dialog dialog(hWND);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(HWND hWND, const String& title, const String& message, DialogIcons icon)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton)
		{
			Dialog dialog(hWND);
			dialog.SetIcon(icon);
			dialog.SetTitle(title);
			dialog.SetMessage(message);
			dialog.SetButtons(buttons);
			dialog.SetDefaultButton(defaultButton);

			return dialog.Show();
		}
		// @throw AL::Exceptions::Exception
		static DialogResults Show(HWND hWND, const String& title, const String& message, DialogIcons icon, DialogButtons buttons, DialogDefaultButtons defaultButton, bool topMost)
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
#endif

		Dialog()
		{
		}

		Dialog(Dialog&& dialog)
			: isTopMost(
				dialog.isTopMost
			),
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			hWND(
				dialog.hWND
			),
#endif
			title(
				Move(dialog.title)
			),
			message(
				Move(dialog.message)
			),
			icon(
				dialog.icon
			),
			buttons(
				dialog.buttons
			),
			defaultButton(
				dialog.defaultButton
			)
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			dialog.hWND = NULL;
#endif
		}

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		explicit Dialog(HWND hWND)
			: hWND(
				hWND
			)
		{
		}
#endif

		virtual ~Dialog()
		{
		}

		bool IsTopMost() const
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

		void SetTopMost(bool set = true)
		{
			isTopMost = set;
		}

		void SetIcon(DialogIcons value)
		{
			icon = value;
		}

		void SetTitle(String&& value)
		{
			title = Move(
				value
			);
		}
		void SetTitle(const String& value)
		{
			SetTitle(
				String(value)
			);
		}

		void SetMessage(String&& value)
		{
			message = Move(
				value
			);
		}
		void SetMessage(const String& value)
		{
			SetMessage(
				String(value)
			);
		}

		void SetButtons(DialogButtons value)
		{
			buttons = value;
		}

		void SetDefaultButton(DialogDefaultButtons value)
		{
			defaultButton = value;
		}

		// @throw AL::Exceptions::Exception
		DialogResults Show() const
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			AL::BitMask<UINT> flags(
				hWND ? MB_APPLMODAL : MB_TASKMODAL
			);

			flags.Add(static_cast<UINT>(GetIcon()));
			flags.Add(static_cast<UINT>(GetButtons()));
			flags.Add(static_cast<UINT>(GetDefaultButton()));
			
			flags.Set(MB_TOPMOST, IsTopMost());

			MSGBOXPARAMSA params = { 0 };
			params.cbSize = sizeof(MSGBOXPARAMSA);
			params.hwndOwner = hWND;
			params.dwStyle = flags.Mask;
			params.lpszText = GetMessage().GetCString();
			params.lpszCaption = GetTitle().GetCString();

			if (auto result = MessageBoxIndirectA(&params))
			{

				return static_cast<DialogResults>(
					result
				);
			}

			throw Exceptions::SystemException(
				"MessageBoxIndirectA"
			);
#endif
		}
	};
}
