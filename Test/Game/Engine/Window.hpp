#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Thread.hpp>
#include <AL/OS/System.hpp>
#include <AL/OS/Process.hpp>

// #define AL_GAME_ENGINE_OPENGL

#include <AL/Game/Loop.hpp>

#include <AL/Game/Engine/Window.hpp>

#include <AL/Game/Engine/GUI/Label.hpp>
#include <AL/Game/Engine/GUI/Button.hpp>
#include <AL/Game/Engine/GUI/WidgetManager.hpp>

// @throw AL::Exception
static void AL_Game_Engine_Window()
{
	using namespace AL;
	using namespace AL::Game;

	class MainWindow
		: public Engine::Window
	{
		Engine::GUI::WidgetManager widgets;

		struct
		{
			Engine::GUI::Label Thread;
			Engine::GUI::Label Process;
			Engine::GUI::Label Platform;

			// @throw AL::Exception
			Void Update()
			{
				Thread.SetText(  String::Format("Thread: %s",   ToString(OS::GetCurrentThreadId()).GetCString()));
				Process.SetText( String::Format("Process: %s",  ToString(OS::GetCurrentProcessId()).GetCString()));
				Platform.SetText(String::Format("Platform: %s", ToString(Platforms::Machine).GetCString()));
			}
		} system;

		struct
		{
			Engine::GUI::Label FPS;
			Engine::GUI::Label TickRate;
			Engine::GUI::Label Position;
			Engine::GUI::Label Resolution;

			// @throw AL::Exception
			Void Update(uint16 fps, uint16 tickRate, const OS::WindowPosition& position, const OS::WindowResolution& resolution)
			{
				FPS.SetText(       String::Format("FPS: %u",             fps));
				TickRate.SetText(  String::Format("TickRate: %u",        tickRate));
				Position.SetText(  String::Format("Position: %li, %li",  position.X,       position.Y));
				Resolution.SetText(String::Format("Resolution: %lux%lu", resolution.Width, resolution.Height));
			}
		} window;

		uint16    drawCount = 0;
		uint16    updateCount = 0;
		OS::Timer updateTimer;

	public:
		MainWindow(String&& name, String&& title)
			: Window(
				Move(name),
				Move(title)
			),
			widgets(
				*this
			),
			system{
				.Thread     = Engine::GUI::Label(widgets, "System::Thread",     GetWidgetSize(), GetWidgetPosition(4)),
				.Process    = Engine::GUI::Label(widgets, "System::Process",    GetWidgetSize(), GetWidgetPosition(5)),
				.Platform   = Engine::GUI::Label(widgets, "System::Platform",   GetWidgetSize(), GetWidgetPosition(6))
			},
			window{
				.FPS        = Engine::GUI::Label(widgets, "Window::FPS",        GetWidgetSize(), GetWidgetPosition(0)),
				.TickRate   = Engine::GUI::Label(widgets, "Window::TickRate",   GetWidgetSize(), GetWidgetPosition(1)),
				.Position   = Engine::GUI::Label(widgets, "Window::Position",   GetWidgetSize(), GetWidgetPosition(2)),
				.Resolution = Engine::GUI::Label(widgets, "Window::Resolution", GetWidgetSize(), GetWidgetPosition(3))
			}
		{
			system.Update();

			window.Update(
				0,
				0,
				OS::WindowPosition(),
				OS::WindowResolution()
			);
		}

		virtual ~MainWindow()
		{
		}

	protected:
		// @throw AL::Exception
		virtual Void OnOpen() override
		{
			Window::OnOpen();

		}

		virtual Void OnClose() override
		{

			Window::OnClose();
		}

		// @throw AL::Exception
		virtual Void OnCreate() override
		{
			Window::OnCreate();

			try
			{
				widgets.Create();
			}
			catch (Exception& exception)
			{
				Window::OnDestroy();

				throw Exception(
					Move(exception),
					"Error creating widgets"
				);
			}
		}

		virtual Void OnDestroy() override
		{
			widgets.Destroy();

			Window::OnDestroy();
		}

		// @throw AL::Exception
		virtual Void OnLoadContent() override
		{
			Window::OnLoadContent();

			try
			{
				widgets.LoadContent();
			}
			catch (Exception& exception)
			{
				Window::OnUnloadContent();

				throw Exception(
					Move(exception),
					"Error loading widgets content"
				);
			}
		}

		virtual Void OnUnloadContent() override
		{
			widgets.UnloadContent();

			Window::OnUnloadContent();
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta) override
		{
			Window::OnDraw(
				delta
			);

			if (drawCount++ == 0xFFFF)
			{

				drawCount = 0xFFFF;
			}

			GetGraphics().DrawCircle<uint32>(
				Engine::Vector2U32(10, 10),
				Engine::SolidColorBrush(GetGraphics(), Engine::Colors::Black),
				50.0f
			);

			GetGraphics().DrawEllipse<uint32>(
				Engine::Vector2U32(200, 10),
				Engine::SolidColorBrush(GetGraphics(), Engine::Colors::Black),
				100.0f,
				50.0f
			);

			GetGraphics().DrawRectangle<uint32>(
				Engine::Vector2U32(10, 200),
				Engine::SolidColorBrush(GetGraphics(), Engine::Colors::Black),
				100,
				100
			);

			GetGraphics().DrawRoundedRectangle<uint32>(
				Engine::Vector2U32(200, 200),
				Engine::SolidColorBrush(GetGraphics(), Engine::Colors::Black),
				100,
				100,
				10.0f
			);
		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta) override
		{
			try
			{
				widgets.Draw(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error drawing widgets"
				);
			}

			Window::OnEndDraw(
				delta
			);
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta) override
		{
			Window::OnUpdate(
				delta
			);

			if (updateCount++ == 0xFFFF)
			{

				updateCount = 0xFFFF;
			}

			if (updateTimer.GetElapsed() >= TimeSpan::FromSeconds(1))
			{
				updateTimer.Reset();

				window.Update(
					drawCount,
					updateCount,
					GetPosition(),
					GetResolution()
				);

				system.Update();

				drawCount = 0;
				updateCount = 0;
			}

			try
			{
				widgets.Update(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating widgets"
				);
			}
		}

		// @throw AL::Exception
		// @return True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event) override
		{
			if (Window::OnMouseEvent(event))
			{

				return True;
			}

			try
			{
				if (widgets.HandleMouseEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling widgets mouse event"
				);
			}

			return False;
		}

		// @throw AL::Exception
		// @return True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event) override
		{
			if (Window::OnKeyboardEvent(event))
			{

				return True;
			}

			try
			{
				if (widgets.HandleKeyboardEvent(event))
				{

					return True;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error handling widgets keyboard event"
				);
			}

			return False;
		}

	private:
		Engine::GUI::WidgetSize GetWidgetSize() const
		{
			return Engine::GUI::WidgetSize(
				GetResolution().Width,
				15
			);
		}

		Engine::GUI::WidgetPosition GetWidgetPosition(AL::size_t index) const
		{
			auto size = GetWidgetSize();

			return Engine::GUI::WidgetPosition(
				0,
				index * size.Height
			);
		}
	};

	MainWindow mainWindow(
		"MainWindow",
		"AL::Game::Engine"
	);

	mainWindow.Open();
	mainWindow.GetGraphics().EnableVSync(False);

	Loop::Run(
		20,
		[&mainWindow](TimeSpan _delta)
		{
			if (!mainWindow.Update(_delta))
			{

				return False;
			}

			return True;
		},
		[&mainWindow](Float _delta)
		{
			mainWindow.Draw(
				_delta
			);

			return True;
		}
	);
}
