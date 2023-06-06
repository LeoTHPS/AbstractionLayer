#pragma once
#include "AL/Common.hpp"

#include "Audio.hpp"
#include "Graphics.hpp"

#include "AL/OS/Window.hpp"

#include "AL/Hardware/Mouse.hpp"
#include "AL/Hardware/Keyboard.hpp"

namespace AL::Game::Engine
{
	class Window;

	typedef OS::WindowIcon    WindowIcon;
	typedef OS::WindowIcons   WindowIcons;

	typedef OS::WindowColor   WindowColor;
	typedef OS::WindowColors  WindowColors;

	typedef OS::WindowCursor  WindowCursor;
	typedef OS::WindowCursors WindowCursors;

	class WindowComponent
	{
		Bool      isCreated       = False;
		Bool      isDrawing       = False;
		Bool      isUpdating      = False;
		Bool      isContentLoaded = False;

		Audio*    lpAudio;
		Graphics* lpGraphics;
		Window*   lpWindow;

		WindowComponent(WindowComponent&&) = delete;
		WindowComponent(const WindowComponent&) = delete;

	public:
		explicit WindowComponent(Window& window);

		virtual ~WindowComponent();

		Bool IsCreated() const
		{
			return isCreated;
		}

		Bool IsDrawing() const
		{
			return isDrawing;
		}

		Bool IsUpdating() const
		{
			return isUpdating;
		}

		Bool IsContentLoaded() const
		{
			return isContentLoaded;
		}

		auto& GetAudio()
		{
			return *lpAudio;
		}
		auto& GetAudio() const
		{
			return *lpAudio;
		}

		auto& GetGraphics()
		{
			return *lpGraphics;
		}
		auto& GetGraphics() const
		{
			return *lpGraphics;
		}

		auto& GetWindow()
		{
			return *lpWindow;
		}
		auto& GetWindow() const
		{
			return *lpWindow;
		}

		template<typename T_WINDOW>
		T_WINDOW& GetWindow()
		{
			static_assert(
				Is_Base_Of<Window, T_WINDOW>::Value,
				"T_WINDOW must inherit Window"
			);

			return static_cast<T_WINDOW&>(
				GetWindow()
			);
		}
		template<typename T_WINDOW>
		const T_WINDOW& GetWindow() const
		{
			static_assert(
				Is_Base_Of<Window, T_WINDOW>::Value,
				"T_WINDOW must inherit Window"
			);

			return static_cast<const T_WINDOW&>(
				GetWindow()
			);
		}

		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"WindowComponent already created"
			);

			OnCreate();

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				if (IsContentLoaded())
				{

					UnloadContent();
				}

				OnDestroy();

				isCreated = False;
			}
		}

		// @throw AL::Exception
		Void LoadContent()
		{
			AL_ASSERT(
				IsCreated(),
				"WindowComponent not created"
			);

			AL_ASSERT(
				!IsContentLoaded(),
				"WindowComponent content already loaded"
			);

			OnLoadContent();

			isContentLoaded = True;
		}

		Void UnloadContent()
		{
			if (IsContentLoaded())
			{
				OnUnloadContent();

				isContentLoaded = False;
			}
		}

		// @throw AL::Exception
		Void Draw(Float delta)
		{
			AL_ASSERT(
				IsCreated(),
				"WindowComponent not created"
			);

			AL_ASSERT(
				IsContentLoaded(),
				"WindowComponent content not loaded"
			);

			isDrawing = True;

			try
			{
				OnDraw(
					delta
				);
			}
			catch (Exception&)
			{
				isDrawing = False;

				throw;
			}

			try
			{
				OnEndDraw(
					delta
				);
			}
			catch (Exception&)
			{
				isDrawing = False;

				throw;
			}

			isDrawing = False;
		}

		// @throw AL::Exception
		Void Update(TimeSpan delta)
		{
			AL_ASSERT(
				IsCreated(),
				"WindowComponent not created"
			);

			AL_ASSERT(
				IsContentLoaded(),
				"WindowComponent content not loaded"
			);

			isUpdating = True;

			try
			{
				OnUpdate(
					delta
				);
			}
			catch (Exception&)
			{
				isUpdating = False;

				throw;
			}

			isUpdating = False;
		}

		// @throw AL::Exception
		Bool PushCursor(WindowCursors value);

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
		Bool PopCursor(WindowCursors& value);

		// @throw AL::Exception
		// @return AL::True if handled
		Bool HandleMouseEvent(const Hardware::MouseEvent& event)
		{
			AL_ASSERT(
				IsCreated(),
				"WindowComponent not created"
			);

			AL_ASSERT(
				IsContentLoaded(),
				"WindowComponent content not loaded"
			);

			if (OnMouseEvent(event))
			{

				return True;
			}

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		Bool HandleKeyboardEvent(const Hardware::KeyboardEvent& event)
		{
			AL_ASSERT(
				IsCreated(),
				"WindowComponent not created"
			);

			AL_ASSERT(
				IsContentLoaded(),
				"WindowComponent content not loaded"
			);

			if (OnKeyboardEvent(event))
			{

				return True;
			}

			return False;
		}

		Bool operator == (const WindowComponent& component) const
		{
			if (this != &component)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const WindowComponent& component) const
		{
			if (operator==(component))
			{

				return False;
			}

			return True;
		}

	protected: // Events
		// @throw AL::Exception
		virtual Void OnCreate()
		{
		}

		virtual Void OnDestroy()
		{
		}

		// @throw AL::Exception
		virtual Void OnLoadContent()
		{
		}

		virtual Void OnUnloadContent()
		{
		}

		// @throw AL::Exception
		virtual Void OnDraw(Float delta)
		{
		}

		// @throw AL::Exception
		virtual Void OnEndDraw(Float delta)
		{
		}

		// @throw AL::Exception
		virtual Void OnUpdate(TimeSpan delta)
		{
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnMouseEvent(const Hardware::MouseEvent& event)
		{

			return False;
		}

		// @throw AL::Exception
		// @return AL::True if handled
		virtual Bool OnKeyboardEvent(const Hardware::KeyboardEvent& event)
		{

			return False;
		}
	};
}
