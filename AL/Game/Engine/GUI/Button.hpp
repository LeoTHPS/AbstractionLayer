#pragma once
#include "AL/Common.hpp"

#include "Widget.hpp"

namespace AL::Game::Engine::GUI
{
	// @throw AL::Exception
	typedef EventHandler<Void(Hardware::MouseButtons button)> ButtonOnClickEventHandler;

	class Button
		: public Widget
	{
		SolidColorBrush textBrush;
		TextFormat      textFormat;
		TextLayout      textLayout;

	public:
		// @throw AL::Exception
		Event<ButtonOnClickEventHandler> OnClick;

		Button(WidgetManager& manager, String&& name, const WidgetSize& size = WidgetSize(), const WidgetPosition& position = WidgetPosition())
			: Widget(
				manager,
				Move(name),
				size,
				position
			),
			textBrush(
				GetGraphics(),
				Colors::Black
			),
			textFormat(
				GetGraphics(),
				Font
				{
					.Name = "Consolas",
					.Size = 14.0f
				},
				TextAlignment::Center_H | TextAlignment::Center_V
			),
			textLayout(
				GetGraphics(),
				textFormat,
				"AL::Game::Engine::GUI::Button",
				GetSize().Cast<Float>()
			)
		{
			SetCursor(
				WindowCursors::Hand
			);

			SetBorderColor(
				Colors::Transparent
			);

			SetBackgroundColor(
				Colors::Transparent
			);

			SetInputEnabled();
		}

		auto& GetFont() const
		{
			return textFormat.GetFont();
		}

		auto GetFontColor() const
		{
			return textBrush.GetColor();
		}

		auto& GetText() const
		{
			return textLayout.GetText();
		}

		// @throw AL::Exception
		Void SetFont(Font&& value)
		{
			textFormat.SetFont(
				Move(value)
			);
		}
		// @throw AL::Exception
		Void SetFont(const Font& value)
		{
			Font font =
			{
				.Name = value.Name,
				.Size = value.Size
			};

			SetFont(
				Move(font)
			);
		}
		// @throw AL::Exception
		Void SetFont(const String& name, Float size)
		{
			Font font = 
			{
				.Name = name,
				.Size = size
			};

			SetFont(
				Move(font)
			);
		}

		// @throw AL::Exception
		Void SetFontColor(Color value)
		{
			textBrush.SetColor(
				value
			);
		}

		// @throw AL::Exception
		Void SetText(String&& value)
		{
			textLayout.SetText(
				Move(value)
			);
		}
		// @throw AL::Exception
		Void SetText(const String& value)
		{
			String string(
				value
			);

			SetText(
				Move(string)
			);
		}

		// @throw AL::Exception
		Void Click(Hardware::MouseButtons button)
		{
			SetFocus();

			OnClick.Execute(
				button
			);
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			Widget::OnDraw(
				delta
			);

			if (GetFontColor().A != 0x00)
			{
				GetGraphics().DrawTextLayout(
					GetPosition(),
					textBrush,
					textLayout
				);
			}
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
		{
			if (Widget::OnMouseEvent(event))
			{

				return True;
			}

			if (event.Type == Hardware::MouseEvents::ButtonDown)
			{
				Click(
					event.Button
				);

				return True;
			}

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
		{
			if (Widget::OnKeyboardEvent(event))
			{

				return True;
			}

			if (IsFocus())
			{
				if (event.Type == Hardware::KeyboardEvents::KeyDown)
				{
					if (event.Key == Hardware::Keys::Enter)
					{
						Click(
							Hardware::MouseButtons::Left
						);

						return True;
					}
				}
			}

			return False;
		}
	};
}
