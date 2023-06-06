#pragma once
#include "AL/Common.hpp"

#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"

#if !defined(AL_GAME_ENGINE_OPENGL) && !defined(AL_GAME_ENGINE_DIRECTX)
	#if defined(AL_PLATFORM_LINUX)
		#define AL_GAME_ENGINE_OPENGL
	#elif defined(AL_PLATFORM_WINDOWS)
		#define AL_GAME_ENGINE_DIRECTX
	#endif
#endif

#if defined(AL_GAME_ENGINE_OPENGL)
	#include "AL/OS/OpenGL/Context.hpp"
#elif defined(AL_GAME_ENGINE_DIRECTX)
	#include "AL/OS/Windows/DirectX/Direct2D.hpp"
	#include "AL/OS/Windows/DirectX/Direct3D.hpp"
#endif

namespace AL::Game::Engine
{
	class Window;
	class Graphics;

	typedef Drawing::Color        Color;
	typedef Drawing::Colors       Colors;

	template<typename T>
	using Size = Drawing::Size<T>;

	template<typename T>
	using Vector1 = Drawing::Vector1<T>;
	template<typename T>
	using Vector2 = Drawing::Vector2<T>;
	template<typename T>
	using Vector3 = Drawing::Vector3<T>;
	template<typename T>
	using Vector4 = Drawing::Vector4<T>;

	template<typename T>
	using Rectangle = Drawing::Rectangle<T>;

	typedef Drawing::SizeF        SizeF;
	typedef Drawing::SizeD        SizeD;
	typedef Drawing::SizeI        SizeI;
	typedef Drawing::SizeU        SizeU;
	typedef Drawing::SizeI8       SizeI8;
	typedef Drawing::SizeU8       SizeU8;
	typedef Drawing::SizeI16      SizeI16;
	typedef Drawing::SizeU16      SizeU16;
	typedef Drawing::SizeI32      SizeI32;
	typedef Drawing::SizeU32      SizeU32;
	typedef Drawing::SizeI64      SizeI64;
	typedef Drawing::SizeU64      SizeU64;

	typedef Drawing::Vector1F     Vector1F;
	typedef Drawing::Vector1D     Vector1D;
	typedef Drawing::Vector1I     Vector1I;
	typedef Drawing::Vector1U     Vector1U;
	typedef Drawing::Vector1I8    Vector1I8;
	typedef Drawing::Vector1U8    Vector1U8;
	typedef Drawing::Vector1I16   Vector1I16;
	typedef Drawing::Vector1U16   Vector1U16;
	typedef Drawing::Vector1I32   Vector1I32;
	typedef Drawing::Vector1U32   Vector1U32;
	typedef Drawing::Vector1I64   Vector1I64;
	typedef Drawing::Vector1U64   Vector1U64;

	typedef Drawing::Vector2F     Vector2F;
	typedef Drawing::Vector2D     Vector2D;
	typedef Drawing::Vector2I     Vector2I;
	typedef Drawing::Vector2U     Vector2U;
	typedef Drawing::Vector2I8    Vector2I8;
	typedef Drawing::Vector2U8    Vector2U8;
	typedef Drawing::Vector2I16   Vector2I16;
	typedef Drawing::Vector2U16   Vector2U16;
	typedef Drawing::Vector2I32   Vector2I32;
	typedef Drawing::Vector2U32   Vector2U32;
	typedef Drawing::Vector2I64   Vector2I64;
	typedef Drawing::Vector2U64   Vector2U64;

	typedef Drawing::Vector3F     Vector3F;
	typedef Drawing::Vector3D     Vector3D;
	typedef Drawing::Vector3I     Vector3I;
	typedef Drawing::Vector3U     Vector3U;
	typedef Drawing::Vector3I8    Vector3I8;
	typedef Drawing::Vector3U8    Vector3U8;
	typedef Drawing::Vector3I16   Vector3I16;
	typedef Drawing::Vector3U16   Vector3U16;
	typedef Drawing::Vector3I32   Vector3I32;
	typedef Drawing::Vector3U32   Vector3U32;
	typedef Drawing::Vector3I64   Vector3I64;
	typedef Drawing::Vector3U64   Vector3U64;

