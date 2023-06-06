#pragma once
#include "AL/Common.hpp"

#include "Scene.hpp"

#include "WindowComponent.hpp"

#include "AL/Collections/Stack.hpp"

namespace AL::Game::Engine
{
	class SceneManager
		: public WindowComponent
	{
		size_t                     sceneLockCount = 0;

		Scene*                     lpActiveScene = nullptr;
		Collections::Stack<Scene*> pendingScenes;

		SceneManager(SceneManager&&) = delete;
		SceneManager(const SceneManager&) = delete;

	public:
		explicit SceneManager(Window& window)
			: WindowComponent(
				window
			)
		{
		}

		virtual ~SceneManager()
		{
		}

		auto GetActive() const
		{
			return lpActiveScene;
		}

		// @throw AL::Exception
		Void SetActive(Scene* lpScene)
		{
			pendingScenes.Push(
				lpScene
			);

			if (sceneLockCount == 0)
			{

				UpdateActiveScene();
			}
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnCreate() override
		{
			WindowComponent::OnCreate();

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					lpScene->Create();
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					WindowComponent::OnDestroy();

					throw Exception(
						Move(exception),
						"Error creating Scene"
					);
				}

				--sceneLockCount;
			}
		}

		virtual Void OnDestroy() override
		{
			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				lpScene->Destroy();

				--sceneLockCount;
			}

			WindowComponent::OnDestroy();
		}

		// @throw AL::Exception
		virtual Void OnLoadContent() override
		{
			WindowComponent::OnLoadContent();

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					lpScene->LoadContent();
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					WindowComponent::OnUnloadContent();

					throw Exception(
						Move(exception),
						"Error loading Scene content"
					);
				}
			}
		}

		virtual Void OnUnloadContent() override
		{
			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				lpScene->UnloadContent();

				--sceneLockCount;
			}

			WindowComponent::OnUnloadContent();
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			WindowComponent::OnDraw(
				delta
			);

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					lpScene->Draw(
						delta
					);
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					throw Exception(
						Move(exception),
						"Error drawing Scene"
					);
				}

				--sceneLockCount;
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

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					lpScene->Update(
						delta
					);
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					throw Exception(
						Move(exception),
						"Error updating Scene"
					);
				}

				--sceneLockCount;
			}

			try
			{
				UpdateActiveScene();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating active Scene"
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

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					if (lpScene->HandleMouseEvent(event))
					{
						--sceneLockCount;

						return True;
					}
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					throw Exception(
						Move(exception),
						"Error handling Scene mouse event"
					);
				}

				--sceneLockCount;
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

			if (auto lpScene = GetActive())
			{
				++sceneLockCount;

				try
				{
					if (lpScene->HandleKeyboardEvent(event))
					{
						--sceneLockCount;

						return True;
					}
				}
				catch (Exception& exception)
				{
					--sceneLockCount;

					throw Exception(
						Move(exception),
						"Error handling Scene keyboard event"
					);
				}

				--sceneLockCount;
			}

			return False;
		}

		// @throw AL::Exception
		virtual Void OnSceneChanged(Scene* lpActiveScene, Scene* lpPreviousScene)
		{
		}

	private:
		// @throw AL::Exception
		Void UpdateActiveScene()
		{
			Scene* lpNextScene;

			while (pendingScenes.Pop(lpNextScene))
			{
				Scene* lpPreviousScene;

				if ((lpPreviousScene = GetActive()) != nullptr)
				{
					try
					{
						lpPreviousScene->SetActive(
							False
						);
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error deactivating current Scene"
						);
					}

					if (IsCreated())
					{
						if (IsContentLoaded())
						{

							lpPreviousScene->UnloadContent();
						}

						lpPreviousScene->Destroy();
					}

					this->lpActiveScene = nullptr;
				}

				if ((this->lpActiveScene = lpNextScene) != nullptr)
				{
					if (IsCreated())
					{
						try
						{
							lpNextScene->Create();
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error creating Scene"
							);
						}

						if (IsContentLoaded())
						{
							try
							{
								lpNextScene->LoadContent();
							}
							catch (Exception& exception)
							{
								lpNextScene->Destroy();

								throw Exception(
									Move(exception),
									"Error loading Scene content"
								);
							}
						}
					}

					try
					{
						lpNextScene->SetActive();
					}
					catch (Exception& exception)
					{
						if (IsCreated())
						{
							if (IsContentLoaded())
							{

								lpNextScene->UnloadContent();
							}

							lpNextScene->Destroy();
						}

						throw Exception(
							Move(exception),
							"Error activating Scene"
						);
					}
				}

				OnSceneChanged(
					lpNextScene,
					lpPreviousScene
				);
			}
		}
	};
}

inline AL::Game::Engine::Scene::Scene(SceneManager& manager, String&& name)
	: WindowComponent(
		manager.GetWindow()
	),
	id(
		name
	),
	name(
		Move(name)
	),
	lpManager(
		&manager
	),
	widgetManager(
		GetWindow()
	)
{
}
