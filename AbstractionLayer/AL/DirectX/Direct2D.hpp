#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#include "AL/FileSystem/Path.hpp"

#if __has_include(<d2d1_3.h>)
	#include <d2d1_3.h>

	#define AL_DEPENDENCY_DIRECT2D_3

	#define AL_DEPENDENCY_DIRECT2D_VERSION 3
#elif __has_include(<d2d1_2.h>)
	#include <d2d1_2.h>

	#define AL_DEPENDENCY_DIRECT2D_2

	#define AL_DEPENDENCY_DIRECT2D_VERSION 2
#elif __has_include(<d2d1_1.h>)
	#include <d2d1_1.h>

	#define AL_DEPENDENCY_DIRECT2D_1

	#define AL_DEPENDENCY_DIRECT2D_VERSION 1
#elif __has_include(<d2d1.h>)
	#include <d2d1.h>

	#define AL_DEPENDENCY_DIRECT2D

	#define AL_DEPENDENCY_DIRECT2D_VERSION 0
#endif

#if __has_include(<dwrite_3.h>)
	#include <dwrite_3.h>

	#define AL_DEPENDENCY_DWRITE_3

	#define AL_DEPENDENCY_DWRITE_VERSION 3
#elif __has_include(<dwrite_2.h>)
	#include <dwrite_2.h>

	#define AL_DEPENDENCY_DWRITE_2

	#define AL_DEPENDENCY_DWRITE_VERSION 2
#elif __has_include(<dwrite_1.h>)
	#include <dwrite_1.h>

	#define AL_DEPENDENCY_DWRITE_1

	#define AL_DEPENDENCY_DWRITE_VERSION 1
#elif __has_include(<dwrite.h>)
	#include <dwrite.h>

	#define AL_DEPENDENCY_DWRITE

	#define AL_DEPENDENCY_DWRITE_VERSION 0
#endif

