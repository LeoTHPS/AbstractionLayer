#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

#include "Resource.hpp"

#include "AL/Collections/LinkedList.hpp"

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#include "AL/FileSystem/Path.hpp"

#if AL_HAS_INCLUDE(<d2d1_3.h>)
	#include <d2d1_3.h>

	#define AL_DEPENDENCY_DIRECT2D
	#define AL_DEPENDENCY_DIRECT2D_3
#elif AL_HAS_INCLUDE(<d2d1_2.h>)
	#include <d2d1_2.h>

	#define AL_DEPENDENCY_DIRECT2D
	#define AL_DEPENDENCY_DIRECT2D_2
#elif AL_HAS_INCLUDE(<d2d1_1.h>)
	#include <d2d1_1.h>

	#define AL_DEPENDENCY_DIRECT2D
	#define AL_DEPENDENCY_DIRECT2D_1
#elif AL_HAS_INCLUDE(<d2d1.h>)
	#include <d2d1.h>

	#define AL_DEPENDENCY_DIRECT2D
#else

#endif

#if AL_HAS_INCLUDE(<dwrite_3.h>)
	#include <dwrite_3.h>

	#define AL_DEPENDENCY_DWRITE
	#define AL_DEPENDENCY_DWRITE_3
#elif AL_HAS_INCLUDE(<dwrite_2.h>)
	#include <dwrite_2.h>

	#define AL_DEPENDENCY_DWRITE
	#define AL_DEPENDENCY_DWRITE_2
#elif AL_HAS_INCLUDE(<dwrite_1.h>)
	#include <dwrite_1.h>

	#define AL_DEPENDENCY_DWRITE
	#define AL_DEPENDENCY_DWRITE_1
#elif AL_HAS_INCLUDE(<dwrite.h>)
	#include <dwrite.h>

	#define AL_DEPENDENCY_DWRITE
#else

#endif

#include <wincodec.h>

#if defined(AL_COMPILER_MSVC)
	#if defined(AL_DEPENDENCY_DIRECT2D)
		#pragma comment(lib, "d2d1.lib")
		#pragma comment(lib, "winmm.lib")
		#pragma comment(lib, "windowscodecs.lib")
	#endif

	#if defined(AL_DEPENDENCY_DWRITE)
		#pragma comment(lib, "dwrite.lib")
	#endif
#endif

namespace AL::OS::Windows::DirectX
{
	typedef D2D1::Matrix3x2F Transform;

	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct2DResource = Resource<T, TReleaser>;

	class Bitmap
		: public Direct2DResource<::ID2D1Bitmap>
	{
	public:
		using Direct2DResource<::ID2D1Bitmap>::Direct2DResource;

		auto GetDpi() const
		{
			Drawing::Vector2F dpi;

			GetResource()->GetDpi(
				&dpi.X,
				&dpi.Y
			);

			return dpi;
		}

		auto GetSize() const
		{
			auto _size = GetResource()->GetSize();

			Drawing::SizeF size;
			size.Width = _size.width;
			size.Height = _size.height;

			return size;
		}

		auto GetPixelSize() const
		{
			auto _pixelSize = GetResource()->GetPixelSize();

			Drawing::SizeU32 pixelSize;
			pixelSize.Width = _pixelSize.width;
			pixelSize.Height = _pixelSize.height;

			return pixelSize;
		}
	};

