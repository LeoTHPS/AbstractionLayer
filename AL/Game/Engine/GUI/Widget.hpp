#pragma once
#include "AL/Common.hpp"

#include "AL/Game/Engine/WindowComponent.hpp"

namespace AL::Game::Engine::GUI
{
	class WidgetManager;

	typedef SizeU32    WidgetSize;
	typedef Vector2U32 WidgetPosition;

	class Widget
		: public WindowComponent
	{
		Bool                 isFocus        = False;
		Bool                 isTarget       = False;
		Bool                 isEnabled      = True;
		Bool                 isCursorSet    = False;
		Bool                 isInputEnabled = False;

		String               name;

		WidgetSize           size;
		WidgetPosition       position;

		WindowCursors        cursor;

		WidgetManager* const lpManager;

		SolidColorBrush      borderBrush;
		Float                borderWidth = 1.0f;
		Float                borderRadius = 0.0f;

		SolidColorBrush      backgroundBrush;

	public:
		Widget(WidgetManager& manager, String&& name, const WidgetSize& size = WidgetSize(), const WidgetPosition& position = WidgetPosition());

		virtual ~Widget();

		Bool IsFocus() const
		{
			return isFocus;
		}

		Bool IsTarget() const
		{
			return isTarget;
		}

		Bool IsEnabled() const
		{
			return isEnabled;
		}

		Bool IsInputEnabled() const
		{
			return isInputEnabled;
		}

		auto& GetName() const
		{
			return name;
		}

		auto& GetSize() const
		{
			return size;
		}

		auto& GetPosition() const
		{
			return position;
		}

		auto GetCursor() const
		{
			return cursor;
		}

		auto& GetManager()
		{
			return *lpManager;
		}
		auto& GetManager() const
		{
			return *lpManager;
		}

		auto GetBorderColor() const
		{
			return borderBrush.GetColor();
		}

		auto GetBorderWidth() const
		{
			return borderWidth;
		}

		auto GetBorderRadius() const
		{
			return borderRadius;
		}

		auto GetBackgroundColor() const
		{
			return backgroundBrush.GetColor();
		}

		// @throw AL::Exception
		Void SetFocus(Bool set = True);

		// @throw AL::Exception
		Void SetTarget(Bool set = True);

		// @throw AL::Exception
		Bool SetCursor(WindowCursors value)
		{
			cursor = value;

			if (isCursorSet)
			{
				if (!PopCursor())
				{

					return False;
				}

				isCursorSet = False;

				if (!PushCursor(value))
				{

					return False;
				}

				isCursorSet = True;
			}

			return True;
		}

		// @throw AL::Exception
		Void SetEnabled(Bool set = True)
		{
			if (!set)
			{
				SetFocus(False);
				SetTarget(False);
			}

			isEnabled = set;
		}

		Void SetInputEnabled(Bool set = True)
		{
			isInputEnabled = set;
		}

		// @throw AL::Exception
		Void SetSize(const WidgetSize& value)
		{
			SetSize(
				value.Width,
				value.Height
			);
		}
		// @throw AL::Exception
		Void SetSize(WidgetSize::Type width, WidgetSize::Type height)
		{
			size.Width  = width;
			size.Height = height;

			OnSizeChanged(
				GetSize()
			);
		}

		// @throw AL::Exception
		Void SetPosition(const WidgetPosition& value)
		{
			SetPosition(
				value.X,
				value.Y
			);
		}
		// @throw AL::Exception
		Void SetPosition(WidgetPosition::Type x, WidgetPosition::Type y)
		{
			position.X = x;
			position.Y = y;

			OnPositionChanged(
				GetPosition()
			);
		}

		// @throw AL::Exception
		Void SetBorderColor(Color value)
		{
			borderBrush.SetColor(
				value
			);
		}

		// @throw AL::Exception
		Void SetBorderWidth(Float value)
		{
			borderWidth = value;
		}

		// @throw AL::Exception
		Void SetBorderRadius(Float value)
		{
			borderRadius = value;
		}

		// @throw AL::Exception
		Void SetBackgroundColor(Color value)
		{
			backgroundBrush.SetColor(
				value
			);
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnFocus()
		{
		}

		// @throw AL::Exception
		virtual Void OnFocusLost()
		{
		}

		// @throw AL::Exception
		virtual Void OnTarget()
		{
			{
				WindowCursors cursor;

				if ((cursor = GetCursor()) != WindowCursors::None)
				{
					if (PushCursor(cursor))
					{

						isCursorSet = True;
					}
				}
			}
		}

		// @throw AL::Exception
		virtual Void OnTargetLost()
		{
			if (isCursorSet && PopCursor())
			{

				isCursorSet = False;
			}
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			WindowComponent::OnDraw(
				delta
			);

			if (GetBackgroundColor().A != 0x00)
			{
				GetGraphics().FillRoundedRectangle(
					GetPosition(),
					backgroundBrush,
					GetSize().Width,
					GetSize().Height,
					GetBorderRadius()
				);
			}

			if (GetBorderColor().A != 0x00)
			{
				GetGraphics().DrawRoundedRectangle(
					GetPosition(),
					borderBrush,
					GetSize().Width,
					GetSize().Height,
					GetBorderRadius(),
					GetBorderWidth()
				);
			}
		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta) override
		{
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

		}

		// @throw AL::Exception
		virtual Void OnSizeChanged(const WidgetSize& size)
		{
		}

		// @throw AL::Exception
		virtual Void OnPositionChanged(const WidgetPosition& position)
		{
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
		{
			if (WindowComponent::OnMouseEvent(event))
			{

				return True;
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

			return False;
		}

	private: // WidgetManager access
		friend WidgetManager;

		// @throw AL::Exception
		Void Focus(Bool set = True)
		{
			if (set && !IsFocus())
			{
				OnFocus();

				isFocus = True;
			}
			else if (!set && IsFocus())
			{
				OnFocusLost();

				isFocus = False;
			}
		}

		// @throw AL::Exception
		Void Target(Bool set = True)
		{
			if (set && !IsTarget())
			{
				OnTarget();

				isTarget = True;
			}
			else if (!set && IsTarget())
			{
				OnTargetLost();

				isTarget = False;
			}
		}
	};
}