	typedef Drawing::Vector4F     Vector4F;
	typedef Drawing::Vector4D     Vector4D;
	typedef Drawing::Vector4I     Vector4I;
	typedef Drawing::Vector4U     Vector4U;
	typedef Drawing::Vector4I8    Vector4I8;
	typedef Drawing::Vector4U8    Vector4U8;
	typedef Drawing::Vector4I16   Vector4I16;
	typedef Drawing::Vector4U16   Vector4U16;
	typedef Drawing::Vector4I32   Vector4I32;
	typedef Drawing::Vector4U32   Vector4U32;
	typedef Drawing::Vector4I64   Vector4I64;
	typedef Drawing::Vector4U64   Vector4U64;

	typedef Drawing::RectangleF   RectangleF;
	typedef Drawing::RectangleD   RectangleD;
	typedef Drawing::RectangleI   RectangleI;
	typedef Drawing::RectangleU   RectangleU;
	typedef Drawing::RectangleI8  RectangleI8;
	typedef Drawing::RectangleU8  RectangleU8;
	typedef Drawing::RectangleI16 RectangleI16;
	typedef Drawing::RectangleU16 RectangleU16;
	typedef Drawing::RectangleI32 RectangleI32;
	typedef Drawing::RectangleU32 RectangleU32;
	typedef Drawing::RectangleI64 RectangleI64;
	typedef Drawing::RectangleU64 RectangleU64;

	struct Font
	{
		String Name;
		Float  Size;
	};

	class GraphicsResource
	{
		Bool isCreated       = False;
		Bool isContentLoaded = False;

		Graphics* const lpGraphics;

		GraphicsResource(GraphicsResource&&) = delete;
		GraphicsResource(const GraphicsResource&) = delete;

	public:
		explicit GraphicsResource(Graphics& graphics)
			: lpGraphics(
				&graphics
			)
		{
		}

		virtual ~GraphicsResource()
		{
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		Bool IsContentLoaded() const
		{
			return isContentLoaded;
		}

		auto& GetGraphics()
		{
			return *lpGraphics;
		}
		auto& GetGraphics() const
		{
			return *lpGraphics;
		}

	protected:
		// @throw AL::Exception
		Void ReloadContent()
		{
			if (IsContentLoaded())
			{
				UnloadContent();
				LoadContent();
			}
		}

	protected:
		// @throw AL::Exception
		virtual Void OnCreate() = 0;

		virtual Void OnDestroy() = 0;

		// @throw AL::Exception
		virtual Void OnLoadContent() = 0;

		virtual Void OnUnloadContent() = 0;

	private: // Graphics access
		friend Graphics;

		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"GraphicsResource already created"
			);

			OnCreate();

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				OnDestroy();

