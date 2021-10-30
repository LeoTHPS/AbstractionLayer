#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "Resource.hpp"

#include "AL/Drawing/Size.hpp"
#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"
#include "AL/Drawing/Vector.hpp"
#include "AL/Drawing/Rectangle.hpp"

#if __has_include(<d3d11_4.h>)
	#include <d3d11_4.h>

	#define AL_DEPENDENCY_DIRECT3D_11_4

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.4
#elif __has_include(<d3d11_3.h>)
	#include <d3d11_3.h>

	#define AL_DEPENDENCY_DIRECT3D_11_3

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.3
#elif __has_include(<d3d11_2.h>)
	#include <d3d11_2.h>

	#define AL_DEPENDENCY_DIRECT3D_11_2

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.2
#elif __has_include(<d3d11_1.h>)
	#include <d3d11_1.h>

	#define AL_DEPENDENCY_DIRECT3D_11_1

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.1
#elif __has_include(<d3d11.h>)
	#include <d3d11.h>

	#define AL_DEPENDENCY_DIRECT3D_11

	#define AL_DEPENDENCY_DIRECT3D_11_VERSION 11.0
#endif

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "dxguid.lib")
#endif

namespace AL::OS::Windows::DirectX
{
	enum class AntiAliasing
	{
		None     = 0,
		Center   = D3D11_CENTER_MULTISAMPLE_PATTERN,
		Standard = D3D11_STANDARD_MULTISAMPLE_PATTERN
	};

	// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro
	enum class FeatureLevels
	{
		DX9_1  = D3D_FEATURE_LEVEL_9_1,
		DX9_2  = D3D_FEATURE_LEVEL_9_2,
		DX9_3  = D3D_FEATURE_LEVEL_9_3,

		DX10   = D3D_FEATURE_LEVEL_10_0,
		DX10_1 = D3D_FEATURE_LEVEL_10_1,

		DX11   = D3D_FEATURE_LEVEL_11_0,
		DX11_1 = D3D_FEATURE_LEVEL_11_1,

		DX12   = D3D_FEATURE_LEVEL_12_0,
		DX12_1 = D3D_FEATURE_LEVEL_12_1
	};

	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct3D11Resource = Resource<T, TReleaser>;

	class Direct3D
	{
		typedef Direct3D11Resource<ID3D11Device> Device;
		typedef Direct3D11Resource<ID3D11DeviceContext> DeviceContext;

		typedef Direct3D11Resource<ID3D11RenderTargetView> TargetView;

		typedef Direct3D11Resource<IDXGISwapChain> SwapChain;

		Device device;
		SwapChain swapChain;
		DeviceContext deviceContext;

		TargetView targetView;

		Direct3D(Direct3D&&) = delete;
		Direct3D(const Direct3D&) = delete;

	public:
		Direct3D()
		{
		}

		virtual ~Direct3D()
		{
			if (IsCreated())
			{
				Destroy();
			}
		}

		Bool IsCreated() const
		{
			return device.IsCreated();
		}

		Bool IsTargetCreated() const
		{
			return targetView.IsCreated();
		}

		auto& GetDevice() const
		{
			return device;
		}

		auto& GetDeviceContext() const
		{
			return deviceContext;
		}

		auto& GetSwapChain() const
		{
			return swapChain;
		}

		auto& GetTargetView() const
		{
			return targetView;
		}

		// @throw AL::Exception
		Void SetClientSize(uint32 width, uint32 height)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				!IsTargetCreated(),
				"Direct3D target already created"
			);

