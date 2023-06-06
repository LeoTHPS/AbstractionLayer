#pragma once
#include "AL/Common.hpp"

#include "Widget.hpp"

namespace AL::Game::Engine::GUI
{
	class Label
		: public Widget
	{
		SolidColorBrush textBrush;
		TextFormat      textFormat;
		TextLayout      textLayout;

	public:
		Label(WidgetManager& manager, String&& name, const WidgetSize& size = WidgetSize(), const WidgetPosition& position = WidgetPosition())
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
				"AL::Game::Engine::GUI::Label",
				GetSize().Cast<Float>()
			)
		{
			SetBorderColor(
				Colors::Transparent
			);

			SetBackgroundColor(
				Colors::Transparent
			);
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

		auto& GetTextAlignment() const
		{
			return textFormat.GetAlignment();
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
		Void SetTextAlignment(TextAlignment value)
		{
			textFormat.SetAlignment(
				value
			);

			textLayout.SetFormat(
				textFormat
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
		virtual Void OnSizeChanged(const WidgetSize& size) override
		{
			Widget::OnSizeChanged(
				size
			);

			textLayout.SetSize(
				size.Cast<Float>()
			);
		}
	};
}
