#pragma once
#include "AL/Common.hpp"

#include "WindowComponent.hpp"

#include "GUI/WidgetManager.hpp"

#include "AL/Algorithms/FNV.hpp"

namespace AL::Game::Engine
{
	class SceneManager;

	class SceneID
	{
		uint64 value;

	public:
		SceneID()
			: SceneID(
				0
			)
		{
		}

		SceneID(uint64 value)
			: value(
				value
			)
		{
		}

		SceneID(const String& value)
			: value(
				Algorithms::FNV64::Calculate(
					value.GetCString(),
					value.GetLength()
				)
			)
		{
		}

		template<size_t S>
		SceneID(const char(&value)[S])
			: value(
				Algorithms::FNV64::Calculate(
					value
				)
			)
		{
		}

		virtual ~SceneID()
		{
		}

		auto GetValue() const
		{
			return value;
		}

		operator uint64() const
		{
			return GetValue();
		}

		Bool operator == (const SceneID& id) const
		{
			if (GetValue() != id.GetValue())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const SceneID& id) const
		{
			if (operator==(id))
			{

				return False;
			}

			return True;
		}
	};

	class Scene
		: public WindowComponent
	{
		Bool                isActive = False;

		SceneID             id;
		String              name;
		SceneManager* const lpManager;
		GUI::WidgetManager  widgetManager;

		Scene(Scene&&) = delete;
		Scene(const Scene&) = delete;

	public:
		Scene(SceneManager& manager, String&& name);

		virtual ~Scene()
		{
		}

		Bool IsActive() const
		{
			return isActive;
		}

		auto& GetID() const
		{
			return id;
		}

		auto& GetName() const
		{
			return name;
		}

		auto& GetManager()
		{
			return *lpManager;
		}
		auto& GetManager() const
		{
			return *lpManager;
		}

		auto& GetWidgetManager()
		{
			return widgetManager;
		}
		auto& GetWidgetManager() const
		{
			return widgetManager;
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

		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta) override
		{
			WindowComponent::OnEndDraw(
				delta
			);

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

		// @throw AL::Exception
		virtual Void OnActivate()
		{
		}

		// @throw AL::Exception
		virtual Void OnDeactivate()
		{
		}

	private: // SceneManager access
		friend SceneManager;

		// @throw AL::Exception
		Void SetActive(Bool set = True)
		{
			if (!IsActive() && set)
			{
				OnActivate();

				isActive = True;
			}
			else if (IsActive() && !set)
			{
				OnDeactivate();

				isActive = False;
			}
		}
	};
}