			if (FAILED(GetSwapChain()->ResizeBuffers(0, static_cast<::UINT>(width), static_cast<::UINT>(height), ::DXGI_FORMAT_UNKNOWN, 0)))
			{

				throw Exception(
					"Error resizing swap chain buffer(s)"
				);
			}
		}

		// @throw AL::Exception
		Void Create(::HWND hWnd, AntiAliasing antiAliasing = AntiAliasing::None, uint32 msaaLevel = 1, Bool multithreaded = False, FeatureLevels featureLevel = FeatureLevels::DX11)
		{
			AL_ASSERT(
				!IsCreated(),
				"Direct3D already created"
			);

			BitMask<::UINT> flags;
			flags.Add(::D3D11_CREATE_DEVICE_BGRA_SUPPORT);
			flags.Set(::D3D11_CREATE_DEVICE_SINGLETHREADED, !multithreaded);

#if defined(AL_DEBUG)
			flags.Add(::D3D11_CREATE_DEVICE_DEBUG);
			//flags.Add(D3D11_CREATE_DEVICE_DEBUGGABLE);
#endif

			::D3D_FEATURE_LEVEL featureLevels[1] =
			{
				static_cast<::D3D_FEATURE_LEVEL>(featureLevel)
			};

			msaaLevel = Math::Clamp<uint32>(
				msaaLevel,
				1,
				D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT
			);

			::DXGI_SWAP_CHAIN_DESC description;
			ZeroMemory(&description, sizeof(::DXGI_SWAP_CHAIN_DESC));

			description.BufferDesc.Width = 0;
			description.BufferDesc.Height = 0;
			description.BufferDesc.Format = ::DXGI_FORMAT_B8G8R8A8_UNORM;
			description.BufferDesc.Scaling = ::DXGI_MODE_SCALING_UNSPECIFIED;
			description.BufferDesc.ScanlineOrdering = ::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			description.BufferDesc.RefreshRate.Numerator = 0;
			description.BufferDesc.RefreshRate.Denominator = 1;

			description.SampleDesc.Count = static_cast<::UINT>(msaaLevel);
			description.SampleDesc.Quality = static_cast<::UINT>(antiAliasing);

			//description.Flags = ::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			description.Windowed = TRUE;
			description.BufferCount = 1;
			description.SwapEffect = ::DXGI_SWAP_EFFECT_DISCARD;
			description.OutputWindow = hWnd;
			description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

			typename Device::Type* lpDevice;
			typename SwapChain::Type* lpSwapChain;
			typename DeviceContext::Type* lpDeviceContext;

			if (FAILED(D3D11CreateDeviceAndSwapChain(
				nullptr, // default adapter
				::D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				flags.Value,
				&featureLevels[0],
				sizeof(featureLevels) / sizeof(::D3D_FEATURE_LEVEL),
				D3D11_SDK_VERSION,
				&description,
				&lpSwapChain,
				&lpDevice,
				nullptr,
				&lpDeviceContext
			)))
			{

				throw Exception(
					"Error calling D3D11CreateDeviceAndSwapChain"
				);
			}

			::IDXGIDevice* lpDXGIDevice;

			if (FAILED(lpDevice->QueryInterface(&lpDXGIDevice)))
			{
				lpDevice->Release();
				lpSwapChain->Release();
				lpDeviceContext->Release();

				throw Exception(
					"Error getting IDXGIDevice*"
				);
			}

			::IDXGIAdapter* lpDXGIAdapter;

			if (FAILED(lpDXGIDevice->GetAdapter(&lpDXGIAdapter)))
			{
				lpDXGIDevice->Release();

				lpDevice->Release();
				lpSwapChain->Release();
				lpDeviceContext->Release();

				throw Exception(
					"Error getting IDXGIAdapter*"
				);
			}

			::IDXGIFactory* lpDXGIFactory;

			if (FAILED(lpDXGIAdapter->GetParent(IID_PPV_ARGS(&lpDXGIFactory))))
			{
				lpDXGIAdapter->Release();
				lpDXGIDevice->Release();

				lpDevice->Release();
				lpSwapChain->Release();
				lpDeviceContext->Release();

				throw Exception(
					"Error getting IDXGIFactory*"
				);
			}

			if (FAILED(lpDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES)))
			{
				lpDXGIFactory->Release();
				lpDXGIAdapter->Release();
				lpDXGIDevice->Release();

				lpDevice->Release();
				lpSwapChain->Release();
				lpDeviceContext->Release();

				throw Exception(
					"Error setting DXGI_MWA_NO_WINDOW_CHANGES"
				);
			}

			lpDXGIFactory->Release();
			lpDXGIAdapter->Release();
			lpDXGIDevice->Release();

			device = lpDevice;
			swapChain = lpSwapChain;
			deviceContext = lpDeviceContext;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				DestroyTarget();

				deviceContext.Release();
				swapChain.Release();
				device.Release();
			}
		}

		// @throw AL::Exception
		Void CreateTarget()
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				!IsTargetCreated(),
				"Direct3D target already created"
			);

			::ID3D11Texture2D* lpTexture;

			if (FAILED(GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&lpTexture))))
			{

				throw Exception(
					"Error getting ID3D11Texture2D*"
				);
			}

			typename TargetView::Type* lpTargetView;

			if (FAILED(GetDevice()->CreateRenderTargetView(lpTexture, nullptr, &lpTargetView)))
			{
				lpTexture->Release();

				throw Exception(
					"Error creating TargetView"
				);
			}

			lpTexture->Release();

			GetDeviceContext()->OMSetRenderTargets(
				1,
				&lpTargetView,
				nullptr
			);

			targetView = lpTargetView;
		}

		Void DestroyTarget()
		{
			if (IsTargetCreated())
			{
				GetDeviceContext()->OMSetRenderTargets(
					0,
					nullptr,
					nullptr
				);

				targetView.Release();
			}
		}

		Void Clear(Drawing::Color color)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct3D target not created"
			);

			::FLOAT rgba[4];
			rgba[0] = color.R / 255.0f;
			rgba[1] = color.G / 255.0f;
			rgba[2] = color.B / 255.0f;
			rgba[3] = color.A / 255.0f;

			GetDeviceContext()->ClearRenderTargetView(
				GetTargetView(),
				rgba
			);
		}

		// @throw AL::Exception
		Void Present(Bool vsync = False)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct3D target not created"
			);

			auto hResult = GetSwapChain()->Present(
				vsync ? 1 : 0,
				0
			);

			if (FAILED(hResult) && (hResult != DXGI_ERROR_WAS_STILL_DRAWING))
			{

				throw Exception(
					"Error presenting swap chain"
				);
			}
		}
	};
}