#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace AL::DirectX
{
	typedef D2D1::Matrix3x2F Transform;

	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct2DResource = Resource<T, TReleaser>;

	template<typename T>
	struct Size
	{
		T Width;
		T Height;

		Size()
		{
		}

		Size(T width, T height)
			: Width(
				width
			),
			Height(
				height
			)
		{
		}
	};

	typedef Size<float> SizeF;
	typedef Size<uint32> SizeU;
	
	template<typename T>
	using Point = Graphics::Vector2<T>;

	typedef Point<float> PointF;
	typedef Point<uint32> PointU;

	template<typename T>
	using Rectangle = Graphics::Rectangle<T>;

	typedef Rectangle<float> RectangleF;
	typedef Rectangle<uint32> RectangleU;
	
	class Bitmap
		: public Direct2DResource<ID2D1Bitmap>
	{
	public:
		using Direct2DResource<ID2D1Bitmap>::Direct2DResource;

		auto GetDpi() const
		{
			PointF dpi;

			GetResource()->GetDpi(
				&dpi.X,
				&dpi.Y
			);

			return dpi;
		}

		auto GetSize() const
		{
			auto _size = GetResource()->GetSize();

			SizeF size;
			size.Width = _size.width;
			size.Height = _size.height;

			return size;
		}

		auto GetPixelSize() const
		{
			auto _pixelSize = GetResource()->GetPixelSize();

			SizeU pixelSize;
			pixelSize.Width = _pixelSize.width;
			pixelSize.Height = _pixelSize.height;

			return pixelSize;
		}
	};

	class BitmapBrush
		: public Direct2DResource<ID2D1BitmapBrush>
	{
	public:
		using Direct2DResource<ID2D1BitmapBrush>::Direct2DResource;

		float GetOpacity() const
		{
			return GetResource()->GetOpacity();
		}

		auto GetTransform() const
		{
			Transform transform;

			GetResource()->GetTransform(
				&transform
			);

			return transform;
		}

		void SetBitmap(const Bitmap& bitmap)
		{
			GetResource()->SetBitmap(
				bitmap
			);
		}

		void SetOpacity(float opacity)
		{
			GetResource()->SetOpacity(
				opacity
			);
		}

		void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(
				transform
			);
		}
	};

	class SolidColorBrush
		: public Direct2DResource<ID2D1SolidColorBrush>
	{
	public:
		using Direct2DResource<ID2D1SolidColorBrush>::Direct2DResource;

		auto GetColor() const
		{
			auto _color = GetResource()->GetColor();

			return Graphics::Color(
				static_cast<uint8>(_color.r * 255),
				static_cast<uint8>(_color.g * 255),
				static_cast<uint8>(_color.b * 255),
				static_cast<uint8>(_color.a * 255)
			);
		}

		float GetOpacity() const
		{
			return GetResource()->GetOpacity();
		}

		auto GetTransform() const
		{
			Transform transform;

			GetResource()->GetTransform(
				&transform
			);

			return transform;
		}

		void SetColor(Graphics::Color color)
		{
			GetResource()->SetColor(
				D2D1::ColorF(
					static_cast<float>(color.R) / 255,
					static_cast<float>(color.G) / 255,
					static_cast<float>(color.B) / 255,
					static_cast<float>(color.A) / 255
				)
			);
		}

		void SetOpacity(float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct LinearGradientBrushReleaser
	{
		static void AddRef(ID2D1LinearGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			lpResource->AddRef();
			stopCollection->AddRef();
		}

		static void Release(ID2D1LinearGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			stopCollection->Release();
			lpResource->Release();
		}
	};

	class LinearGradientBrush
		: public Direct2DResource<ID2D1LinearGradientBrush, LinearGradientBrushReleaser>
	{
	public:
		using Direct2DResource<ID2D1LinearGradientBrush, LinearGradientBrushReleaser>::Direct2DResource;

		float GetOpacity() const
		{
			return GetResource()->GetOpacity();
		}

		auto GetTransform() const
		{
			Transform transform;

			GetResource()->GetTransform(
				&transform
			);

			return transform;
		}

		void SetOpacity(float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct RadialGradientBrushReleaser
	{
		static void AddRef(ID2D1RadialGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			lpResource->AddRef();
			stopCollection->AddRef();
		}

		static void Release(ID2D1RadialGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			stopCollection->Release();
			lpResource->Release();
		}
	};

	class RadialGradientBrush
		: public Direct2DResource<ID2D1RadialGradientBrush, RadialGradientBrushReleaser>
	{
	public:
		using Direct2DResource<ID2D1RadialGradientBrush, RadialGradientBrushReleaser>::Direct2DResource;

		float GetOpacity() const
		{
			return GetResource()->GetOpacity();
		}

		auto GetTransform() const
		{
			Transform transform;

			GetResource()->GetTransform(
				&transform
			);

			return transform;
		}

		void SetOpacity(float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct GradientStop
	{
		Graphics::Color Color;
		float           Position;

		GradientStop()
			: Color(
				Graphics::Colors::White
			),
			Position(
				0.0f
			)
		{
		}

		GradientStop(Graphics::Color color, float position)
			: Color(
				color
			),
			Position(
				position
			)
		{
		}
	};

	typedef Direct2DResource<ID2D1EllipseGeometry> EllipseGeometry;

	typedef Direct2DResource<ID2D1RectangleGeometry> RectangleGeometry;

	typedef Direct2DResource<ID2D1RoundedRectangleGeometry> RoundedRectangleGeometry;

	typedef Direct2DResource<ID2D1TransformedGeometry> TransformedGeometry;

	typedef Direct2DResource<ID2D1PathGeometry> PathGeometry;

	class Layer
		: public Direct2DResource<ID2D1Layer>
	{
		D2D1_LAYER_PARAMETERS params;

	public:
		Layer()
			: Layer(
				nullptr
			)
		{
		}

		Layer(Layer&& layer)
			: Direct2DResource<ID2D1Layer>(
				Move(layer)
			)
		{
			params = Move(
				layer.params
			);
		}

		Layer(Type* lpResource)
			: Direct2DResource<ID2D1Layer>(
				lpResource
			)
		{
			ZeroMemory(&params, sizeof(params));
			params.maskAntialiasMode = D2D1_ANTIALIAS_MODE_ALIASED;

			SetOpacity(
				1.0f
			);
		}

		bool IsAliasing() const
		{
			return params.maskAntialiasMode == D2D1_ANTIALIAS_MODE_ALIASED;
		}

		auto& GetParams()
		{
			return params;
		}
		auto& GetParams() const
		{
			return params;
		}

		void SetOpacity(float opacity)
		{
			params.opacity = opacity;
		}

		void SetSize(float width, float height)
		{
			params.contentBounds.right = params.contentBounds.left + width;
			params.contentBounds.bottom = params.contentBounds.top + height;
		}

		void SetPosition(float x, float y)
		{
			auto width = params.contentBounds.right - params.contentBounds.left;
			auto height = params.contentBounds.bottom - params.contentBounds.top;

			params.contentBounds.left = x;
			params.contentBounds.right = params.contentBounds.left + width;

			params.contentBounds.top = y;
			params.contentBounds.bottom = params.contentBounds.top + height;
		}

		void SetAliasing(bool set = true)
		{
			params.maskAntialiasMode = set ? D2D1_ANTIALIAS_MODE_ALIASED : D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
		}

		template<typename T>
		void SetGeometry(T& geometry)
		{
			params = D2D1::LayerParameters(
				D2D1::InfiniteRect(), geometry
			);
		}

		auto& operator = (Type* resource)
		{
			Direct2DResource<ID2D1Layer>::operator=(
				resource
			);

			return *this;
		}

		auto& operator = (Layer&& layer)
		{
			Direct2DResource<ID2D1Layer>::operator=(
				Move(layer)
			);

			params = Move(
				layer.params
			);

			return *this;
		}
	};

	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using DirectWriteResource = Resource<T, TReleaser>;

	enum class FontWeight
	{
		Light  = 300,
		Normal = 400,
		Medium = 500,
		Bold   = 600
	};

	enum class FontStyle
	{
		Normal  = DWRITE_FONT_STYLE_NORMAL,
		Oblique = DWRITE_FONT_STYLE_OBLIQUE,
		Italic  = DWRITE_FONT_STYLE_ITALIC
	};

	enum class FontStretch
	{
		Condensed = DWRITE_FONT_STRETCH_EXTRA_CONDENSED,
		Normal    = DWRITE_FONT_STRETCH_NORMAL,
		Expanded  = DWRITE_FONT_STRETCH_EXPANDED
	};

	enum class ParagraphAlignment
	{
		Top    = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		Bottom = DWRITE_PARAGRAPH_ALIGNMENT_FAR,
		Center = DWRITE_PARAGRAPH_ALIGNMENT_CENTER
	};

	enum class TextAlignment
	{
		Left   = DWRITE_TEXT_ALIGNMENT_LEADING,
		Right  = DWRITE_TEXT_ALIGNMENT_TRAILING,
		Center = DWRITE_TEXT_ALIGNMENT_CENTER
	};

	enum class ReadingDirection
	{
		LeftToRight = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT,
		RightToLeft = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT
	};

	enum class LineSpacingMethod
	{
		Default = DWRITE_LINE_SPACING_METHOD_DEFAULT,
		Uniform = DWRITE_LINE_SPACING_METHOD_UNIFORM
	};

	struct LineSpacingInfo
	{
		LineSpacingMethod Method;
		float             LineSpacing;
		float             BaseLine;
	};

	struct TextMetrics
	{
		float    Left;
		float    Top;
		float    Width;
		float    WidthIncludingTrailingWhitespace;
		float    Height;
		float    LayoutWidth;
		float    LayoutHeight;
		//uint32 maxBidiReorderingDepth;
		uint32   LineCount;
	};

	class TextFormat
		: public DirectWriteResource<IDWriteTextFormat>
	{
	public:
		using DirectWriteResource<IDWriteTextFormat>::DirectWriteResource;

		float GetFontSize() const
		{
			return GetResource()->GetFontSize();
		}

		auto GetFontStyle() const
		{
			return static_cast<FontStyle>(
				GetResource()->GetFontStyle()
			);
		}

		auto GetFontWeight() const
		{
			return static_cast<FontWeight>(
				GetResource()->GetFontWeight()
			);
		}

		auto GetFontStetch() const
		{
			return static_cast<FontStretch>(
				GetResource()->GetFontStretch()
			);
		}

		bool GetWordWrapping() const
		{
			return GetResource()->GetWordWrapping() == DWRITE_WORD_WRAPPING_WRAP;
		}

		auto GetTextAlignment() const
		{
			return static_cast<TextAlignment>(
				GetResource()->GetTextAlignment()
			);
		}

		auto GetReadingDirection() const
		{
			return static_cast<ReadingDirection>(
				GetResource()->GetReadingDirection()
			);
		}

		auto GetParagraphAlignment() const
		{
			return static_cast<ParagraphAlignment>(
				GetResource()->GetParagraphAlignment()
			);
		}

		// @throw AL::Exceptions::Exception
		auto GetLineSpacing() const
		{
			LineSpacingInfo info;

			if (FAILED(GetResource()->GetLineSpacing(reinterpret_cast<DWRITE_LINE_SPACING_METHOD*>(&info.Method), &info.LineSpacing, &info.BaseLine)))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::GetLineSpacing"
				);
			}

			return info;
		}

		// @throw AL::Exceptions::Exception
		void SetWordWrapping(bool wrap = true)
		{
			auto wordWrapping = wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP;

			if (FAILED(GetResource()->SetWordWrapping(wordWrapping)))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::SetWordWrapping"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetTextAlignment(TextAlignment textAlignment)
		{
			if (FAILED(GetResource()->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(textAlignment))))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::SetTextAlignment"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetReadingDirection(ReadingDirection readingDirection)
		{
			if (FAILED(GetResource()->SetReadingDirection(static_cast<DWRITE_READING_DIRECTION>(readingDirection))))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::SetReadingDirection"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetParagraphAlignment(ParagraphAlignment paragraphAlignment)
		{
			if (FAILED(GetResource()->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(paragraphAlignment))))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::SetParagraphAlignment"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetLineSpacing(LineSpacingMethod method, float lineSpacing, float baseLine)
		{
			if (FAILED(GetResource()->SetLineSpacing(static_cast<DWRITE_LINE_SPACING_METHOD>(method), lineSpacing, baseLine)))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextFormat::SetLineSpacing"
				);
			}
		}
	};

	class Direct2D;

	class TextLayout
		: public DirectWriteResource<IDWriteTextLayout>
	{
		bool clipBounds = false;

	public:
		TextLayout()
			: TextLayout(
				nullptr
			)
		{
		}

		TextLayout(TextLayout&& layout)
			: DirectWriteResource<IDWriteTextLayout>(
				Move(layout)
			)
		{
			clipBounds = layout.clipBounds;
			layout.clipBounds = false;
		}

		explicit TextLayout(Type* lpResource)
			: DirectWriteResource<IDWriteTextLayout>(
				lpResource
			)
		{
		}

		bool GetClipping() const
		{
			return clipBounds;
		}

		float GetMinWidth() const
		{
			float minWidth = 0;

			if (SUCCEEDED(GetResource()->DetermineMinWidth(&minWidth)))
			{

				return minWidth;
			}

			return GetResource()->GetMaxWidth();
		}

		bool GetWordWrapping() const
		{
			return GetResource()->GetWordWrapping() == DWRITE_WORD_WRAPPING_WRAP;
		}

		auto GetTextAlignment() const
		{
			return static_cast<TextAlignment>(
				GetResource()->GetTextAlignment()
			);
		}

		auto GetReadingDirection() const
		{
			return static_cast<ReadingDirection>(
				GetResource()->GetReadingDirection()
			);
		}

		auto GetParagraphAlignment() const
		{
			return static_cast<ParagraphAlignment>(
				GetResource()->GetParagraphAlignment()
			);
		}

		// @throw AL::Exceptions::Exception
		auto GetMetrics() const
		{
			DWRITE_TEXT_METRICS metrics;

			if (FAILED(GetResource()->GetMetrics(&metrics)))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextLayout::GetMetrics"
				);
			}

			TextMetrics textMetrics;
			textMetrics.Left = metrics.left;
			textMetrics.Top = metrics.top;
			textMetrics.Width = metrics.width;
			textMetrics.WidthIncludingTrailingWhitespace = metrics.widthIncludingTrailingWhitespace;
			textMetrics.Height = metrics.height;
			textMetrics.LayoutWidth = metrics.layoutWidth;
			textMetrics.LayoutHeight = metrics.layoutHeight;
			textMetrics.LineCount = metrics.lineCount;

			return textMetrics;
		}

		// @throw AL::Exceptions::Exception
		static auto GetMetrics(Direct2D& direct2D, const TextFormat& format, const String& string);
		// @throw AL::Exceptions::Exception
		static auto GetMetrics(Direct2D& direct2D, const TextFormat& format, const WString& string);

		void SetClipping(bool set = true)
		{
			clipBounds = set;
		}

		// @throw AL::Exceptions::Exception
		void SetWordWrapping(bool wrap = true)
		{
			auto wordWrapping = wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP;

			if (FAILED(GetResource()->SetWordWrapping(wordWrapping)))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextLayout::SetWordWrapping"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetTextAlignment(TextAlignment alignment)
		{
			if (FAILED(GetResource()->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(alignment))))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextLayout::SetTextAlignment"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void SetParagraphAlignment(ParagraphAlignment alignment)
		{
			if (FAILED(GetResource()->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(alignment))))
			{

				throw Exceptions::Exception(
					"Error calling IDWriteTextLayout::SetParagraphAlignment"
				);
			}
		}

		auto& operator =(Type* resource)
		{
			DirectWriteResource<IDWriteTextLayout>::operator=(
				resource
			);

			return *this;
		}

		auto& operator =(TextLayout&& layout)
		{
			DirectWriteResource<IDWriteTextLayout>::operator=(
				Move(layout)
			);

			clipBounds = layout.clipBounds;
			layout.clipBounds = false;

			return *this;
		}
	};

	class Direct2D
	{
		typedef Direct2DResource<ID2D1Factory1> Factory;
		typedef Direct2DResource<ID2D1RenderTarget> Target;

		typedef Direct2DResource<ID2D1HwndRenderTarget> HWNDTarget;

		typedef Direct2DResource<IWICImagingFactory> WICFactory;

		typedef DirectWriteResource<IDWriteFactory1> DWFactory;

		Factory factory;
		DWFactory dwFactory;
		WICFactory wicFactory;

		Target target;
		HWNDTarget hwndTarget;

		Target sharedTarget;

		Collections::List<Transform> transforms;

	public:
		Direct2D()
		{
			PushTransform(
				Transform::Identity()
			);
		}

		bool IsCreated() const
		{
			return factory.IsCreated();
		}

		bool IsTargetCreated() const
		{
			return sharedTarget.IsCreated();
		}

		auto& GetFactory() const
		{
			return factory;
		}

		const Target& GetTarget() const
		{
			return sharedTarget;
		}

		auto& GetDWFactory() const
		{
			return dwFactory;
		}

		auto& GetWICFactory() const
		{
			return wicFactory;
		}

		auto& GetTransform() const
		{
			return *transforms.GetFront();
		}

		void PushTransform(Transform&& transform)
		{
			transforms.PushFront(
				Move(transform)
			);

			if (IsTargetCreated())
			{
				GetTarget()->SetTransform(
					*transforms.GetFront()
				);
			}
		}
		void PushTransform(const Transform& transform)
		{
			transforms.PushFront(
				transform
			);

			if (IsTargetCreated())
			{
				GetTarget()->SetTransform(
					*transforms.GetFront()
				);
			}
		}

		bool PopTransform()
		{
			if (transforms.GetSize() >= 2)
			{
				transforms.PopFront();

				GetTarget()->SetTransform(
					*transforms.GetFront()
				);

				return true;
			}

			return false;
		}
		bool PopTransform(Transform& transform)
		{
			if (transforms.GetSize() >= 2)
			{
				transform = Move(
					*transforms.GetFront()
				);

				transforms.PopFront();

				GetTarget()->SetTransform(
					*transforms.GetFront()
				);

				return true;
			}

			return false;
		}

		// @throw AL::Exceptions::Exception
		void Create(bool multithreaded = false)
		{
			AL_ASSERT(!IsCreated(), "Direct2D already created");

			D2D1_FACTORY_OPTIONS options;
			ZeroMemory(&options, sizeof(options));

#if defined(AL_DEBUG)
			BitMask<D2D1_DEBUG_LEVEL> debugLevel;
			debugLevel.Add(D2D1_DEBUG_LEVEL_ERROR);
			debugLevel.Add(D2D1_DEBUG_LEVEL_WARNING);
			debugLevel.Add(D2D1_DEBUG_LEVEL_INFORMATION);

			options.debugLevel = debugLevel.Mask;
#endif

			typename Factory::Type* lpFactory;

			if (FAILED(D2D1CreateFactory(multithreaded ? D2D1_FACTORY_TYPE_MULTI_THREADED : D2D1_FACTORY_TYPE_SINGLE_THREADED, &lpFactory)))
			{

				throw Exceptions::Exception(
					"Error calling D2D1CreateFactory"
				);
			}

			typename DWFactory::Type* lpDWFactory;

			if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(typename DWFactory::Type), reinterpret_cast<IUnknown**>(&lpDWFactory))))
			{
				lpFactory->Release();

				throw Exceptions::Exception(
					"Error calling DWriteCreateFactory"
				);
			}

			if (FAILED(CoInitialize(nullptr)))
			{
				lpDWFactory->Release();

				lpFactory->Release();

				throw Exceptions::Exception(
					"Error calling CoInitialize"
				);
			}

			typename WICFactory::Type* lpWICFactory;

			if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&lpWICFactory))))
			{
				CoUninitialize();

				lpDWFactory->Release();

				lpFactory->Release();

				throw Exceptions::Exception(
					"Error calling CoCreateInstance"
				);
			}

			factory = lpFactory;
			dwFactory = lpDWFactory;
			wicFactory = lpWICFactory;
		}

		void Destroy()
		{
			if (IsCreated())
			{
				DestroyTarget();

				wicFactory.Release();
				CoUninitialize();

				dwFactory.Release();

				factory.Release();
			}
		}

		// @throw AL::Exceptions::Exception
		void CreateTarget(HWND hWnd)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(!IsTargetCreated(), "Direct2D target already created");

			FLOAT dpiX, dpiY;

			GetFactory()->GetDesktopDpi(&dpiX, &dpiY);

			auto properties = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_HARDWARE,
				D2D1::PixelFormat(
					DXGI_FORMAT_UNKNOWN,
					D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				dpiX,
				dpiY
			);

			auto hwndProperties = D2D1::HwndRenderTargetProperties(
				hWnd
			);

			typename HWNDTarget::Type* lpTarget;

			if (FAILED(GetFactory()->CreateHwndRenderTarget(
				properties,
				hwndProperties,
				&lpTarget
			)))
			{

				throw Exceptions::Exception(
					"Error creating HWND render target"
				);
			}

			hwndTarget = lpTarget;

			lpTarget->AddRef();
			sharedTarget = lpTarget;
		}
		// @throw AL::Exceptions::Exception
		void CreateTarget(IDXGISwapChain* lpSwapChain)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(!IsTargetCreated(), "Direct2D target already created");

			IDXGISurface* lpSurface;

			if (FAILED(lpSwapChain->GetBuffer(0, IID_PPV_ARGS(&lpSurface))))
			{

				throw Exceptions::Exception(
					"Error getting IDXGISurface*"
				);
			}

			FLOAT dpiX, dpiY;

			GetFactory()->GetDesktopDpi(&dpiX, &dpiY);

			auto properties = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(
					DXGI_FORMAT_UNKNOWN,
					D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				dpiX,
				dpiY
			);

			HRESULT hResult;
			typename Target::Type* lpTarget;

			if (FAILED(hResult = GetFactory()->CreateDxgiSurfaceRenderTarget(lpSurface, properties, &lpTarget)))
			{
				lpSurface->Release();

				throw Exceptions::Exception(
					"Error creating DXGI render target"
				);
			}

			lpSurface->Release();

			target = lpTarget;

			lpTarget->AddRef();
			sharedTarget = lpTarget;
		}

		void DestroyTarget()
		{
			if (IsTargetCreated())
			{
				target.Release();
				hwndTarget.Release();
				sharedTarget.Release();
			}
		}

		// @throw AL::Exceptions::Exception
		void CreateLayer(Layer& layer)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename Layer::Type* lpLayer;

			if (FAILED(GetTarget()->CreateLayer(&lpLayer)))
			{

				throw Exceptions::Exception(
					"Error creating ClipLayer"
				);
			}

			layer = lpLayer;
		}

		// @throw AL::Exceptions::Exception 
		void CreateSolidColorBrush(SolidColorBrush& brush, Graphics::Color color)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename SolidColorBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateSolidColorBrush(
				D2D1::ColorF(
					static_cast<float>(color.R) / 255,
					static_cast<float>(color.G) / 255,
					static_cast<float>(color.B) / 255,
					static_cast<float>(color.A) / 255
				),
				&lpBrush
			)))
			{

				throw Exceptions::Exception(
					"Error creating SolidColorBrush"
				);
			}

			brush = lpBrush;
		}
		// @throw AL::Exceptions::Exception
		void CreateBitmapBrush(BitmapBrush& brush, const Bitmap& bitmap, float opacity = 1.0f, const Transform& transform = Transform::Identity())
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename BitmapBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateBitmapBrush(
				bitmap,
				D2D1::BitmapBrushProperties(),
				D2D1::BrushProperties(
					opacity,
					transform
				),
				&lpBrush
			)))
			{

				throw Exceptions::Exception(
					"Error creating BitmapBrush"
				);
			}

			brush = lpBrush;
		}
		// @throw AL::Exceptions::Exception
		template<size_t S>
		void CreateLinearGradientBrush(LinearGradientBrush& brush, const PointF& start, const PointF& end, const GradientStop(&stops)[S])
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			Collections::Array<D2D1_GRADIENT_STOP> _stops(
				S
			);

			for (size_t i = 0; i < _stops.GetSize(); i++)
			{
				_stops[i].color.a = stops[i].Color.A;
				_stops[i].color.r = stops[i].Color.R;
				_stops[i].color.g = stops[i].Color.G;
				_stops[i].color.b = stops[i].Color.B;

				_stops[i].position = stops[i].Position;
			}

			ID2D1GradientStopCollection* lpStopCollection;

			if (FAILED(GetTarget()->CreateGradientStopCollection(&_stops[0], static_cast<UINT>(_stops.GetSize()), &lpStopCollection)))
			{

				throw Exceptions::Exception(
					"Error creating GradientStopCollection"
				);
			}

			typename LinearGradientBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateLinearGradientBrush(
				D2D1::LinearGradientBrushProperties(
					D2D1::Point2F(
						start.X,
						start.Y
					),
					D2D1::Point2F(
						end.X,
						end.Y
					)
				),
				lpStopCollection,
				&lpBrush
			)))
			{
				lpStopCollection->Release();

				throw Exceptions::Exception(
					"Error creating LinearGradientBrush"
				);
			}

			lpStopCollection->Release();

			brush = lpBrush;
		}
		// @throw AL::Exceptions::Exception
		template<size_t S>
		void CreateRadialGradientBrush(RadialGradientBrush& brush, const PointF& center, const PointF& offset, const PointF& radius, const GradientStop(&stops)[S])
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			Collections::Array<D2D1_GRADIENT_STOP> _stops(
				S
			);

			for (size_t i = 0; i < _stops.GetSize(); i++)
			{
				_stops[i].color.a = stops[i].Color.A;
				_stops[i].color.r = stops[i].Color.R;
				_stops[i].color.g = stops[i].Color.G;
				_stops[i].color.b = stops[i].Color.B;

				_stops[i].position = stops[i].Position;
			}

			ID2D1GradientStopCollection* lpStopCollection;

			if (FAILED(GetTarget()->CreateGradientStopCollection(&_stops[0], static_cast<UINT>(_stops.GetSize()), &lpStopCollection)))
			{
				throw Exceptions::Exception(
					"Error creating GradientStopCollection"
				);
			}

			RadialGradientBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties(
					D2D1::Point2F(
						center.X,
						center.Y
					),
					D2D1::Point2F(
						offset.X,
						offset.Y
					),
					radius.X,
					radius.Y
				),
				lpStopCollection,
				&lpBrush)))
			{
				lpStopCollection->Release();

				throw Exceptions::Exception(
					"Error creating RadialGradientBrush"
				);
			}

			lpStopCollection->Release();

			brush = lpBrush;
		}

		// @throw AL::Exceptions::Exception
		void CreateBitmap(Bitmap& bitmap, const FileSystem::Path& file, bool color = true)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			auto fileName = WString::Format(
				L"%S",
				file.GetString().GetCString()
			);

			IWICBitmapDecoder* lpWICDecoder;

			if (FAILED(GetWICFactory()->CreateDecoderFromFilename(fileName.GetCString(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &lpWICDecoder)))
			{

				throw Exceptions::Exception(
					"Error creating IWICBitmapDecoder"
				);
			}

			try
			{
				CreateBitmapFromDecoder(
					bitmap,
					lpWICDecoder,
					color
				);
			}
			catch (Exceptions::Exception& exception)
			{
				lpWICDecoder->Release();

				throw Exceptions::Exception(
					Move(exception),
					"Error creating bitmap from decoder"
				);
			}

			lpWICDecoder->Release();
		}
		// @throw AL::Exceptions::Exception
		void CreateBitmap(Bitmap& bitmap, const void* lpBuffer, size_t bufferSize, bool color = true)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			IWICStream* lpStream;

			if (FAILED(GetWICFactory()->CreateStream(&lpStream)))
			{

				throw Exceptions::Exception(
					"Error creating IWICStream"
				);
			}

			if (FAILED(lpStream->InitializeFromMemory(reinterpret_cast<BYTE*>(const_cast<void*>(lpBuffer)), static_cast<DWORD>(bufferSize))))
			{
				lpStream->Release();

				throw Exceptions::Exception(
					"Error initializing Bitmap buffer from memory"
				);
			}

			IWICBitmapDecoder* lpWICDecoder;

			if (FAILED(GetWICFactory()->CreateDecoderFromStream(lpStream, nullptr, WICDecodeMetadataCacheOnLoad, &lpWICDecoder)))
			{
				lpStream->Release();

				throw Exceptions::Exception(
					"Error creating IWICBitmapDecoder"
				);
			}

			lpStream->Release();

			try
			{
				CreateBitmapFromDecoder(
					bitmap,
					lpWICDecoder,
					color
				);
			}
			catch (Exceptions::Exception& exception)
			{
				lpWICDecoder->Release();

				throw Exceptions::Exception(
					Move(exception),
					"Error creating bitmap from decoder"
				);
			}

			lpWICDecoder->Release();
		}

		// @throw AL::Exceptions::Exception
		void CreateTextFormat(TextFormat& textFormat, const String& fontName, float fontSize)
		{
			CreateTextFormat(
				textFormat,
				fontName,
				fontSize,
				FontWeight::Normal,
				FontStyle::Normal,
				FontStretch::Normal
			);
		}
		// @throw AL::Exceptions::Exception
		void CreateTextFormat(TextFormat& textFormat, const String& fontName, float fontSize, FontWeight fontWeight, FontStyle fontStyle, FontStretch fontStretch)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			auto fontFamily = WString::Format(
				L"%S",
				fontName.GetCString()
			);

			typename TextFormat::Type* lpFormat;

			if (FAILED(GetDWFactory()->CreateTextFormat(fontFamily.GetCString(), nullptr, static_cast<DWRITE_FONT_WEIGHT>(fontWeight), static_cast<DWRITE_FONT_STYLE>(fontStyle), static_cast<DWRITE_FONT_STRETCH>(fontStretch), fontSize, L"", &lpFormat)))
			{

				throw Exceptions::Exception(
					"Error creating TextFormat"
				);
			}

			textFormat = lpFormat;
		}

		// @throw AL::Exceptions::Exception
		void CreateTextLayout(TextLayout& textLayout, const TextFormat& textFormat, const String& text, float width, float height)
		{
			auto wtext = WString::Format(
				L"%S",
				text.GetCString()
			);

			CreateTextLayout(
				textLayout,
				textFormat,
				wtext,
				width,
				height
			);
		}
		// @throw AL::Exceptions::Exception
		void CreateTextLayout(TextLayout& textLayout, const TextFormat& textFormat, const WString& text, float width, float height)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename TextLayout::Type* lpLayout;

			if (FAILED(GetDWFactory()->CreateTextLayout(text.GetCString(), static_cast<UINT32>(text.GetLength()), textFormat, width, height, &lpLayout)))
			{

				throw Exceptions::Exception(
					"Error creating TextLayout"
				);
			}

			textLayout = lpLayout;
		}

		// @throw AL::Exceptions::Exception
		template<typename T>
		void CreateTransformedGeometry(TransformedGeometry& geometry, const T& sourceGeometry, const Transform& transform)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename TransformedGeometry::Type* lpGeomtry;

			if (FAILED(GetTarget()->CreateTransformedGeometry(sourceGeometry, transform, &lpGeomtry)))
			{

				throw Exceptions::Exception(
					"Error creating TransformedGeometry"
				);
			}

			geometry = lpGeomtry;
		}

		// @throw AL::Exceptions::Exception
		void CreatePathGeometry(PathGeometry& geometry)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename PathGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreatePathGeometry(&lpGeometry)))
			{

				throw Exceptions::Exception(
					"Error creating PathGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exceptions::Exception
		void CreateEllipseGeometry(EllipseGeometry& geometry, const PointF& center, float radiusX, float radiusY)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename EllipseGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateEllipseGeometry(
				D2D1::Ellipse(
					D2D1::Point2F(
						center.X, center.Y
					),
					radiusX,
					radiusY
				),
				&lpGeometry
			)))
			{
				throw Exceptions::Exception(
					"Error creating EllipseGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exceptions::Exception
		void CreateRectangleGeometry(RectangleGeometry& geometry, const RectangleF& rectangle)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			typename RectangleGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateRectangleGeometry(
				D2D1::RectF(
					rectangle.Left,
					rectangle.Top,
					rectangle.Right,
					rectangle.Bottom
				),
				&lpGeometry
			)))
			{

				throw Exceptions::Exception(
					"Error creating RectangleGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exceptions::Exception
		void CreateRoundedRectangleGeometry(RoundedRectangleGeometry& geometry, const RectangleF& rectangle, float radius)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			RoundedRectangleGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateRoundedRectangleGeometry(
				D2D1::RoundedRect(
					D2D1::RectF(
						rectangle.Left,
						rectangle.Top,
						rectangle.Right,
						rectangle.Bottom
					),
					radius,
					radius
				),
				&lpGeometry
			)))
			{
				throw Exceptions::Exception(
					"Error creating RoundedRectangleGeometry"
				);
			}

			geometry = lpGeometry;
		}

		void PushLayer(const Layer& layer)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->PushLayer(
				layer.GetParams(),
				layer
			);
		}

		void PopLayer()
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->PopLayer();
		}

		void PushClip(const RectangleF& rectangle, bool antialias = false)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->PushAxisAlignedClip(
				D2D1::RectF(
					rectangle.Left,
					rectangle.Top,
					rectangle.Right,
					rectangle.Bottom
				),
				antialias ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED
			);
		}

		void PopClip()
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->PopAxisAlignedClip();
		}

		void DrawBitmap(const PointF& position, const Bitmap& bitmap, float opacity = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			auto size = bitmap.GetSize();

			DrawBitmap(
				position,
				bitmap,
				size.Width,
				size.Height,
				opacity
			);
		}
		void DrawBitmap(const PointF& position, const Bitmap& bitmap, float width, float height, float opacity = 1.0f) const
		{
			DrawBitmap(
				position,
				bitmap,
				width,
				height,
				0,
				0,
				opacity
			);
		}
		void DrawBitmap(const PointF& position, const Bitmap& bitmap, float width, float height, float sourceX, float sourceY, float opacity = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			auto size = bitmap.GetSize();

			auto src = D2D1::RectF(
				sourceX,
				sourceY,
				size.Width,
				size.Height
			);

			auto dst = D2D1::RectF(
				position.X,
				position.Y,
				position.X + width,
				position.Y + height
			);

			GetTarget()->DrawBitmap(
				bitmap,
				dst,
				opacity,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				src
			);
		}

		template<typename T>
		void DrawString(const PointF& position, const T& brush, const TextFormat& textFormat, const String& text) const
		{
			DrawString(
				position,
				brush,
				textFormat,
				WString::Format(
					L"%S",
					text.GetCString()
				)
			);
		}
		template<typename T>
		void DrawString(const PointF& position, const T& brush, const TextFormat& textFormat, const WString& text) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawTextA(
				text.GetCString(),
				static_cast<UINT>(text.GetLength()),
				textFormat,
				D2D1::RectF(
					position.X,
					position.Y,
					position.X + (text.GetLength() * textFormat.GetFontSize()),
					position.Y + textFormat.GetFontSize()
				),
				brush
			);
		}

		template<typename T>
		void DrawTextLayout(const PointF& position, const T& brush, const TextLayout& textLayout) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawTextLayout(
				D2D1::Point2F(
					position.X,
					position.Y
				),
				textLayout,
				brush,
				textLayout.GetClipping() ? D2D1_DRAW_TEXT_OPTIONS_CLIP : D2D1_DRAW_TEXT_OPTIONS_NONE
			);
		}

		template<typename T>
		void DrawLine(const PointF& start, const PointF& end, const T& brush, float thickness = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawLine(
				D2D1::Point2F(
					start.X,
					start.Y
				),
				D2D1::Point2F(
					end.X,
					end.Y
				),
				brush,
				thickness
			);
		}

		template<typename T>
		void DrawEllipse(const PointF& position, const T& brush, float radiusX, float radiusY, float strokeWidth = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(
						position.X,
						position.Y
					),
					radiusX,
					radiusY
				),
				brush,
				strokeWidth
			);
		}

		template<typename T>
		void DrawCircle(const PointF& position, const T& brush, float radius, float strokeWidth = 1.0f) const
		{
			DrawEllipse(
				position,
				brush,
				radius,
				radius,
				strokeWidth
			);
		}

		template<typename T>
		void DrawRectangle(const PointF& position, const T& brush, float width, float height, float strokeWidth = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawRectangle(
				D2D1::RectF(
					position.X,
					position.Y,
					position.X + width,
					position.Y + height
				),
				brush,
				strokeWidth
			);
		}

		template<typename T>
		void DrawRoundedRectangle(const PointF& position, const T& brush, float width, float height, float radius, float strokeWidth = 1.0f) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->DrawRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(
						position.X,
						position.Y,
						position.X + width,
						position.Y + height
					),
					radius,
					radius
				),
				brush,
				strokeWidth
			);
		}

		template<typename T>
		void FillEllipse(const PointF& position, const T& brush, float radiusX, float radiusY) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(
						position.X,
						position.Y
					),
					radiusX,
					radiusY
				),
				brush
			);
		}

		template<typename T>
		void FillCircle(const PointF& position, const T& brush, float radius) const
		{
			FillEllipse(
				position,
				brush,
				radius,
				radius
			);
		}

		template<typename T>
		void FillRectangle(const PointF& position, const T& brush, float width, float height) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->FillRectangle(
				D2D1::RectF(
					position.X,
					position.Y,
					position.X + width,
					position.Y + height
				),
				brush
			);
		}

		template<typename T>
		void FillRoundedRectangle(const PointF& position, const T& brush, float width, float height, float radius) const
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			AL_ASSERT(IsTargetCreated(), "Direct2D target not created");

			GetTarget()->FillRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(
						position.X,
						position.Y,
						position.X + width,
						position.Y + height
					),
					radius,
					radius
				),
				brush
			);
		}

		void BeginDraw()
		{
			Target::Type* lpTarget;

			AL_ASSERT((lpTarget = &(*GetTarget())), "Direct2D target not created");

			lpTarget->BeginDraw();
		}

		void Clear(Graphics::Color color)
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");

			Target::Type* lpTarget;

			AL_ASSERT((lpTarget = &(*GetTarget())), "Direct2D target not created");

			D2D1_COLOR_F colorf;
			colorf.a = static_cast<float>(color.A) * 255;
			colorf.r = static_cast<float>(color.R) * 255;
			colorf.g = static_cast<float>(color.G) * 255;
			colorf.b = static_cast<float>(color.B) * 255;

			lpTarget->Clear(
				colorf
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false on D2DERR_RECREATE_TARGET
		bool EndDraw()
		{
			AL_ASSERT(IsCreated(), "Direct2D not created");
			
			Target::Type* lpTarget;

			AL_ASSERT((lpTarget = &(*GetTarget())), "Direct2D target not created");

			HRESULT hResult;

			if (FAILED(hResult = lpTarget->EndDraw()))
			{
				if (hResult == D2DERR_RECREATE_TARGET)
				{

					return false;
				}

				throw Exceptions::Exception(
					"Error caling %s::EndDraw",
					Get_Type_Name<decltype(*lpTarget)>::Value
				);
			}

			return true;
		}

	private:
		// @throw AL::Exceptions::Exception
		void CreateBitmapFromDecoder(Bitmap& bitmap, IWICBitmapDecoder* wicDecoder, bool color)
		{
			IWICBitmapFrameDecode* lpWICFrame;

			if (FAILED(wicDecoder->GetFrame(0, &lpWICFrame)))
			{

				throw Exceptions::Exception(
					"Error creating IWICBitmapFrameDecode"
				);
			}

			IWICFormatConverter* lpWICConverter;

			if (FAILED(GetWICFactory()->CreateFormatConverter(&lpWICConverter)))
			{
				lpWICFrame->Release();

				throw Exceptions::Exception(
					"Error creating IWICFormatConverter"
				);
			}

			if (FAILED(lpWICConverter->Initialize(
				lpWICFrame,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.0, // alpha
				color ? WICBitmapPaletteTypeMedianCut : WICBitmapPaletteTypeFixedBW)))
			{
				lpWICFrame->Release();

				lpWICConverter->Release();

				throw Exceptions::Exception(
					"Error initializing IWICFormatConverter"
				);
			}

			typename Bitmap::Type* lpBitmap;

			if (FAILED(GetTarget()->CreateBitmapFromWicBitmap(lpWICConverter, nullptr, &lpBitmap)))
			{
				lpWICFrame->Release();

				lpWICConverter->Release();

				throw Exceptions::Exception(
					"Error creating ID2D1Bitmap"
				);
			}

			lpWICFrame->Release();
			lpWICConverter->Release();

			bitmap = lpBitmap;
		}
	};
}

