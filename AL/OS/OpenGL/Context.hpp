#pragma once
#include "AL/Common.hpp"

#include "OpenGLException.hpp"

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#if AL_HAS_INCLUDE(<GL/gl.h>)
		#define AL_DEPENDENCY_OPENGL

		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/OS/Windows/GDI/GDIException.hpp"

	#if AL_HAS_INCLUDE(<gl/GL.h>)
		#define AL_DEPENDENCY_OPENGL

		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glaux.h>

		#if defined(AL_COMPILER_MSVC)
			#pragma comment(lib, "Gdi32.lib")
			#pragma comment(lib, "glu32.lib")
			#pragma comment(lib, "opengl32.lib")
		#endif
	#endif

	#include <wingdi.h>
#endif

#if !defined(AL_DEPENDENCY_OPENGL)
	typedef float GLfloat;
#endif

namespace AL::OS::OpenGL
{
	class Context
	{
#if defined(AL_PLATFORM_LINUX)
		typedef void* HWINDOW;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::HWND HWINDOW;
#endif

		Bool isCreated = False;

#if defined(AL_DEPENDENCY_OPENGL)
	#if defined(AL_PLATFORM_LINUX)

	#elif defined(AL_PLATFORM_WINDOWS)
		::HDC   hDC;
		::HGLRC hGLRC;
	#endif
#endif

		HWINDOW          hWindow;
		Drawing::SizeU32 clientSize;

		Context(Context&&) = delete;
		Context(const Context&) = delete;

	public:
		Context()
		{
		}