	class BitmapBrush
		: public Direct2DResource<::ID2D1BitmapBrush>
	{
	public:
		using Direct2DResource<::ID2D1BitmapBrush>::Direct2DResource;

		Float GetOpacity() const
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

		Void SetBitmap(const Bitmap& bitmap)
		{
			GetResource()->SetBitmap(
				bitmap
			);
		}

		Void SetOpacity(Float opacity)
		{
			GetResource()->SetOpacity(
				opacity
			);
		}

		Void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(
				transform
			);
		}
	};

	class SolidColorBrush
		: public Direct2DResource<::ID2D1SolidColorBrush>
	{
	public:
		using Direct2DResource<::ID2D1SolidColorBrush>::Direct2DResource;

		auto GetColor() const
		{
			auto _color = GetResource()->GetColor();

			return Drawing::Color(
				static_cast<uint8>(_color.r * 255),
				static_cast<uint8>(_color.g * 255),
				static_cast<uint8>(_color.b * 255),
				static_cast<uint8>(_color.a * 255)
			);
		}

		Float GetOpacity() const
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

		Void SetColor(Drawing::Color color)
		{
			GetResource()->SetColor(
				::D2D1::ColorF(
					static_cast<Float>(color.R) / 255,
					static_cast<Float>(color.G) / 255,
					static_cast<Float>(color.B) / 255,
					static_cast<Float>(color.A) / 255
				)
			);
		}

		Void SetOpacity(Float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		Void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct LinearGradientBrushReleaser
	{
		static Void AddRef(ID2D1LinearGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			lpResource->AddRef();
			stopCollection->AddRef();
		}

		static Void Release(ID2D1LinearGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			stopCollection->Release();
			lpResource->Release();
		}
	};

	class LinearGradientBrush
		: public Direct2DResource<::ID2D1LinearGradientBrush, LinearGradientBrushReleaser>
	{
	public:
		using Direct2DResource<::ID2D1LinearGradientBrush, LinearGradientBrushReleaser>::Direct2DResource;

		Float GetOpacity() const
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

		Void SetOpacity(Float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		Void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct RadialGradientBrushReleaser
	{
		static Void AddRef(ID2D1RadialGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			lpResource->AddRef();
			stopCollection->AddRef();
		}

		static Void Release(ID2D1RadialGradientBrush* lpResource)
		{
			ID2D1GradientStopCollection* stopCollection;
			lpResource->GetGradientStopCollection(&stopCollection);

			stopCollection->Release();
			lpResource->Release();
		}
	};

	class RadialGradientBrush
		: public Direct2DResource<::ID2D1RadialGradientBrush, RadialGradientBrushReleaser>
	{
	public:
		using Direct2DResource<::ID2D1RadialGradientBrush, RadialGradientBrushReleaser>::Direct2DResource;

		Float GetOpacity() const
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

		Void SetOpacity(Float opacity)
		{
			GetResource()->SetOpacity(opacity);
		}

		Void SetTransform(const Transform& transform)
		{
			GetResource()->SetTransform(transform);
		}
	};

	struct GradientStop
	{
		Drawing::Color Color;
		Float          Position;

		GradientStop()
			: Color(
				Drawing::Colors::White
			),
			Position(
				0.0f
			)
		{
		}

		GradientStop(Drawing::Color color, Float position)
			: Color(
				color
			),
			Position(
				position
			)
		{
		}
	};

	typedef Direct2DResource<ID2D1EllipseGeometry>          EllipseGeometry;
	typedef Direct2DResource<ID2D1RectangleGeometry>        RectangleGeometry;
	typedef Direct2DResource<ID2D1RoundedRectangleGeometry> RoundedRectangleGeometry;
	typedef Direct2DResource<ID2D1TransformedGeometry>      TransformedGeometry;
	typedef Direct2DResource<ID2D1PathGeometry>             PathGeometry;

	class Layer
		: public Direct2DResource<::ID2D1Layer>
	{
		::D2D1_LAYER_PARAMETERS params;

	public:
		Layer()
			: Layer(
				nullptr
			)
		{
		}

		Layer(Layer&& layer)
			: Direct2DResource<::ID2D1Layer>(
				Move(layer)
			)
		{
			params = Move(
				layer.params
			);
		}

		Layer(Type* lpResource)
			: Direct2DResource<::ID2D1Layer>(
				lpResource
			)
		{
			ZeroMemory(&params, sizeof(params));
			params.maskAntialiasMode = ::D2D1_ANTIALIAS_MODE_ALIASED;

			SetOpacity(
				1.0f
			);
		}

		Bool IsAliasing() const
		{
			return params.maskAntialiasMode == ::D2D1_ANTIALIAS_MODE_ALIASED;
		}

		auto& GetParams()
		{
			return params;
		}
		auto& GetParams() const
		{
			return params;
		}

		Void SetOpacity(Float opacity)
		{
			params.opacity = opacity;
		}

		Void SetSize(Float width, Float height)
		{
			params.contentBounds.right = params.contentBounds.left + width;
			params.contentBounds.bottom = params.contentBounds.top + height;
		}

		Void SetPosition(Float x, Float y)
		{
			auto width = params.contentBounds.right - params.contentBounds.left;
			auto height = params.contentBounds.bottom - params.contentBounds.top;

			params.contentBounds.left = x;
			params.contentBounds.right = params.contentBounds.left + width;

			params.contentBounds.top = y;
			params.contentBounds.bottom = params.contentBounds.top + height;
		}

		Void SetAliasing(Bool set = True)
		{
			params.maskAntialiasMode = set ? ::D2D1_ANTIALIAS_MODE_ALIASED : ::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
		}

		template<typename T>
		Void SetGeometry(T& geometry)
		{
			params = ::D2D1::LayerParameters(
				::D2D1::InfiniteRect(), geometry
			);
		}

		auto& operator = (Type* resource)
		{
			Direct2DResource<::ID2D1Layer>::operator=(
				resource
			);

			return *this;
		}

		auto& operator = (Layer&& layer)
		{
			Direct2DResource<::ID2D1Layer>::operator=(
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
		Normal  = ::DWRITE_FONT_STYLE_NORMAL,
		Oblique = ::DWRITE_FONT_STYLE_OBLIQUE,
		Italic  = ::DWRITE_FONT_STYLE_ITALIC
	};

	enum class FontStretch
	{
		Condensed = ::DWRITE_FONT_STRETCH_EXTRA_CONDENSED,
		Normal    = ::DWRITE_FONT_STRETCH_NORMAL,
		Expanded  = ::DWRITE_FONT_STRETCH_EXPANDED
	};

	enum class ParagraphAlignment
	{
		Top    = ::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		Bottom = ::DWRITE_PARAGRAPH_ALIGNMENT_FAR,
		Center = ::DWRITE_PARAGRAPH_ALIGNMENT_CENTER
	};

	enum class TextAlignment
	{
		Left   = ::DWRITE_TEXT_ALIGNMENT_LEADING,
		Right  = ::DWRITE_TEXT_ALIGNMENT_TRAILING,
		Center = ::DWRITE_TEXT_ALIGNMENT_CENTER
	};

	enum class ReadingDirection
	{
		LeftToRight = ::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT,
		RightToLeft = ::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT
	};

	enum class LineSpacingMethod
	{
		Default = ::DWRITE_LINE_SPACING_METHOD_DEFAULT,
		Uniform = ::DWRITE_LINE_SPACING_METHOD_UNIFORM
	};

	struct LineSpacingInfo
	{
		LineSpacingMethod Method;
		Float             LineSpacing;
		Float             BaseLine;
	};

	struct TextMetrics
	{
		Float    Left;
		Float    Top;
		Float    Width;
		Float    WidthIncludingTrailingWhitespace;
		Float    Height;
		Float    LayoutWidth;
		Float    LayoutHeight;
		//uint32 maxBidiReorderingDepth;
		uint32   LineCount;
	};

	class TextFormat
		: public DirectWriteResource<::IDWriteTextFormat>
	{
	public:
		using DirectWriteResource<::IDWriteTextFormat>::DirectWriteResource;

		Float GetFontSize() const
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

		Bool GetWordWrapping() const
		{
			return GetResource()->GetWordWrapping() == ::DWRITE_WORD_WRAPPING_WRAP;
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

		// @throw AL::Exception
		auto GetLineSpacing() const
		{
			LineSpacingInfo info;

			if (FAILED(GetResource()->GetLineSpacing(reinterpret_cast<::DWRITE_LINE_SPACING_METHOD*>(&info.Method), &info.LineSpacing, &info.BaseLine)))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::GetLineSpacing"
				);
			}

			return info;
		}

		// @throw AL::Exception
		Void SetWordWrapping(Bool wrap = True)
		{
			auto wordWrapping = wrap ? ::DWRITE_WORD_WRAPPING_WRAP : ::DWRITE_WORD_WRAPPING_NO_WRAP;

			if (FAILED(GetResource()->SetWordWrapping(wordWrapping)))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::SetWordWrapping"
				);
			}
		}

		// @throw AL::Exception
		Void SetTextAlignment(TextAlignment textAlignment)
		{
			if (FAILED(GetResource()->SetTextAlignment(static_cast<::DWRITE_TEXT_ALIGNMENT>(textAlignment))))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::SetTextAlignment"
				);
			}
		}

		// @throw AL::Exception
		Void SetReadingDirection(ReadingDirection readingDirection)
		{
			if (FAILED(GetResource()->SetReadingDirection(static_cast<::DWRITE_READING_DIRECTION>(readingDirection))))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::SetReadingDirection"
				);
			}
		}

		// @throw AL::Exception
		Void SetParagraphAlignment(ParagraphAlignment paragraphAlignment)
		{
			if (FAILED(GetResource()->SetParagraphAlignment(static_cast<::DWRITE_PARAGRAPH_ALIGNMENT>(paragraphAlignment))))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::SetParagraphAlignment"
				);
			}
		}

		// @throw AL::Exception
		Void SetLineSpacing(LineSpacingMethod method, Float lineSpacing, Float baseLine)
		{
			if (FAILED(GetResource()->SetLineSpacing(static_cast<::DWRITE_LINE_SPACING_METHOD>(method), lineSpacing, baseLine)))
			{

				throw Exception(
					"Error calling IDWriteTextFormat::SetLineSpacing"
				);
			}
		}
	};

	class Direct2D;

	class TextLayout
		: public DirectWriteResource<::IDWriteTextLayout>
	{
		Bool clipBounds = False;

	public:
		TextLayout()
			: TextLayout(
				nullptr
			)
		{
		}

		TextLayout(TextLayout&& layout)
			: DirectWriteResource<::IDWriteTextLayout>(
				Move(layout)
			)
		{
			clipBounds = layout.clipBounds;
			layout.clipBounds = False;
		}

		explicit TextLayout(Type* lpResource)
			: DirectWriteResource<::IDWriteTextLayout>(
				lpResource
			)
		{
		}

		Bool GetClipping() const
		{
			return clipBounds;
		}

		Float GetMinWidth() const
		{
			Float minWidth = 0;

			if (SUCCEEDED(GetResource()->DetermineMinWidth(&minWidth)))
			{

				return minWidth;
			}

			return GetResource()->GetMaxWidth();
		}

		Bool GetWordWrapping() const
		{
			return GetResource()->GetWordWrapping() == ::DWRITE_WORD_WRAPPING_WRAP;
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

		// @throw AL::Exception
		auto GetMetrics() const
		{
			::DWRITE_TEXT_METRICS metrics;

			if (FAILED(GetResource()->GetMetrics(&metrics)))
			{

				throw Exception(
					"Error calling IDWriteTextLayout::GetMetrics"
				);
			}

			TextMetrics textMetrics;
			textMetrics.Left                             = metrics.left;
			textMetrics.Top                              = metrics.top;
			textMetrics.Width                            = metrics.width;
			textMetrics.WidthIncludingTrailingWhitespace = metrics.widthIncludingTrailingWhitespace;
			textMetrics.Height                           = metrics.height;
			textMetrics.LayoutWidth                      = metrics.layoutWidth;
			textMetrics.LayoutHeight                     = metrics.layoutHeight;
			textMetrics.LineCount                        = metrics.lineCount;

			return textMetrics;
		}

		// @throw AL::Exception
		static auto GetMetrics(Direct2D& direct2D, const TextFormat& format, const String& string);
		// @throw AL::Exception
		static auto GetMetrics(Direct2D& direct2D, const TextFormat& format, const WString& string);

		Void SetClipping(Bool set = True)
		{
			clipBounds = set;
		}

		// @throw AL::Exception
		Void SetWordWrapping(Bool wrap = True)
		{
			auto wordWrapping = wrap ? ::DWRITE_WORD_WRAPPING_WRAP : ::DWRITE_WORD_WRAPPING_NO_WRAP;

			if (FAILED(GetResource()->SetWordWrapping(wordWrapping)))
			{

				throw Exception(
					"Error calling IDWriteTextLayout::SetWordWrapping"
				);
			}
		}

		// @throw AL::Exception
		Void SetTextAlignment(TextAlignment alignment)
		{
			if (FAILED(GetResource()->SetTextAlignment(static_cast<::DWRITE_TEXT_ALIGNMENT>(alignment))))
			{

				throw Exception(
					"Error calling IDWriteTextLayout::SetTextAlignment"
				);
			}
		}

		// @throw AL::Exception
		Void SetParagraphAlignment(ParagraphAlignment alignment)
		{
			if (FAILED(GetResource()->SetParagraphAlignment(static_cast<::DWRITE_PARAGRAPH_ALIGNMENT>(alignment))))
			{

				throw Exception(
					"Error calling IDWriteTextLayout::SetParagraphAlignment"
				);
			}
		}

		auto& operator = (Type* resource)
		{
			DirectWriteResource<::IDWriteTextLayout>::operator=(
				resource
			);

			return *this;
		}

		auto& operator = (TextLayout&& layout)
		{
			DirectWriteResource<::IDWriteTextLayout>::operator=(
				Move(layout)
			);

			clipBounds = layout.clipBounds;
			layout.clipBounds = False;

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

		Factory    factory;
		DWFactory  dwFactory;
		WICFactory wicFactory;

		Target     target;
		HWNDTarget hwndTarget;

		Target     sharedTarget;

		Collections::LinkedList<Transform> transforms;

	public:
		Direct2D()
		{
			PushTransform(
				Transform::Identity()
			);
		}

		Bool IsCreated() const
		{
			return factory.IsCreated();
		}

		Bool IsTargetCreated() const
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
			return *transforms.begin();
		}

		Void PushTransform(Transform&& transform)
		{
			transforms.PushFront(
				Move(transform)
			);

			if (IsTargetCreated())
			{
				GetTarget()->SetTransform(
					*transforms.begin()
				);
			}
		}
		Void PushTransform(const Transform& transform)
		{
			transforms.PushFront(
				transform
			);

			if (IsTargetCreated())
			{
				GetTarget()->SetTransform(
					*transforms.begin()
				);
			}
		}

		Bool PopTransform()
		{
			if (transforms.GetSize() >= 2)
			{
				transforms.PopFront();

				GetTarget()->SetTransform(
					*transforms.begin()
				);

				return True;
			}

			return False;
		}
		Bool PopTransform(Transform& transform)
		{
			if (transforms.GetSize() >= 2)
			{
				transform = Move(
					*transforms.begin()
				);

				transforms.PopFront();

				GetTarget()->SetTransform(
					*transforms.begin()
				);

				return True;
			}

			return False;
		}

		// @throw AL::Exception
		Void Create(Bool multithreaded = False)
		{
			AL_ASSERT(
				!IsCreated(),
				"Direct2D already created"
			);

			::D2D1_FACTORY_OPTIONS options;
			ZeroMemory(&options, sizeof(options));

#if defined(AL_DEBUG)
			BitMask<::D2D1_DEBUG_LEVEL> debugLevel;
			debugLevel.Add(::D2D1_DEBUG_LEVEL_ERROR);
			debugLevel.Add(::D2D1_DEBUG_LEVEL_WARNING);
			debugLevel.Add(::D2D1_DEBUG_LEVEL_INFORMATION);

			options.debugLevel = debugLevel.Value;
#endif

			typename Factory::Type* lpFactory;

			if (FAILED(::D2D1CreateFactory(multithreaded ? ::D2D1_FACTORY_TYPE_MULTI_THREADED : ::D2D1_FACTORY_TYPE_SINGLE_THREADED, &lpFactory)))
			{

				throw Exception(
					"Error calling D2D1CreateFactory"
				);
			}

			typename DWFactory::Type* lpDWFactory;

			if (FAILED(::DWriteCreateFactory(::DWRITE_FACTORY_TYPE_SHARED, __uuidof(typename DWFactory::Type), reinterpret_cast<::IUnknown**>(&lpDWFactory))))
			{
				lpFactory->Release();

				throw Exception(
					"Error calling DWriteCreateFactory"
				);
			}

			if (FAILED(::CoInitialize(nullptr)))
			{
				lpDWFactory->Release();

				lpFactory->Release();

				throw Exception(
					"Error calling CoInitialize"
				);
			}

			typename WICFactory::Type* lpWICFactory;

			if (FAILED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, ::CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&lpWICFactory))))
			{
				CoUninitialize();

				lpDWFactory->Release();

				lpFactory->Release();

				throw Exception(
					"Error calling CoCreateInstance"
				);
			}

			factory = lpFactory;
			dwFactory = lpDWFactory;
			wicFactory = lpWICFactory;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				DestroyTarget();

				wicFactory.Release();
				::CoUninitialize();

				dwFactory.Release();

				factory.Release();
			}
		}

		// @throw AL::Exception
		Void CreateTarget(HWND hWnd)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				!IsTargetCreated(),
				"Direct2D target already created"
			);

			::FLOAT dpiX, dpiY;

			GetFactory()->GetDesktopDpi(
				&dpiX,
				&dpiY
			);

			auto properties = ::D2D1::RenderTargetProperties(
				::D2D1_RENDER_TARGET_TYPE_HARDWARE,
				D2D1::PixelFormat(
					::DXGI_FORMAT_UNKNOWN,
					::D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				dpiX,
				dpiY
			);

			auto hwndProperties = ::D2D1::HwndRenderTargetProperties(
				hWnd
			);

			typename HWNDTarget::Type* lpTarget;

			if (FAILED(GetFactory()->CreateHwndRenderTarget(properties, hwndProperties, &lpTarget)))
			{

				throw Exception(
					"Error creating HWND render target"
				);
			}

			hwndTarget = lpTarget;

			lpTarget->AddRef();
			sharedTarget = lpTarget;
		}
		// @throw AL::Exception
		Void CreateTarget(IDXGISwapChain* lpSwapChain)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				!IsTargetCreated(),
				"Direct2D target already created"
			);

			::IDXGISurface* lpSurface;

			if (FAILED(lpSwapChain->GetBuffer(0, IID_PPV_ARGS(&lpSurface))))
			{

				throw Exception(
					"Error getting IDXGISurface*"
				);
			}

			::FLOAT dpiX, dpiY;

			GetFactory()->GetDesktopDpi(
				&dpiX,
				&dpiY
			);

			auto properties = ::D2D1::RenderTargetProperties(
				::D2D1_RENDER_TARGET_TYPE_DEFAULT,
				::D2D1::PixelFormat(
					::DXGI_FORMAT_UNKNOWN,
					::D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				dpiX,
				dpiY
			);

			::HRESULT hResult;
			typename Target::Type* lpTarget;

			if (FAILED(hResult = GetFactory()->CreateDxgiSurfaceRenderTarget(lpSurface, properties, &lpTarget)))
			{
				lpSurface->Release();

				throw Exception(
					"Error creating DXGI render target"
				);
			}

			lpSurface->Release();

			target = lpTarget;

			lpTarget->AddRef();
			sharedTarget = lpTarget;
		}

		Void DestroyTarget()
		{
			if (IsTargetCreated())
			{
				target.Release();
				hwndTarget.Release();
				sharedTarget.Release();
			}
		}

		// @throw AL::Exception
		Void CreateLayer(Layer& layer)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename Layer::Type* lpLayer;

			if (FAILED(GetTarget()->CreateLayer(&lpLayer)))
			{

				throw Exception(
					"Error creating ClipLayer"
				);
			}

			layer = lpLayer;
		}

		// @throw AL::Exception 
		Void CreateSolidColorBrush(SolidColorBrush& brush, Drawing::Color color)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename SolidColorBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateSolidColorBrush(
				::D2D1::ColorF(
					color.R / 255.0f,
					color.G / 255.0f,
					color.B / 255.0f,
					color.A / 255.0f
				),
				&lpBrush
			)))
			{

				throw Exception(
					"Error creating SolidColorBrush"
				);
			}

			brush = lpBrush;
		}
		// @throw AL::Exception
		Void CreateBitmapBrush(BitmapBrush& brush, const Bitmap& bitmap, Float opacity = 1.0f, const Transform& transform = Transform::Identity())
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename BitmapBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateBitmapBrush(
				bitmap,
				::D2D1::BitmapBrushProperties(),
				::D2D1::BrushProperties(
					opacity,
					transform
				),
				&lpBrush
			)))
			{

				throw Exception(
					"Error creating BitmapBrush"
				);
			}

			brush = lpBrush;
		}
		// @throw AL::Exception
		template<size_t S>
		Void CreateLinearGradientBrush(LinearGradientBrush& brush, const Drawing::Vector2F& start, const Drawing::Vector2F& end, const GradientStop(&stops)[S])
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			Collections::Array<::D2D1_GRADIENT_STOP> _stops(
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

			::ID2D1GradientStopCollection* lpStopCollection;

			if (FAILED(GetTarget()->CreateGradientStopCollection(&_stops[0], static_cast<::UINT>(_stops.GetSize()), &lpStopCollection)))
			{

				throw Exception(
					"Error creating GradientStopCollection"
				);
			}

			typename LinearGradientBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateLinearGradientBrush(
				::D2D1::LinearGradientBrushProperties(
					::D2D1::Point2F(
						start.X,
						start.Y
					),
					::D2D1::Point2F(
						end.X,
						end.Y
					)
				),
				lpStopCollection,
				&lpBrush
			)))
			{
				lpStopCollection->Release();

				throw Exception(
					"Error creating LinearGradientBrush"
				);
			}

			lpStopCollection->Release();

			brush = lpBrush;
		}
		// @throw AL::Exception
		template<size_t S>
		Void CreateRadialGradientBrush(RadialGradientBrush& brush, const Drawing::Vector2F& center, const Drawing::Vector2F& offset, const Drawing::Vector2F& radius, const GradientStop(&stops)[S])
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			Collections::Array<::D2D1_GRADIENT_STOP> _stops(
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

			::ID2D1GradientStopCollection* lpStopCollection;

			if (FAILED(GetTarget()->CreateGradientStopCollection(&_stops[0], static_cast<::UINT>(_stops.GetSize()), &lpStopCollection)))
			{
				throw Exception(
					"Error creating GradientStopCollection"
				);
			}

			RadialGradientBrush::Type* lpBrush;

			if (FAILED(GetTarget()->CreateRadialGradientBrush(
				::D2D1::RadialGradientBrushProperties(
					::D2D1::Point2F(
						center.X,
						center.Y
					),
					::D2D1::Point2F(
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

				throw Exception(
					"Error creating RadialGradientBrush"
				);
			}

			lpStopCollection->Release();

			brush = lpBrush;
		}

		// @throw AL::Exception
		Void CreateBitmap(Bitmap& bitmap, const FileSystem::Path& path, Bool color = True)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			::IWICBitmapDecoder* lpWICDecoder;

			if (FAILED(GetWICFactory()->CreateDecoderFromFilename(path.GetString().ToWString().GetCString(), nullptr, GENERIC_READ, ::WICDecodeMetadataCacheOnLoad, &lpWICDecoder)))
			{

				throw Exception(
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
			catch (Exception& exception)
			{
				lpWICDecoder->Release();

				throw Exception(
					Move(exception),
					"Error creating bitmap from decoder"
				);
			}

			lpWICDecoder->Release();
		}
		// @throw AL::Exception
		Void CreateBitmap(Bitmap& bitmap, const Void* lpBuffer, size_t bufferSize, Bool color = True)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			::IWICStream* lpStream;

			if (FAILED(GetWICFactory()->CreateStream(&lpStream)))
			{

				throw Exception(
					"Error creating IWICStream"
				);
			}

			if (FAILED(lpStream->InitializeFromMemory(reinterpret_cast<::BYTE*>(const_cast<Void*>(lpBuffer)), static_cast<::DWORD>(bufferSize))))
			{
				lpStream->Release();

				throw Exception(
					"Error initializing Bitmap buffer from memory"
				);
			}

			::IWICBitmapDecoder* lpWICDecoder;

			if (FAILED(GetWICFactory()->CreateDecoderFromStream(lpStream, nullptr, ::WICDecodeMetadataCacheOnLoad, &lpWICDecoder)))
			{
				lpStream->Release();

				throw Exception(
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
			catch (Exception& exception)
			{
				lpWICDecoder->Release();

				throw Exception(
					Move(exception),
					"Error creating bitmap from decoder"
				);
			}

			lpWICDecoder->Release();
		}

		// @throw AL::Exception
		Void CreateTextFormat(TextFormat& textFormat, const String& fontName, Float fontSize)
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
		// @throw AL::Exception
		Void CreateTextFormat(TextFormat& textFormat, const String& fontName, Float fontSize, FontWeight fontWeight, FontStyle fontStyle, FontStretch fontStretch)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename TextFormat::Type* lpFormat;

			if (FAILED(GetDWFactory()->CreateTextFormat(fontName.ToWString().GetCString(), nullptr, static_cast<::DWRITE_FONT_WEIGHT>(fontWeight), static_cast<::DWRITE_FONT_STYLE>(fontStyle), static_cast<::DWRITE_FONT_STRETCH>(fontStretch), fontSize, L"", &lpFormat)))
			{

				throw Exception(
					"Error creating TextFormat"
				);
			}

			textFormat = lpFormat;
		}

		// @throw AL::Exception
		Void CreateTextLayout(TextLayout& textLayout, const TextFormat& textFormat, const String& text, Float width, Float height)
		{
			CreateTextLayout(
				textLayout,
				textFormat,
				text.ToWString(),
				width,
				height
			);
		}
		// @throw AL::Exception
		Void CreateTextLayout(TextLayout& textLayout, const TextFormat& textFormat, const WString& text, Float width, Float height)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename TextLayout::Type* lpLayout;

			if (FAILED(GetDWFactory()->CreateTextLayout(text.GetCString(), static_cast<::UINT32>(text.GetLength()), textFormat, width, height, &lpLayout)))
			{

				throw Exception(
					"Error creating TextLayout"
				);
			}

			textLayout = lpLayout;
		}

		// @throw AL::Exception
		template<typename T>
		Void CreateTransformedGeometry(TransformedGeometry& geometry, const T& sourceGeometry, const Transform& transform)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename TransformedGeometry::Type* lpGeomtry;

			if (FAILED(GetFactory()->CreateTransformedGeometry(sourceGeometry, transform, &lpGeomtry)))
			{

				throw Exception(
					"Error creating TransformedGeometry"
				);
			}

			geometry = lpGeomtry;
		}

		// @throw AL::Exception
		Void CreatePathGeometry(PathGeometry& geometry)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename PathGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreatePathGeometry(&lpGeometry)))
			{

				throw Exception(
					"Error creating PathGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exception
		Void CreateEllipseGeometry(EllipseGeometry& geometry, const Drawing::Vector2F& center, Float radiusX, Float radiusY)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename EllipseGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateEllipseGeometry(
				::D2D1::Ellipse(
					::D2D1::Point2F(
						center.X, center.Y
					),
					radiusX,
					radiusY
				),
				&lpGeometry
			)))
			{
				throw Exception(
					"Error creating EllipseGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exception
		Void CreateRectangleGeometry(RectangleGeometry& geometry, const Drawing::RectangleF& rectangle)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			typename RectangleGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateRectangleGeometry(
				::D2D1::RectF(
					rectangle.Left,
					rectangle.Top,
					rectangle.Right,
					rectangle.Bottom
				),
				&lpGeometry
			)))
			{

				throw Exception(
					"Error creating RectangleGeometry"
				);
			}

			geometry = lpGeometry;
		}
		// @throw AL::Exception
		Void CreateRoundedRectangleGeometry(RoundedRectangleGeometry& geometry, const Drawing::RectangleF& rectangle, Float radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			RoundedRectangleGeometry::Type* lpGeometry;

			if (FAILED(GetFactory()->CreateRoundedRectangleGeometry(
				::D2D1::RoundedRect(
					::D2D1::RectF(
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
				throw Exception(
					"Error creating RoundedRectangleGeometry"
				);
			}

			geometry = lpGeometry;
		}

		Void PushLayer(const Layer& layer)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->PushLayer(
				layer.GetParams(),
				layer
			);
		}

		Void PopLayer()
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->PopLayer();
		}

		Void PushClip(const Drawing::RectangleF& rectangle, Bool antialias = False)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->PushAxisAlignedClip(
				::D2D1::RectF(
					rectangle.Left,
					rectangle.Top,
					rectangle.Right,
					rectangle.Bottom
				),
				antialias ? ::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : ::D2D1_ANTIALIAS_MODE_ALIASED
			);
		}

		Void PopClip()
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->PopAxisAlignedClip();
		}

		template<typename T>
		Void FillCircle(const Drawing::Vector2F& position, const T& brush, Float radius)
		{
			FillEllipse(
				position,
				brush,
				radius,
				radius
			);
		}

		template<typename T>
		Void FillEllipse(const Drawing::Vector2F& position, const T& brush, Float radiusX, Float radiusY)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->FillEllipse(
				::D2D1::Ellipse(
					::D2D1::Point2F(
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
		Void FillRectangle(const Drawing::Vector2F& position, const T& brush, Float width, Float height)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->FillRectangle(
				::D2D1::RectF(
					position.X,
					position.Y,
					position.X + width,
					position.Y + height
				),
				brush
			);
		}

		template<typename T>
		Void FillRoundedRectangle(const Drawing::Vector2F& position, const T& brush, Float width, Float height, Float radius)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->FillRoundedRectangle(
				::D2D1::RoundedRect(
					::D2D1::RectF(
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

		template<typename T>
		Void DrawLine(const Drawing::Vector2F& start, const Drawing::Vector2F& end, const T& brush, Float thickness = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawLine(
				::D2D1::Point2F(
					start.X,
					start.Y
				),
				::D2D1::Point2F(
					end.X,
					end.Y
				),
				brush,
				thickness
			);
		}

		template<typename T>
		Void DrawCircle(const Drawing::Vector2F& position, const T& brush, Float radius, Float strokeWidth = 1.0f)
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
		Void DrawEllipse(const Drawing::Vector2F& position, const T& brush, Float radiusX, Float radiusY, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawEllipse(
				::D2D1::Ellipse(
					::D2D1::Point2F(
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
		Void DrawRectangle(const Drawing::Vector2F& position, const T& brush, Float width, Float height, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawRectangle(
				::D2D1::RectF(
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
		Void DrawRoundedRectangle(const Drawing::Vector2F& position, const T& brush, Float width, Float height, Float radius, Float strokeWidth = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawRoundedRectangle(
				::D2D1::RoundedRect(
					::D2D1::RectF(
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

		Void DrawBitmap(const Drawing::Vector2F& position, const Bitmap& bitmap, Float opacity = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			auto size = bitmap.GetSize();

			DrawBitmap(
				position,
				bitmap,
				size.Width,
				size.Height,
				opacity
			);
		}
		Void DrawBitmap(const Drawing::Vector2F& position, const Bitmap& bitmap, Float width, Float height, Float opacity = 1.0f)
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
		Void DrawBitmap(const Drawing::Vector2F& position, const Bitmap& bitmap, Float width, Float height, Float sourceX, Float sourceY, Float opacity = 1.0f)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			auto size = bitmap.GetSize();

			auto src = ::D2D1::RectF(
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
				::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				src
			);
		}

		template<typename T>
		Void DrawString(const Drawing::Vector2F& position, const T& brush, const TextFormat& textFormat, const String& text)
		{
			DrawString(
				position,
				brush,
				textFormat,
				text.ToWString()
			);
		}
		template<typename T>
		Void DrawString(const Drawing::Vector2F& position, const T& brush, const TextFormat& textFormat, const WString& text)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawTextA(
				text.GetCString(),
				static_cast<::UINT>(text.GetLength()),
				textFormat,
				::D2D1::RectF(
					position.X,
					position.Y,
					position.X + (text.GetLength() * textFormat.GetFontSize()),
					position.Y + textFormat.GetFontSize()
				),
				brush
			);
		}

		template<typename T>
		Void DrawTextLayout(const Drawing::Vector2F& position, const T& brush, const TextLayout& textLayout)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct2D target not created"
			);

			GetTarget()->DrawTextLayout(
				::D2D1::Point2F(
					position.X,
					position.Y
				),
				textLayout,
				brush,
				textLayout.GetClipping() ? ::D2D1_DRAW_TEXT_OPTIONS_CLIP : ::D2D1_DRAW_TEXT_OPTIONS_NONE
			);
		}

		Void BeginDraw()
		{
			auto lpTarget = &(*GetTarget());

			AL_ASSERT(
				lpTarget != nullptr,
				"Direct2D target not created"
			);

			lpTarget->BeginDraw();
		}

		Void Clear(Drawing::Color color)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);

			auto lpTarget = &(*GetTarget());

			AL_ASSERT(
				lpTarget != nullptr,
				"Direct2D target not created"
			);

			::D2D1_COLOR_F colorf;
			colorf.a = color.A * 255.0f;
			colorf.r = color.R * 255.0f;
			colorf.g = color.G * 255.0f;
			colorf.b = color.B * 255.0f;

			lpTarget->Clear(
				colorf
			);
		}

		// @throw AL::Exception
		// @return AL::False on D2DERR_RECREATE_TARGET
		Bool EndDraw()
		{
			AL_ASSERT(
				IsCreated(),
				"Direct2D not created"
			);
			
			auto lpTarget = &(*GetTarget());

			AL_ASSERT(
				lpTarget != nullptr,
				"Direct2D target not created"
			);

			::HRESULT hResult;

			if (FAILED(hResult = lpTarget->EndDraw()))
			{
				if (hResult == static_cast<::HRESULT>(D2DERR_RECREATE_TARGET))
				{

					return False;
				}

				throw Exception(
					"Error caling %s::EndDraw",
					Type<decltype(*lpTarget)>::Name
				);
			}

			return True;
		}

	private:
		// @throw AL::Exception
		Void CreateBitmapFromDecoder(Bitmap& bitmap, ::IWICBitmapDecoder* wicDecoder, Bool color)
		{
			::IWICBitmapFrameDecode* lpWICFrame;

			if (FAILED(wicDecoder->GetFrame(0, &lpWICFrame)))
			{

				throw Exception(
					"Error creating IWICBitmapFrameDecode"
				);
			}

			::IWICFormatConverter* lpWICConverter;

			if (FAILED(GetWICFactory()->CreateFormatConverter(&lpWICConverter)))
			{
				lpWICFrame->Release();

				throw Exception(
					"Error creating IWICFormatConverter"
				);
			}

			if (FAILED(lpWICConverter->Initialize(
				lpWICFrame,
				GUID_WICPixelFormat32bppPBGRA,
				::WICBitmapDitherTypeNone,
				nullptr,
				0.0, // alpha
				color ? ::WICBitmapPaletteTypeMedianCut : ::WICBitmapPaletteTypeFixedBW)))
			{
				lpWICFrame->Release();

				lpWICConverter->Release();

				throw Exception(
					"Error initializing IWICFormatConverter"
				);
			}

			typename Bitmap::Type* lpBitmap;

			if (FAILED(GetTarget()->CreateBitmapFromWicBitmap(lpWICConverter, nullptr, &lpBitmap)))
			{
				lpWICFrame->Release();

				lpWICConverter->Release();

				throw Exception(
					"Error creating ID2D1Bitmap"
				);
			}

			lpWICFrame->Release();
			lpWICConverter->Release();

			bitmap = lpBitmap;
		}
	};
}