// @throw AL::Exceptions::Exception
inline auto AL::DirectX::TextLayout::GetMetrics(Direct2D& direct2D, const TextFormat& format, const String& string)
{
	TextLayout layout;

	auto size = direct2D.GetTarget()->GetSize();

	try
	{
		direct2D.CreateTextLayout(
			layout,
			format,
			string,
			size.width,
			size.height
		);
	}
	catch (Exceptions::Exception& exception)
	{

		throw Exceptions::Exception(
			Move(exception),
			"Error creating text layout"
		);
	}

	try
	{
		auto metrics = layout.GetMetrics();

		layout.Release();

		return metrics;
	}
	catch (Exceptions::Exception& exception)
	{
		layout.Release();

		throw Exceptions::Exception(
			Move(exception),
			"Error getting layout metrics"
		);
	}
}
// @throw AL::Exceptions::Exception
inline auto AL::DirectX::TextLayout::GetMetrics(Direct2D& direct2D, const TextFormat& format, const WString& string)
{
	TextLayout layout;

	auto size = direct2D.GetTarget()->GetSize();

	try
	{
		direct2D.CreateTextLayout(
			layout,
			format,
			string,
			size.width,
			size.height
		);
	}
	catch (Exceptions::Exception& exception)
	{

		throw Exceptions::Exception(
			Move(exception),
			"Error creating text layout"
		);
	}

	try
	{
		auto metrics = layout.GetMetrics();

		layout.Release();

		return metrics;
	}
	catch (Exceptions::Exception& exception)
	{
		layout.Release();

		throw Exceptions::Exception(
			Move(exception),
			"Error getting layout metrics"
		);
	}
}