		virtual ~Context()
		{
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		auto GetHandle() const
		{
#if defined(AL_DEPENDENCY_OPENGL)
	#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
	#elif defined(AL_PLATFORM_WINDOWS)
			return hGLRC;
	#endif
#endif
		}

		// @throw AL::Exception
		Void Create(HWINDOW hWindow)
		{
			AL_ASSERT(
				!IsCreated(),
				"Context already created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
	#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
	#elif defined(AL_PLATFORM_WINDOWS)
			hDC = ::GetDC(
				hWindow
			);

			{
				PIXELFORMATDESCRIPTOR pfd = { };
				pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
				pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
				pfd.nVersion   = 1;
				pfd.cColorBits = 16;
				pfd.cDepthBits = 16;
				pfd.iPixelType = PFD_TYPE_RGBA;
				pfd.iLayerType = PFD_MAIN_PLANE;

				int pixelFormat;

				if ((pixelFormat = ::ChoosePixelFormat(hDC, &pfd)) == 0)
				{
					::ReleaseDC(
						hWindow,
						hDC
					);

					throw Windows::GDI::GDIException(
						"ChoosePixelFormat"
					);
				}

				if (!::SetPixelFormat(hDC, pixelFormat, &pfd))
				{
					::ReleaseDC(
						hWindow,
						hDC
					);

					throw Windows::GDI::GDIException(
						"SetPixelFormat"
					);
				}
			}

			if ((hGLRC = ::wglCreateContext(hDC)) == NULL)
			{
				::ReleaseDC(
					hWindow,
					hDC
				);

				throw OpenGLException(
					"wglCreateContext"
				);
			}

			if (!::wglMakeCurrent(hDC, hGLRC))
			{
				::wglDeleteContext(
					GetHandle()
				);

				::ReleaseDC(
					hWindow,
					hDC
				);

				throw OpenGLException(
					"wglMakeCurrent"
				);
			}
	#endif
#else
			throw DependencyMissingException(
				"OpenGL"
			);
#endif

			this->hWindow = hWindow;

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
#if defined(AL_DEPENDENCY_OPENGL)
	#if defined(AL_PLATFORM_LINUX)
				// TODO: implement
	#elif defined(AL_PLATFORM_WINDOWS)
				::wglMakeCurrent(
					NULL,
					NULL
				);

				::wglDeleteContext(
					GetHandle()
				);

				::ReleaseDC(
					hWindow,
					hDC
				);
	#endif
#endif

				isCreated = False;
			}
		}

		// @throw AL::Exception
		Void SetClientSize(uint32 width, uint32 height)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
			if (width == 0)
			{

				width = 1;
			}

			if (height == 0)
			{

				height = 1;
			}

			::glViewport(
				0,
				0,
				static_cast<GLsizei>(width & Integer<uint32>::SignedCastMask),
				static_cast<GLsizei>(height & Integer<uint32>::SignedCastMask)
			);

			clientSize.Width  = width & Integer<uint32>::SignedCastMask;
			clientSize.Height = height & Integer<uint32>::SignedCastMask;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void Clear(Drawing::Color color)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
			::glClearColor(
				color.R / 255.0f,
				color.G / 255.0f,
				color.B / 255.0f,
				color.A / 255.0f
			);

			::glClear(
				GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
			);

			::glLoadIdentity();
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void Present()
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
	#if defined(AL_PLATFORM_LINUX)
			// TODO: vsync
			// TODO: implement
			throw NotImplementedException();
	#elif defined(AL_PLATFORM_WINDOWS)
			if (!::SwapBuffers(hDC))
			{

				throw Windows::GDI::GDIException(
					"SwapBuffers"
				);
			}
	#endif
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void FillCircle(const Drawing::Vector2F& position, Drawing::Color color, Float radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			FillEllipse(
				position,
				color,
				radius,
				radius
			);
		}

		// @throw AL::Exception
		Void FillEllipse(const Drawing::Vector2F& position, Drawing::Color color, Float radiusX, Float radiusY)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void FillRectangle(const Drawing::Vector2F& position, Drawing::Color color, Float width, Float height)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
			::GLfloat tl_X, tl_Y;
			TranslatePositionToNative(position, tl_X, tl_Y);

			::GLfloat tr_X, tr_Y;
			TranslatePositionToNative(Drawing::Vector2F(position.X + width, position.Y), tr_X, tr_Y);

			::GLfloat bl_X, bl_Y;
			TranslatePositionToNative(Drawing::Vector2F(position.X, position.Y + height), bl_X, bl_Y);

			::GLfloat br_X, br_Y;
			TranslatePositionToNative(Drawing::Vector2F(position.X + width, position.Y + height), br_X, br_Y);

			::glBegin(GL_QUADS);
			::glColor3f(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f);

			::glVertex2f(tl_X, tl_Y);
			::glVertex2f(tr_X, tr_Y);
			::glVertex2f(br_X, br_Y);
			::glVertex2f(bl_X, bl_Y);

			::glEnd();
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void FillRoundedRectangle(const Drawing::Vector2F& position, Drawing::Color color, Float width, Float height, Float radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void DrawLine(const Drawing::Vector2F& start, const Drawing::Vector2F& end, Drawing::Color color, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

#if defined(AL_DEPENDENCY_OPENGL)
			::GLfloat start_X, start_Y;
			TranslatePositionToNative(start, start_X, start_Y);

			::GLfloat end_X, end_Y;
			TranslatePositionToNative(end, end_X, end_Y);

			::glBegin(GL_LINES);
			::glLineWidth(strokeWidth); // TODO: support widths above 1.0f
			::glColor3f(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f);

			::glVertex2f(start_X, start_Y);
			::glVertex2f(end_X, end_Y);

			::glEnd();
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		Void DrawString(const Drawing::Vector2F& position, Drawing::Color color, const String& fontName, Float fontSize, const String& text)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void DrawCircle(const Drawing::Vector2F& position, Drawing::Color color, Float radius, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			DrawEllipse(
				position,
				color,
				radius,
				radius,
				strokeWidth
			);
		}

		// @throw AL::Exception
		Void DrawEllipse(const Drawing::Vector2F& position, Drawing::Color color, Float radiusX, Float radiusY, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// TODO: implement
			throw NotImplementedException();
		}

		// @throw AL::Exception
		Void DrawRectangle(const Drawing::Vector2F& position, Drawing::Color color, Float width, Float height, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// top
			DrawLine(position, Drawing::Vector2F(position.X + width, position.Y), color, strokeWidth);
			// right
			DrawLine(Drawing::Vector2F(position.X + width, position.Y), Drawing::Vector2F(position.X + width, position.Y + height), color, strokeWidth);
			// bottom
			DrawLine(Drawing::Vector2F(position.X, position.Y + height), Drawing::Vector2F(position.X + width, position.Y + height), color, strokeWidth);
			// left
			DrawLine(Drawing::Vector2F(position.X, position.Y + height), position, color, strokeWidth);
		}

		// @throw AL::Exception
		Void DrawRoundedRectangle(const Drawing::Vector2F& position, Drawing::Color color, Float width, Float height, Float radius, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Context not created"
			);

			// TODO: implement
			throw NotImplementedException();
		}

	private:
		Void TranslatePositionToNative(const Drawing::Vector2F& position, ::GLfloat& x, ::GLfloat& y) const
		{
			x = -1.0f + ((position.X / clientSize.Width) * 2);
			y = 1.0f - ((position.Y / clientSize.Height) * 2);
		}
	};
}