				isCreated = False;
			}
		}

		// @throw AL::Exception
		Void LoadContent()
		{
			AL_ASSERT(
				!IsContentLoaded(),
				"GraphicsResource content already loaded"
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
	};

	class Image
		: public GraphicsResource
	{
		const Bool                isFileSource;

		String                    path;
		Collections::Array<uint8> buffer;

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
		OS::Windows::DirectX::Bitmap resource;
#endif

	public:
		// @throw AL::Exception
		Image(Graphics& graphics, String&& path);
		// @throw AL::Exception
		Image(Graphics& graphics, const Void* lpBuffer, size_t size);

		virtual ~Image();

		Bool IsFileSource() const
		{
			return isFileSource;
		}

		auto& GetPath() const
		{
			return path;
		}

		auto& GetBuffer() const
		{
			return buffer;
		}

		auto& GetResource() const
		{
#if defined(AL_GAME_ENGINE_OPENGL)
			return *this;
#elif defined(AL_GAME_ENGINE_DIRECTX)
			return resource;
#else
			return *this;
#endif
		}

	protected:
		// @throw AL::Exception
		virtual Void OnCreate() override;

		virtual Void OnDestroy() override;

		// @throw AL::Exception
		virtual Void OnLoadContent() override;

		virtual Void OnUnloadContent() override;
	};

	enum class TextAlignment
	{
		Top      = 0x01,
		Left     = 0x02,
		Right    = 0x04,
		Bottom   = 0x08,
		Center_H = 0x10,
		Center_V = 0x20
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(TextAlignment);

	class TextFormat
		: public GraphicsResource
	{
		Font                   font;
		BitMask<TextAlignment> alignment;

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
		OS::Windows::DirectX::TextFormat resource;
#endif

	public:
		// @throw AL::Exception
		TextFormat(Graphics& graphics, Font&& font, TextAlignment alignment = TextAlignment::Left);

		virtual ~TextFormat();

		auto& GetFont() const
		{
			return font;
		}

		auto& GetAlignment() const
		{
			return alignment;
		}

		auto& GetResource() const
		{
#if defined(AL_GAME_ENGINE_OPENGL)
			return *this;
#elif defined(AL_GAME_ENGINE_DIRECTX)
			return resource;
#else
			return *this;
#endif
		}

		// @throw AL::Exception
		Void SetFont(Font&& value);
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
		Void SetAlignment(TextAlignment value);

	protected:
		// @throw AL::Exception
		virtual Void OnCreate() override;

		virtual Void OnDestroy() override;

		// @throw AL::Exception
		virtual Void OnLoadContent() override;

		virtual Void OnUnloadContent() override;
	};

	class TextLayout
		: public GraphicsResource
	{
		String      text;
		SizeF       size;
		TextFormat* lpFormat;

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
		OS::Windows::DirectX::TextLayout resource;
#endif

	public:
		// @throw AL::Exception
		TextLayout(Graphics& graphics, TextFormat& format, String&& text, SizeF&& size);

		virtual ~TextLayout();

		auto& GetText() const
		{
			return text;
		}

		auto& GetSize() const
		{
			return size;
		}

		auto& GetFormat()
		{
			return *lpFormat;
		}
		auto& GetFormat() const
		{
			return *lpFormat;
		}

		auto& GetResource() const
		{
#if defined(AL_GAME_ENGINE_OPENGL)
			return *this;
#elif defined(AL_GAME_ENGINE_DIRECTX)
			return resource;
#else
			return *this;
#endif
		}

		// @throw AL::Exception
		Void SetSize(SizeF&& value)
		{
			size = Move(
				value
			);

			ReloadContent();
		}
		// @throw AL::Exception
		Void SetSize(const SizeF& value)
		{
			SizeF size(value);

			SetSize(
				Move(size)
			);
		}

		// @throw AL::Exception
		Void SetText(String&& value);
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
		Void SetFormat(TextFormat& value);

	protected:
		// @throw AL::Exception
		virtual Void OnCreate() override;

		virtual Void OnDestroy() override;

		// @throw AL::Exception
		virtual Void OnLoadContent() override;

		virtual Void OnUnloadContent() override;
	};

	class SolidColorBrush
		: public GraphicsResource
	{
		Color color;

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
		OS::Windows::DirectX::SolidColorBrush resource;
#endif

	public:
		// @throw AL::Exception
		SolidColorBrush(Graphics& graphics, Color color);

		virtual ~SolidColorBrush();

		auto GetColor() const
		{
			return color;
		}

		auto& GetResource() const
		{
#if defined(AL_GAME_ENGINE_OPENGL)
			return *this;
#elif defined(AL_GAME_ENGINE_DIRECTX)
			return resource;
#else
			return *this;
#endif
		}

		// @throw AL::Exception
		Void SetColor(Color value);

	protected:
		// @throw AL::Exception
		virtual Void OnCreate() override;

		virtual Void OnDestroy() override;

		// @throw AL::Exception
		virtual Void OnLoadContent() override;

		virtual Void OnUnloadContent() override;
	};

	class Graphics
	{
		Bool          isCreated       = False;
		Bool          isTargetCreated = False;
		Bool          isDrawing       = False;
		Bool          isVSyncEnabled  = True;

		Window* const lpWindow;

#if defined(AL_GAME_ENGINE_OPENGL)
		OS::OpenGL::Context            openGL;
#elif defined(AL_GAME_ENGINE_DIRECTX)
		OS::Windows::DirectX::Direct2D direct2D;
		OS::Windows::DirectX::Direct3D direct3D;
#endif

		Collections::LinkedList<GraphicsResource*> resources;

		Graphics(Graphics&&) = delete;
		Graphics(const Graphics&) = delete;

	public:
		explicit Graphics(Window& window)
			: lpWindow(
				&window
			)
		{
		}

		virtual ~Graphics()
		{
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		Bool IsTargetCreated() const
		{
			return isTargetCreated;
		}

		Bool IsVSyncEnabled() const
		{
			return isVSyncEnabled;
		}

		auto& GetWindow()
		{
			return *lpWindow;
		}
		auto& GetWindow() const
		{
			return *lpWindow;
		}

#if defined(AL_GAME_ENGINE_OPENGL)
		auto& GetOpenGL()
		{
			return openGL;
		}
		auto& GetOpenGL() const
		{
			return openGL;
		}
#elif defined(AL_GAME_ENGINE_DIRECTX)
		auto& GetDirect2D()
		{
			return direct2D;
		}
		auto& GetDirect2D() const
		{
			return direct2D;
		}

		auto& GetDirect3D()
		{
			return direct3D;
		}
		auto& GetDirect3D() const
		{
			return direct3D;
		}
#endif

		SizeU32 GetTargetSize() const;

		Void EnableVSync(Bool set = True)
		{
			isVSyncEnabled = set;
		}

		// @throw AL::Exception
		Void Create();

		Void Destroy()
		{
			if (IsCreated())
			{
				if (IsTargetCreated())
				{

					DestroyTarget();
				}

				Resources_Destroy();

#if defined(AL_GAME_ENGINE_OPENGL)
				// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
				GetDirect2D().Destroy();
				GetDirect3D().Destroy();
#endif

				isCreated = False;
			}
		}

		// @throw AL::Exception
		Void CreateTarget()
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				!IsTargetCreated(),
				"Graphics target already created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
			try
			{
				GetDirect3D().CreateTarget();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error creating OS::Windows::DirectX::Direct3D target"
				);
			}

			try
			{
				GetDirect2D().CreateTarget(
					GetDirect3D().GetSwapChain()
				);
			}
			catch (Exception& exception)
			{
				GetDirect3D().DestroyTarget();

				throw Exception(
					Move(exception),
					"Error creating OS::Windows::DirectX::Direct2D target"
				);
			}
#endif

			try
			{
				Resources_LoadContent();
			}
			catch (Exception& exception)
			{
#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
				GetDirect2D().DestroyTarget();
				GetDirect3D().DestroyTarget();
#endif

				throw Exception(
					Move(exception),
					"Error loading resources content"
				);
			}

			isTargetCreated = True;
		}

		Void DestroyTarget()
		{
			if (IsTargetCreated())
			{
				Resources_UnloadContent();

#if defined(AL_GAME_ENGINE_OPENGL)

#elif defined(AL_GAME_ENGINE_DIRECTX)
				GetDirect2D().DestroyTarget();
				GetDirect3D().DestroyTarget();
#endif

				isTargetCreated = False;
			}
		}

		// @throw AL::Exception
		Void BeginDraw();

		// @throw AL::Exception
		// @return AL::False if target needs recreated
		Bool EndDraw()
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
				isDrawing,
				"Graphics not drawing"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			// TODO: vsync

			try
			{
				GetOpenGL().Present();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error presening OS::OpenGL::Context"
				);
			}
#elif defined(AL_GAME_ENGINE_DIRECTX)
			try
			{
				if (!GetDirect2D().EndDraw())
				{
					isDrawing = False;

					return False;
				}
			}
			catch (Exception& exception)
			{
				isDrawing = False;

				throw Exception(
					Move(exception),
					"Error ending OS::Windows::DirectX::Direct2D draw"
				);
			}

			try
			{
				GetDirect3D().Present(
					IsVSyncEnabled()
				);
			}
			catch (Exception& exception)
			{
				isDrawing = False;

				throw Exception(
					Move(exception),
					"Error presenting OS::Windows::DirectX::Direct3D"
				);
			}
#endif

			isDrawing = False;

			return True;
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void FillCircle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().FillCircle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(radius)
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().FillCircle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(radius)
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void FillEllipse(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS radiusX, T_BOUNDS radiusY)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().FillEllipse(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(radiusX),
				BitConverter::Cast<Float>(radiusY)
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().FillEllipse(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(radiusX),
				BitConverter::Cast<Float>(radiusY)
			);
#endif
		}

		template<typename T_BOUNDS, typename T_BRUSH>
		Void FillRectangle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS width, T_BOUNDS height)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().FillRectangle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height)
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().FillRectangle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height)
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void FillRoundedRectangle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS width, T_BOUNDS height, Float radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().FillRoundedRectangle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				radius
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().FillRoundedRectangle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				radius
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_POSITION, typename T_BRUSH>
		Void DrawLine(const Vector2<T_POSITION>& start, const Vector2<T_POSITION>& end, const T_BRUSH& brush, Float thickness = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawLine(
				start.template Cast<Float>(),
				end.template Cast<Float>(),
				brush.GetColor(),
				thickness
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawLine(
				start.template Cast<Float>(),
				end.template Cast<Float>(),
				brush.GetResource(),
				thickness
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_POSITION, typename T_BRUSH>
		Void DrawString(const Vector2<T_POSITION>& position, const T_BRUSH& brush, const TextFormat& format, const String& value)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawString(
				position.template Cast<Float>(),
				brush.GetColor(),
				format.GetFont().Name,
				format.GetFont().Size,
				value
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawString(
				position.template Cast<Float>(),
				brush.GetResource(),
				format.GetResource(),
				value
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_POSITION>
		Void DrawImage(const Vector2<T_POSITION>& position, const Image& image, Float opacity = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawBitmap(
				position.template Cast<Float>(),
				image.GetResource()
			);
#endif
		}
		// @throw AL::Exception
		template<typename T_BOUNDS>
		Void DrawImage(const Vector2<T_BOUNDS>& position, const Image& image, T_BOUNDS width, T_BOUNDS height, Float opacity = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawBitmap(
				position.template Cast<Float>(),
				image.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				opacity
			);
#endif
		}
		// @throw AL::Exception
		template<typename T_BOUNDS>
		Void DrawImage(const Vector2<T_BOUNDS>& position, const Image& image, T_BOUNDS sourceX, T_BOUNDS sourceY, T_BOUNDS width, T_BOUNDS height, Float opacity = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawBitmap(
				position.template Cast<Float>(),
				image.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				BitConverter::Cast<Float>(sourceX),
				BitConverter::Cast<Float>(sourceY),
				opacity
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void DrawCircle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS radius, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawCircle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(radius),
				strokeWidth
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawCircle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(radius),
				strokeWidth
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void DrawEllipse(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS radiusX, T_BOUNDS radiusY, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawEllipse(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(radiusX),
				BitConverter::Cast<Float>(radiusY),
				strokeWidth
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawEllipse(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(radiusX),
				BitConverter::Cast<Float>(radiusY),
				strokeWidth
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void DrawRectangle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS width, T_BOUNDS height, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawRectangle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				strokeWidth
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawRectangle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				strokeWidth
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_BOUNDS, typename T_BRUSH>
		Void DrawRoundedRectangle(const Vector2<T_BOUNDS>& position, const T_BRUSH& brush, T_BOUNDS width, T_BOUNDS height, Float radius, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			GetOpenGL().DrawRoundedRectangle(
				position.template Cast<Float>(),
				brush.GetColor(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				radius,
				strokeWidth
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawRoundedRectangle(
				position.template Cast<Float>(),
				brush.GetResource(),
				BitConverter::Cast<Float>(width),
				BitConverter::Cast<Float>(height),
				radius,
				strokeWidth
			);
#endif
		}

		// @throw AL::Exception
		template<typename T_POSITION, typename T_BRUSH>
		Void DrawTextLayout(const Vector2<T_POSITION>& position, const T_BRUSH& brush, const TextLayout& layout)
		{
			AL_ASSERT(
				IsCreated(),
				"Graphics not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Graphics target not created"
			);

#if defined(AL_GAME_ENGINE_OPENGL)
			// TODO: optimize
			GetOpenGL().DrawString(
				position.template Cast<Float>(),
				brush.GetColor(),
				layout.GetFormat().GetFont().Name,
				layout.GetFormat().GetFont().Size,
				layout.GetText()
			);
#elif defined(AL_GAME_ENGINE_DIRECTX)
			GetDirect2D().DrawTextLayout(
				position.template Cast<Float>(),
				brush.GetResource(),
				layout.GetResource()
			);
#endif
		}

	private:
		// @throw AL::Exception
		Void Resources_Create()
		{
			for (auto it = resources.begin(); it != resources.end(); ++it)
			{
				try
				{
					(*it)->Create();
				}
				catch (Exception&)
				{
					while (it != resources.begin())
					{
						(*(--it))->Destroy();
					}

					throw;
				}
			}
		}

		Void Resources_Destroy()
		{
			for (auto it = resources.rbegin(); it != resources.rend(); ++it)
			{
				(*it)->Destroy();
			}
		}

		// @throw AL::Exception
		Void Resources_LoadContent()
		{
			for (auto it = resources.begin(); it != resources.end(); ++it)
			{
				try
				{
					(*it)->LoadContent();
				}
				catch (Exception&)
				{
					while (it != resources.begin())
					{
						(*(--it))->UnloadContent();
					}

					throw;
				}
			}
		}

		Void Resources_UnloadContent()
		{
			for (auto it = resources.rbegin(); it != resources.rend(); ++it)
			{
				(*it)->UnloadContent();
			}
		}

	private: // GraphicsResource access
		friend Image;
		friend TextFormat;
		friend TextLayout;
		friend SolidColorBrush;

		// @throw AL::Exception
		Void Resource_Register(GraphicsResource& resource)
		{
			resources.PushBack(
				&resource
			);

			if (IsCreated())
			{
				try
				{
					resource.Create();
				}
				catch (Exception&)
				{

					resources.PopBack();
				}

				if (IsTargetCreated())
				{
					try
					{
						resource.LoadContent();
					}
					catch (Exception&)
					{
						resource.Destroy();

						resources.PopBack();

						throw;
					}
				}
			}
		}

		Void Resource_Unregister(GraphicsResource& resource)
		{
			auto it = resources.Find(
				&resource
			);

			if (it != resources.end())
			{
				if (IsCreated())
				{
					if (IsTargetCreated())
					{

						resource.UnloadContent();
					}

					resource.Destroy();
				}

				resources.Erase(
					it
				);
			}
		}
	};
}

// @throw AL::Exception
inline AL::Game::Engine::Image::Image(Graphics& graphics, String&& path)
	: GraphicsResource(
		graphics
	),
	isFileSource(
		True
	),
	path(
		Move(path)
	)
{
	GetGraphics().Resource_Register(
		*this
	);
}
// @throw AL::Exception
inline AL::Game::Engine::Image::Image(Graphics& graphics, const Void* lpBuffer, size_t size)
	: GraphicsResource(
		graphics
	),
	isFileSource(
		False
	),
	buffer(
		reinterpret_cast<const uint8*>(lpBuffer),
		size
	)
{
	GetGraphics().Resource_Register(
		*this
	);
}

inline AL::Game::Engine::Image::~Image()
{
	GetGraphics().Resource_Unregister(
		*this
	);
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::Image::OnCreate()
{
}

inline AL::Void AL::Game::Engine::Image::OnDestroy()
{
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::Image::OnLoadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	if (IsFileSource())
	{
		GetGraphics().GetDirect2D().CreateBitmap(
			resource,
			GetPath()
		);
	}
	else
	{
		GetGraphics().GetDirect2D().CreateBitmap(
			resource,
			&GetBuffer()[0],
			GetBuffer().GetSize()
		);
	}
#endif
}

inline AL::Void AL::Game::Engine::Image::OnUnloadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	resource.Release();
#endif
}

// @throw AL::Exception
inline AL::Game::Engine::TextFormat::TextFormat(Graphics& graphics, Font&& font, TextAlignment alignment)
	: GraphicsResource(
		graphics
	),
	font(
		Move(font)
	),
	alignment(
		alignment
	)
{
	GetGraphics().Resource_Register(
		*this
	);
}

inline AL::Game::Engine::TextFormat::~TextFormat()
{
	GetGraphics().Resource_Unregister(
		*this
	);
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextFormat::SetFont(Font&& value)
{
	font = Move(
		value
	);

	if (IsContentLoaded())
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
		GetGraphics().GetDirect2D().CreateTextFormat(
			resource,
			GetFont().Name,
			GetFont().Size
		);
#endif
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextFormat::SetAlignment(TextAlignment value)
{
	alignment = value;

	if (IsContentLoaded())
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
		{
			auto alignment = OS::Windows::DirectX::TextAlignment::Left;

			if (GetAlignment().IsSet(TextAlignment::Left))
			{
			}
			else if (GetAlignment().IsSet(TextAlignment::Right))
			{
				alignment = OS::Windows::DirectX::TextAlignment::Right;
			}
			else if (GetAlignment().IsSet(TextAlignment::Center_H))
			{
				alignment = OS::Windows::DirectX::TextAlignment::Center;
			}

			resource.SetTextAlignment(
				alignment
			);
		}

		{
			auto alignment = OS::Windows::DirectX::ParagraphAlignment::Top;

			if (GetAlignment().IsSet(TextAlignment::Top))
			{
			}
			else if (GetAlignment().IsSet(TextAlignment::Bottom))
			{
				alignment = OS::Windows::DirectX::ParagraphAlignment::Bottom;
			}
			else if (GetAlignment().IsSet(TextAlignment::Center_V))
			{
				alignment = OS::Windows::DirectX::ParagraphAlignment::Center;
			}

			resource.SetParagraphAlignment(
				alignment
			);
		}
#endif
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextFormat::OnCreate()
{
}

inline AL::Void AL::Game::Engine::TextFormat::OnDestroy()
{
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextFormat::OnLoadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	GetGraphics().GetDirect2D().CreateTextFormat(
		resource,
		GetFont().Name,
		GetFont().Size
	);

	{
		auto alignment = OS::Windows::DirectX::TextAlignment::Left;

		if (GetAlignment().IsSet(TextAlignment::Left))
		{
		}
		else if (GetAlignment().IsSet(TextAlignment::Right))
		{
			alignment = OS::Windows::DirectX::TextAlignment::Right;
		}
		else if (GetAlignment().IsSet(TextAlignment::Center_H))
		{
			alignment = OS::Windows::DirectX::TextAlignment::Center;
		}

		resource.SetTextAlignment(
			alignment
		);
	}

	{
		auto alignment = OS::Windows::DirectX::ParagraphAlignment::Top;

		if (GetAlignment().IsSet(TextAlignment::Top))
		{
		}
		else if (GetAlignment().IsSet(TextAlignment::Bottom))
		{
			alignment = OS::Windows::DirectX::ParagraphAlignment::Bottom;
		}
		else if (GetAlignment().IsSet(TextAlignment::Center_V))
		{
			alignment = OS::Windows::DirectX::ParagraphAlignment::Center;
		}

		resource.SetParagraphAlignment(
			alignment
		);
	}
#endif
}

inline AL::Void AL::Game::Engine::TextFormat::OnUnloadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	resource.Release();
#endif
}

// @throw AL::Exception
inline AL::Game::Engine::TextLayout::TextLayout(Graphics& graphics, TextFormat& format, String&& text, SizeF&& size)
	: GraphicsResource(
		graphics
	),
	text(
		Move(text)
	),
	size(
		Move(size)
	),
	lpFormat(
		&format
	)
{
	GetGraphics().Resource_Register(
		*this
	);
}

inline AL::Game::Engine::TextLayout::~TextLayout()
{
	GetGraphics().Resource_Unregister(
		*this
	);
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextLayout::SetText(String&& value)
{
	text = Move(
		value
	);

	if (IsContentLoaded())
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
		GetGraphics().GetDirect2D().CreateTextLayout(
			resource,
			GetFormat().GetResource(),
			GetText(),
			GetSize().Width,
			GetSize().Height
		);
#endif
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextLayout::SetFormat(TextFormat& value)
{
	lpFormat = &value;

	if (IsContentLoaded())
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
		GetGraphics().GetDirect2D().CreateTextLayout(
			resource,
			GetFormat().GetResource(),
			GetText(),
			GetSize().Width,
			GetSize().Height
		);
#endif
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextLayout::OnCreate()
{
}

inline AL::Void AL::Game::Engine::TextLayout::OnDestroy()
{
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::TextLayout::OnLoadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	GetGraphics().GetDirect2D().CreateTextLayout(
		resource,
		GetFormat().GetResource(),
		GetText(),
		GetSize().Width,
		GetSize().Height
	);
#endif
}

inline AL::Void AL::Game::Engine::TextLayout::OnUnloadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	resource.Release();
#endif
}

// @throw AL::Exception
inline AL::Game::Engine::SolidColorBrush::SolidColorBrush(Graphics& graphics, Color color)
	: GraphicsResource(
		graphics
	),
	color(
		color
	)
{
	GetGraphics().Resource_Register(
		*this
	);
}

inline AL::Game::Engine::SolidColorBrush::~SolidColorBrush()
{
	GetGraphics().Resource_Unregister(
		*this
	);
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::SolidColorBrush::SetColor(Color value)
{
	color = value;

	if (IsContentLoaded())
	{
#if defined(AL_GAME_ENGINE_OPENGL)
		// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
		GetGraphics().GetDirect2D().CreateSolidColorBrush(
			resource,
			GetColor()
		);
#endif
	}
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::SolidColorBrush::OnCreate()
{
}

inline AL::Void AL::Game::Engine::SolidColorBrush::OnDestroy()
{
}

// @throw AL::Exception
inline AL::Void AL::Game::Engine::SolidColorBrush::OnLoadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	GetGraphics().GetDirect2D().CreateSolidColorBrush(
		resource,
		GetColor()
	);
#endif
}

inline AL::Void AL::Game::Engine::SolidColorBrush::OnUnloadContent()
{
#if defined(AL_GAME_ENGINE_OPENGL)
	// TODO: implement
#elif defined(AL_GAME_ENGINE_DIRECTX)
	resource.Release();
#endif
}
