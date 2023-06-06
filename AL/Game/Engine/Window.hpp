#pragma once
#include "AL/Common.hpp"

#include "Audio.hpp"
#include "Graphics.hpp"

#include "Menu.hpp"

#include "SceneManager.hpp"

#include "WindowComponent.hpp"

#include "GUI/WidgetManager.hpp"

#include "AL/OS/Window.hpp"

#include "AL/Collections/Stack.hpp"

namespace AL::Game::Engine
{
	class Window
		: public OS::Window
	{
		typedef OS::Window _Window;

		typedef Collections::Stack<WindowCursors> _CursorStack;

		Audio              audio;
		Graphics           graphics;

		_CursorStack       cursors;

		Menu               menu;
		SceneManager       sceneManager;
		GUI::WidgetManager widgetManager;

		Window(Window&&) = delete;
		Window(const Window&) = delete;

	public:
		Window(String&& name, String&& title)
			: _Window(
				Move(name),
				Move(title)
			),
			audio(
				*this
			),
			graphics(
				*this
			),
			menu(
				*this
			),
			sceneManager(
				*this
			),
			widgetManager(
				*this
			)
		{
			EnablePaint(False);
			EnableResize(False);
			EnableMaximize(False);
		}

		auto& GetMenu()
		{
			return menu;
		}
		auto& GetMenu() const
		{
			return menu;
		}

		auto& GetAudio()
		{
			return audio;
		}
		auto& GetAudio() const
		{
			return audio;
		}

		auto& GetGraphics()
		{
			return graphics;
		}
		auto& GetGraphics() const
		{
			return graphics;
		}

		auto& GetSceneManager()
		{
			return sceneManager;
		}
		auto& GetSceneManager() const
		{
			return sceneManager;
		}

		auto& GetWidgetManager()
		{
			return widgetManager;
		}
		auto& GetWidgetManager() const
		{
			return widgetManager;
		}

		// @throw AL::Exception
		Void Draw(Float delta)
		{
			AL_ASSERT(
				IsOpen(),
				"Window not open"
			);

			try
			{
				GetGraphics().BeginDraw();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error beginning Graphics draw"
				);
			}

			OnDraw(
				delta
			);

			OnEndDraw(
				delta
			);

			try
			{
				while (!GetGraphics().EndDraw())
				{
					UnloadContent();

					GetGraphics().DestroyTarget();

					try
					{
						GetGraphics().CreateTarget();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error recreating Graphics target"
						);
					}

					try
					{
						LoadContent();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error reloading content"
						);
					}

					try
					{
						GetGraphics().BeginDraw();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error beginning Graphics draw"
						);
					}
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error ending Graphics draw"
				);
			}
		}

		// @throw AL::Exception
		Bool PushCursor(WindowCursors value)
		{
			cursors.Push(
				value
			);

			if (!SetCursor(value))
			{
				cursors.Pop();

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Bool PopCursor()
		{
			WindowCursors cursor;

			if (!PopCursor(cursor))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool PopCursor(WindowCursors& value)
		{
			if (!cursors.Pop(value))
			{

				return False;
			}

			if (!SetCursor(cursors.GetSize() ? *(--cursors.end()) : WindowCursors::Arrow))
			{
				cursors.Push(
					value
				);

				return False;
			}

			return True;
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnOpen() override
		{
			_Window::OnOpen();

		}

		virtual Void OnClose() override
		{

			_Window::OnClose();
		}

		// @throw AL::Exception
		virtual Void OnCreate() override
		{
			_Window::OnCreate();

			try
			{
				GetAudio().Create();
			}
			catch (Exception& exception)
			{
				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating Audio"
				);
			}

			try
			{
				GetGraphics().Create();
			}
			catch (Exception& exception)
			{
				GetAudio().Destroy();

				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating Graphics"
				);
			}

			try
			{
				GetGraphics().CreateTarget();
			}
			catch (Exception& exception)
			{
				GetGraphics().Destroy();

				GetAudio().Destroy();

				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating Graphics target"
				);
			}

			try
			{
				GetSceneManager().Create();
			}
			catch (Exception& exception)
			{
				GetGraphics().DestroyTarget();
				GetGraphics().Destroy();

				GetAudio().Destroy();

				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating SceneManager"
				);
			}

			try
			{
				GetWidgetManager().Create();
			}
			catch (Exception& exception)
			{
				GetSceneManager().Destroy();

				GetGraphics().DestroyTarget();
				GetGraphics().Destroy();

				GetAudio().Destroy();

				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating GUI::WidgetManager"
				);
			}

			try
			{
				GetMenu().Create();
			}
			catch (Exception& exception)
			{
				GetWidgetManager().Destroy();
				GetSceneManager().Destroy();

				GetGraphics().DestroyTarget();
				GetGraphics().Destroy();

				GetAudio().Destroy();

				_Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating Menu"
				);
			}
		}

		virtual Void OnDestroy() override
		{
			GetMenu().Destroy();

			GetWidgetManager().Destroy();

			GetSceneManager().Destroy();

			GetGraphics().DestroyTarget();
			GetGraphics().Destroy();

			GetAudio().Destroy();

			_Window::OnDestroy();
		}

		// @throw AL::Exception
		virtual Void OnLoadContent() override
		{
			_Window::OnLoadContent();

			try
			{
				GetSceneManager().LoadContent();
			}
			catch (Exception& exception)
			{
				_Window::OnUnloadContent();

				throw Exception(
					Move(exception),
					"Error loading SceneManager content"
				);
			}

			try
			{
				GetWidgetManager().LoadContent();
			}
			catch (Exception& exception)
			{
				GetSceneManager().UnloadContent();

				_Window::OnUnloadContent();

				throw Exception(
					Move(exception),
					"Error loading GUI::WidgetManager content"
				);
			}

			try
			{
				GetMenu().LoadContent();
			}
			catch (Exception& exception)
			{
				GetWidgetManager().UnloadContent();
				GetSceneManager().UnloadContent();

				_Window::OnUnloadContent();

				throw Exception(
					Move(exception),
					"Error loading Menu content"
				);
			}
		}

		virtual Void OnUnloadContent() override
		{
			GetMenu().UnloadContent();

			GetWidgetManager().UnloadContent();

			GetSceneManager().UnloadContent();

			_Window::OnUnloadContent();
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta)
		{
			try
			{
				GetSceneManager().Draw(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error drawing SceneManager"
				);
			}
		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta)
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

			try
			{
				GetMenu().Draw(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error drawing Menu"
				);
			}
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta) override
		{
			_Window::OnUpdate(
				delta
			);

			try
			{
				GetSceneManager().Update(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating SceneManager"
				);
			}

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

			try
			{
				GetMenu().Update(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating Menu"
				);
			}
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
		{
			if (_Window::OnMouseEvent(event))
			{

				return True;
			}

			try
			{
				if (GetMenu().HandleMouseEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling Menu mouse event"
				);
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

			try
			{
				if (GetSceneManager().HandleMouseEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling SceneManager mouse event"
				);
			}

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
		{
			if (_Window::OnKeyboardEvent(event))
			{

				return True;
			}

			try
			{
				if (GetMenu().HandleKeyboardEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling Menu keyboard event"
				);
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

			try
			{
				if (GetSceneManager().HandleKeyboardEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling SceneManager keyboard event"
				);
			}

			return False;
		}

		// @throw AL::Exception
		virtual Void OnResolutionChanged(OS::WindowResolution::Type width, OS::WindowResolution::Type height) override
		{
			_Window::OnResolutionChanged(
				width,
				height
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			try
			{
				GetGraphics().GetOpenGL().SetClientSize(
					width,
					height
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error setting OpenGL::Context client size"
				);
			}
#endif
		}

	private:
		using _Window::SetCursor;
	};
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::Audio::Create()
{
	AL_ASSERT(
		!IsCreated(),
		"Audio already created"
	);

#if defined(AL_PLATFORM_LINUX)
	// TODO: implement
	throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
	try
	{
		dSound.Create(
			GetWindow().GetHandle()
		);
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error creating OS::Windows::DirectX::DirectSound"
		);
	}
#endif

	isCreated = True;
}

inline AL::Game::Engine::SizeU32 AL::Game::Engine::Graphics::GetTargetSize() const
{
	return GetWindow().GetResolution();
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::Graphics::Create()
{
	AL_ASSERT(
		!IsCreated(),
		"Graphics already created"
	);

#if defined(AL_GAME_ENGINE_OPENGL)
	try
	{
		GetOpenGL().Create(
			GetWindow().GetHandle()
		);
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error creating OpenGL::Context"
		);
	}

	try
	{
		GetOpenGL().SetClientSize(
			GetWindow().GetResolution().Width,
			GetWindow().GetResolution().Height
		);
	}
	catch (Exception& exception)
	{
		GetOpenGL().Destroy();

		throw Exception(
			Move(exception),
			"Error setting OpenGL::Context client size"
		);
	}
#elif defined(AL_GAME_ENGINE_DIRECTX)
	try
	{
		GetDirect3D().Create(
			GetWindow().GetHandle()
		);
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error creating OS::Windows::DirectX::Direct3D"
		);
	}

	try
	{
		GetDirect2D().Create();
	}
	catch (Exception& exception)
	{
		GetDirect3D().Destroy();

		throw Exception(
			Move(exception),
			"Error creating OS::Windows::DirectX::Direct2D"
		);
	}
#endif

	try
	{
		Resources_Create();
	}
	catch (Exception& exception)
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
		throw NotImplementedException();
#elif defined(AL_GAME_ENGINE_DIRECTX)
		GetDirect2D().Destroy();
		GetDirect3D().Destroy();
#endif

		throw Exception(
			Move(exception),
			"Error creating resources"
		);
	}

	isCreated = True;
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::Graphics::BeginDraw()
{
	AL_ASSERT(
		IsCreated(),
		"Graphics not created"
	);

	AL_ASSERT(
		IsTargetCreated(),
		"Graphics target not created"
	);

	AL_ASSERT(
		!isDrawing,
		"Graphics already drawing"
	);

	isDrawing = True;

#if defined(AL_GAME_ENGINE_OPENGL)
	try
	{
		GetOpenGL().Clear(
			GetWindow().GetBackgroundColor()
		);
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error clearing OpenGL::Context"
		);
	}
#elif defined(AL_GAME_ENGINE_DIRECTX)
	try
	{
		GetDirect2D().BeginDraw();
	}
	catch (Exception& exception)
	{
		isDrawing = False;

		throw Exception(
			Move(exception),
			"Error beginning OS::Windows::DirectX::Direct2D draw"
		);
	}

	try
	{
		GetDirect3D().Clear(
			GetWindow().GetBackgroundColor()
		);
	}
	catch (Exception& exception)
	{
		isDrawing = False;

		throw Exception(
			Move(exception),
			"Error clearing OS::Windows::DirectX::Direct3D"
		);
	}
#endif
}

inline AL::Game::Engine::WindowComponent::WindowComponent(Window& window)
	: lpAudio(
		&window.GetAudio()
	),
	lpGraphics(
		&window.GetGraphics()
	),
	lpWindow(
		&window
	)
{
}

inline AL::Game::Engine::WindowComponent::~WindowComponent()
{
	if (IsCreated())
	{
		if (IsContentLoaded())
		{

			UnloadContent();
		}

		Destroy();
	}
}

// @throw AL::Exception
inline AL::Bool AL::Game::Engine::WindowComponent::PushCursor(WindowCursors value)
{
	if (!GetWindow().PushCursor(value))
	{

		return False;
	}

	return True;
}

// @throw AL::Exception
inline AL::Bool AL::Game::Engine::WindowComponent::PopCursor(WindowCursors& value)
{
	if (!GetWindow().PopCursor(value))
	{

		return False;
	}

	return True;
}
