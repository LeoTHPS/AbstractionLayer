#pragma once
#include "AL/Common.hpp"

#include "Widget.hpp"

#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

#include "AL/Game/Engine/WindowComponent.hpp"

namespace AL::Game::Engine::GUI
{
	class WidgetManager
		: public WindowComponent
	{
		Bool                                     isDebugInfoShown = False;

		Widget*                                  lpFocus = nullptr;
		Widget*                                  lpTarget = nullptr;

		Collections::LinkedList<Widget*>         widgets;
		Collections::Dictionary<String, Widget*> widgetsByName;

		SolidColorBrush                          debugBorder;
		SolidColorBrush                          debugBackground;
		SolidColorBrush                          debugForeground;
		TextFormat                               debugTextFormat;

	public:
		explicit WidgetManager(Window& window)
			: WindowComponent(
				window
			),
			debugBorder(
				GetGraphics(),
				Color(Colors::Green, 200)
			),
			debugBackground(
				GetGraphics(),
				Color(Colors::Green, 150)
			),
			debugForeground(
				GetGraphics(),
				Colors::White
			),
			debugTextFormat(
				GetGraphics(),
				Font
				{
					.Name = "Consolas",
					.Size = 8.0f
				}
			)
		{
		}

		virtual ~WidgetManager()
		{
		}

		auto GetCount() const
		{
			return widgets.GetSize();
		}

		auto GetFocus() const
		{
			return lpFocus;
		}

		auto GetTarget() const
		{
			return lpTarget;
		}

		Widget* FindWidgetByName(const String& value) const
		{
			auto it = widgetsByName.Find(
				value
			);

			return (it != widgetsByName.end()) ? it->Value : nullptr;
		}

		template<typename T>
		Widget* FindWidgetByPosition(const Vector2<T>& value) const
		{
			auto _value = value.template Cast<typename WidgetPosition::Type>();

			for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
			{
				auto lpWidget           = *it;
				auto& lpWidget_Size     = lpWidget->GetSize();
				auto& lpWidget_Position = lpWidget->GetPosition();

				if (!lpWidget->IsEnabled())
				{

					continue;
				}

				if ((_value.X >= lpWidget_Position.X) && (_value.Y >= lpWidget_Position.Y) &&
					(_value.X <= (lpWidget_Position.X + lpWidget_Size.Width)) && (_value.Y <= (lpWidget_Position.Y + lpWidget_Size.Height)))
				{

					return lpWidget;
				}
			}

			return nullptr;
		}

		// @throw AL::Exception
		Void SetFocus(Widget* lpWidget)
		{
			if (!lpWidget || lpWidget->IsEnabled())
			{
				if (auto lpWidget = GetFocus())
				{
					lpWidget->Focus(
						False
					);
				}

				if ((lpFocus = lpWidget) != nullptr)
				{
					lpWidget->Focus();
				}
			}
		}

		// @throw AL::Exception
		Void SetTarget(Widget* lpWidget)
		{
			if (!lpWidget || lpWidget->IsEnabled())
			{
				if (auto lpWidget = GetTarget())
				{
					lpWidget->Target(
						False
					);
				}

				if ((lpTarget = lpWidget) != nullptr)
				{
					lpWidget->Target();
				}
			}
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnCreate() override
		{
			WindowComponent::OnCreate();

			for (auto it = widgets.begin(); it != widgets.end(); ++it)
			{
				auto lpWidget = *it;

				try
				{
					lpWidget->Create();
				}
				catch (Exception& exception)
				{
					while (it != widgets.begin())
					{
						(*(--it))->Destroy();
					}

					WindowComponent::OnDestroy();

					throw Exception(
						Move(exception),
						"Error creating Widget [Name: %s]",
						lpWidget->GetName().GetCString()
					);
				}
			}
		}

		virtual Void OnDestroy() override
		{
			for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
			{
				(*it)->Destroy();
			}

			WindowComponent::OnDestroy();
		}

		// @throw AL::Exception
		virtual Void OnLoadContent() override
		{
			WindowComponent::OnLoadContent();

			for (auto it = widgets.begin(); it != widgets.end(); ++it)
			{
				auto lpWidget = *it;

				try
				{
					lpWidget->LoadContent();
				}
				catch (Exception& exception)
				{
					while (it != widgets.begin())
					{
						(*(--it))->UnloadContent();
					}

					WindowComponent::OnUnloadContent();

					throw Exception(
						Move(exception),
						"Error loading Widget content [Name: %s]",
						lpWidget->GetName().GetCString()
					);
				}
			}
		}

		virtual Void OnUnloadContent() override
		{
			for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
			{
				(*it)->UnloadContent();
			}

			WindowComponent::OnUnloadContent();
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			WindowComponent::OnDraw(
				delta
			);

			for (auto lpWidget : widgets)
			{
				if (!lpWidget->IsEnabled())
				{

					continue;
				}

				try
				{
					lpWidget->Draw(
						delta
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error drawing Widget [Name: %s]",
						lpWidget->GetName().GetCString()
					);
				}

				if (isDebugInfoShown)
				{
					GetGraphics().FillRectangle(
						lpWidget->GetPosition(),
						debugBackground,
						lpWidget->GetSize().Width,
						lpWidget->GetSize().Height
					);

					GetGraphics().DrawRectangle(
						lpWidget->GetPosition(),
						debugBorder,
						lpWidget->GetSize().Width,
						lpWidget->GetSize().Height
					);

					GetGraphics().DrawString(
						lpWidget->GetPosition(),
						debugForeground,
						debugTextFormat,
						lpWidget->GetName()
					);
				}
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

			for (auto lpWidget : widgets)
			{
				if (!lpWidget->IsEnabled())
				{

					continue;
				}

				try
				{
					lpWidget->Update(
						delta
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error updating Widget [Name: %s]",
						lpWidget->GetName().GetCString()
					);
				}
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

			switch (event.Type)
			{
				case Hardware::MouseEvents::Move:
				{
					if (auto lpWidget = FindWidgetByPosition(event.Position))
					{
						if (!lpWidget->IsInputEnabled())
						{
							try
							{
								SetTarget(
									nullptr
								);
							}
							catch (Exception& exception)
							{

								throw Exception(
									Move(exception),
									"Error clearing target"
								);
							}

							return True;
						}

						try
						{
							SetTarget(
								lpWidget
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error targetting Widget [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						try
						{
							lpWidget->HandleMouseEvent(
								event
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error handling Widget mouse event [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						return True;
					}

					SetTarget(
						nullptr
					);
				}
				break;

				case Hardware::MouseEvents::ButtonUp:
				{
					if (auto lpWidget = GetFocus())
					{
						try
						{
							lpWidget->HandleMouseEvent(
								event
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error handling Widget mouse event [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						return True;
					}
				}
				break;

				case Hardware::MouseEvents::ButtonDown:
				{
					if (auto lpWidget = FindWidgetByPosition(event.Position))
					{
						if (!lpWidget->IsInputEnabled())
						{
							try
							{
								SetFocus(
									nullptr
								);
							}
							catch (Exception& exception)
							{

								throw Exception(
									Move(exception),
									"Error clearing focus"
								);
							}

							return True;
						}

						try
						{
							SetFocus(
								lpWidget
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error focusing Widget [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						try
						{
							lpWidget->HandleMouseEvent(
								event
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error handling Widget mouse event [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						return True;
					}

					SetFocus(
						nullptr
					);
				}
				break;

				case Hardware::MouseEvents::ScrollUp:
				case Hardware::MouseEvents::ScrollDown:
				{
					if (auto lpWidget = FindWidgetByPosition(event.Position))
					{
						if (!lpWidget->IsInputEnabled())
						{

							return True;
						}

						try
						{
							lpWidget->HandleMouseEvent(
								event
							);
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error handling Widget mouse event [Name: %s]",
								lpWidget->GetName().GetCString()
							);
						}

						return True;
					}
				}
				break;
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

			if (auto lpWidget = GetFocus())
			{
				try
				{
					lpWidget->HandleKeyboardEvent(
						event
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error handling Widget keyboard event [Name: %s]",
						lpWidget->GetName().GetCString()
					);
				}

				return True;
			}

			if (event.Type == Hardware::KeyboardEvents::KeyDown)
			{
				if (event.Key == Hardware::Keys::F1)
				{
					isDebugInfoShown = !isDebugInfoShown;
				}
			}

			return False;
		}

	private: // Widget access
		friend Widget;

		Void AddWidget(Widget& widget)
		{
			widgets.PushBack(
				&widget
			);

			widgetsByName.Add(
				widget.GetName(),
				&widget
			);
		}

		Void RemoveWidget(Widget& widget)
		{
			widgetsByName.Remove(
				widget.GetName()
			);

			widgets.Remove(
				&widget
			);
		}
	};
}

inline AL::Game::Engine::GUI::Widget::Widget(WidgetManager& manager, String&& name, const WidgetSize& size, const WidgetPosition& position)
	: WindowComponent(
		manager.GetWindow()
	),
	name(
		Move(name)
	),
	size(
		size
	),
	position(
		position
	),
	cursor(
		WindowCursors::None
	),
	lpManager(
		&manager
	),
	borderBrush(
		GetGraphics(),
		Colors::Transparent
	),
	backgroundBrush(
		GetGraphics(),
		Colors::Transparent
	)
{
	GetManager().AddWidget(
		*this
	);
}

inline AL::Game::Engine::GUI::Widget::~Widget()
{
	GetManager().RemoveWidget(
		*this
	);
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::GUI::Widget::SetFocus(Bool set)
{
	if (set && !IsFocus())
	{
		GetManager().SetFocus(
			this
		);
	}
	else if (!set && IsFocus())
	{
		GetManager().SetFocus(
			nullptr
		);
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::GUI::Widget::SetTarget(Bool set)
{
	if (set && !IsTarget())
	{
		GetManager().SetTarget(
			this
		);
	}
	else if (!set && IsTarget())
	{
		GetManager().SetTarget(
			nullptr
		);
	}
}
