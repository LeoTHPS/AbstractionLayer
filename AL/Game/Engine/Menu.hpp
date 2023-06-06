#pragma once
#include "AL/Common.hpp"

#include "WindowComponent.hpp"

#include "GUI/Label.hpp"
#include "GUI/Panel.hpp"
#include "GUI/Button.hpp"
#include "GUI/WidgetManager.hpp"

#include "AL/Hardware/Keyboard.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL::Game::Engine
{
	typedef GUI::ButtonOnClickEventHandler MenuEntryOnClickEventHandler;

	struct MenuEntry
	{
		MenuEntryOnClickEventHandler OnClick;

		String                       Name;
		Engine::Font                 Font;
		String                       Text;
		Engine::Color                Background;
		Engine::Color                Foreground;
	};

	struct MenuTitle
	{
		Engine::Font  Font;
		String        Text;
		Engine::Color Color;
	};

	class Menu
		: public WindowComponent
	{
		typedef Collections::LinkedList<GUI::Button*> _MenuEntryList;

		Bool               isOpen = False;

		Hardware::Keys     hotkey = Hardware::Keys::Escape;

		SolidColorBrush    backgroundBrush;
		GUI::WidgetManager widgetManager;
		GUI::Label         lblTitle;
		_MenuEntryList     entries;

		Menu(Menu&&) = delete;
		Menu(const Menu&) = delete;

	public:
		explicit Menu(Window& window)
			: WindowComponent(
				window
			),
			backgroundBrush(
				GetGraphics(),
				Color(Colors::Black, 0x7F)
			),
			widgetManager(
				GetWindow()
			),
			lblTitle(
				widgetManager,
				"lblTitle",
				GUI::WidgetSize(0, 50)
			)
		{
			SetTitle(
				{
					.Font =
					{
						.Name = "Consolas",
						.Size = 34.0f
					},
					.Text  = "AL::Game::Engine::Menu",
					.Color = Colors::White
				}
			);
		}

		virtual ~Menu()
		{
			Clear();
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		auto& GetTitle() const
		{
			return lblTitle.GetText();
		}

		auto& GetTitleFont() const
		{
			return lblTitle.GetFont();
		}

		auto GetTitleColor() const
		{
			return lblTitle.GetFontColor();
		}

		auto GetHotkey() const
		{
			return hotkey;
		}

		auto& GetWidgetManager()
		{
			return widgetManager;
		}
		auto& GetWidgetManager() const
		{
			return widgetManager;
		}

		auto GetBackgroundColor() const
		{
			return backgroundBrush.GetColor();
		}

		Void Open()
		{
			if (!IsOpen())
			{

				isOpen = True;
			}
		}

		Void Close()
		{
			if (IsOpen())
			{

				isOpen = False;
			}
		}

		Void Clear()
		{
			for (auto it = entries.rbegin(); it != entries.rend(); )
			{
				delete *it;

				entries.Erase(
					it++
				);
			}
		}

		Void SetHotkey(Hardware::Keys value)
		{
			hotkey = value;
		}

		// @throw AL::Exception
		Void SetTitle(MenuTitle&& value)
		{
			lblTitle.SetText(
				Move(value.Text)
			);

			lblTitle.SetFont(
				Move(value.Font)
			);

			lblTitle.SetFontColor(
				value.Color
			);
		}

		// @throw AL::Exception
		Void SetTitleText(String&& value)
		{
			lblTitle.SetText(
				Move(value)
			);
		}
		// @throw AL::Exception
		Void SetTitleText(const String& value)
		{
			String string(
				value
			);

			SetTitleText(
				Move(string)
			);
		}

		// @throw AL::Exception
		Void AddEntry(MenuEntry&& value)
		{
			auto lpButton = new GUI::Button(
				GetWidgetManager(),
				Move(value.Name),
				GUI::WidgetSize(250, 25),
				GUI::WidgetPosition(
					(GetGraphics().GetTargetSize().Width / 2) - (250 / 2),
					(lblTitle.GetPosition().Y + lblTitle.GetSize().Height) + 50
				)
			);

			lpButton->OnClick.Register(
				Move(value.OnClick)
			);

			try
			{
				lpButton->SetFont(
					Move(value.Font)
				);

				lpButton->SetText(
					Move(value.Text)
				);

				lpButton->SetFontColor(
					value.Foreground
				);

				lpButton->SetBackgroundColor(
					value.Background
				);

				if (entries.GetSize() != 0)
				{
					auto lpPrevButton = *(--entries.end());

					lpButton->SetPosition(
						(lpPrevButton->GetPosition().Y + lpPrevButton->GetSize().Height) + 10,
						lpPrevButton->GetPosition().X
					);
				}
			}
			catch (Exception& exception)
			{
				delete lpButton;

				throw Exception(
					Move(exception),
					"Error initializing GUI::Button"
				);
			}

			entries.PushBack(
				lpButton
			);
		}
		// @throw AL::Exception
		Void AddEntry(String&& name, String&& text, MenuEntryOnClickEventHandler&& onClick)
		{
			AddEntry(
				{
					.OnClick    = Move(onClick),
					.Name       = Move(name),
					.Font       =
					{
						.Name   = lblTitle.GetFont().Name,
						.Size   = 18.0f
					},
					.Text       = Move(text),
					.Background = Color(Colors::White, 0x7F),
					.Foreground = Colors::Black
				}
			);
		}

		// @throw AL::Exception
		Void SetBackgroundColor(Color value)
		{
			backgroundBrush.SetColor(value);
		}

		// @throw AL::Exception
		Void Draw(Float delta)
		{
			if (IsOpen())
			{
				WindowComponent::Draw(
					delta
				);
			}
		}

		// @throw AL::Exception
		Void Update(TimeSpan delta)
		{
			if (IsOpen())
			{
				WindowComponent::Update(
					delta
				);
			}
		}

		// @throw AL::Exception
		// @return AL::True if handled
		Bool HandleMouseEvent(const Hardware::MouseEvent& event)
		{
			if (IsOpen())
			{
				WindowComponent::HandleMouseEvent(
					event
				);

				return True;
			}

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		Bool HandleKeyboardEvent(const Hardware::KeyboardEvent& event)
		{
			if (event.Type == Hardware::KeyboardEvents::KeyDown)
			{
				if (event.Key == GetHotkey())
				{
					(this->*(IsOpen() ? &Menu::Close : &Menu::Open))();

					return True;
				}
			}

			if (IsOpen())
			{
				WindowComponent::HandleKeyboardEvent(
					event
				);

				return True;
			}

			return False;
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnCreate() override
		{
			WindowComponent::OnCreate();

			try
			{
				GetWidgetManager().Create();
			}
			catch (Exception& exception)
			{
				WindowComponent::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating GUI::WidgetManager"
				);
			}
		}

		virtual Void OnDestroy() override
		{
			GetWidgetManager().Destroy();

			WindowComponent::OnDestroy();
		}

		// @throw AL::Exception
		virtual Void OnLoadContent() override
		{
			WindowComponent::OnLoadContent();

			lblTitle.SetSize(
				GetGraphics().GetTargetSize().Width,
				lblTitle.GetSize().Height
			);

			try
			{
				GetWidgetManager().LoadContent();
			}
			catch (Exception& exception)
			{
				WindowComponent::OnUnloadContent();

				throw Exception(
					Move(exception),
					"Error loading GUI::WidgetManager content"
				);
			}
		}

		virtual Void OnUnloadContent() override
		{
			GetWidgetManager().UnloadContent();

			WindowComponent::OnUnloadContent();
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			WindowComponent::OnDraw(
				delta
			);

			auto targetSize = GetGraphics().GetTargetSize();

			GetGraphics().FillRectangle(
				Vector2U32(),
				backgroundBrush,
				targetSize.Width,
				targetSize.Height
			);
		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta) override
		{
			try
			{
				GetWidgetManager().Draw(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error drawing GUI::WidgetManager"
				);
			}

			WindowComponent::OnEndDraw(
				delta
			);
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta) override
		{
			WindowComponent::OnUpdate(
				delta
			);

			try
			{
				GetWidgetManager().Update(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating GUI::WidgetManager"
				);
			}
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
		{
			if (WindowComponent::OnMouseEvent(event))
			{

				return True;
			}

			try
			{
				if (GetWidgetManager().HandleMouseEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling GUI::WidgetManager mouse event"
				);
			}

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
		{
			if (WindowComponent::OnKeyboardEvent(event))
			{

				return True;
			}

			try
			{
				if (GetWidgetManager().HandleKeyboardEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling GUI::WidgetManager keyboard event"
				);
			}

			return False;
		}
	};
}
