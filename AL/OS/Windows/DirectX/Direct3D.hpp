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

#if AL_HAS_INCLUDE(<d3d11_4.h>)
	#include <d3d11_4.h>
#elif AL_HAS_INCLUDE(<d3d11_3.h>)
	#include <d3d11_3.h>
#elif AL_HAS_INCLUDE(<d3d11_2.h>)
	#include <d3d11_2.h>
#elif AL_HAS_INCLUDE(<d3d11_1.h>)
	#include <d3d11_1.h>
#elif AL_HAS_INCLUDE(<d3d11.h>)
	#include <d3d11.h>
#else
	#error Missing d3d11.h
#endif

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "dxguid.lib")
#endif

namespace AL::OS::Windows::DirectX
{
	enum class AntiAliasing : uint8
	{
		None,

		// D3D11_CENTER_MULTISAMPLE_PATTERN
		Center,

		// D3D11_STANDARD_MULTISAMPLE_PATTERN
		Standard
	};

	// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro
	enum class FeatureLevels : uint8
	{
		// D3D_FEATURE_LEVEL_9_1
		DX9_1,
		// D3D_FEATURE_LEVEL_9_2
		DX9_2,
		// D3D_FEATURE_LEVEL_9_3
		DX9_3,

		// D3D_FEATURE_LEVEL_10_0
		DX10,
		// D3D_FEATURE_LEVEL_10_1
		DX10_1,

		// D3D_FEATURE_LEVEL_11_0
		DX11,
		// D3D_FEATURE_LEVEL_11_1
		DX11_1,

		// D3D_FEATURE_LEVEL_12_0
		DX12,
		// D3D_FEATURE_LEVEL_12_1
		DX12_1
	};

	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct3DResource = Resource<T, TReleaser>;

	typedef Direct3DResource<::ID3D11Texture1D>          Direct3DTexture1D;
	typedef Direct3DResource<::ID3D11Texture2D>          Direct3DTexture2D;
	typedef Direct3DResource<::ID3D11Texture3D>          Direct3DTexture3D;
	typedef Direct3DResource<::ID3D11ShaderResourceView> Direct3DShaderResourceView;

	class Direct3D
	{
		typedef Direct3DResource<::ID3D11Device>           Device;
		typedef Direct3DResource<::ID3D11DeviceContext>    DeviceContext;

		typedef Direct3DResource<::IDXGISwapChain>         SwapChain;
		typedef Direct3DResource<::ID3D11RenderTargetView> TargetView;

		Device        device;
		SwapChain     swapChain;
		DeviceContext deviceContext;

		TargetView    targetView;

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
			//flags.Add(::D3D11_CREATE_DEVICE_DEBUGGABLE);
#endif

			::D3D_FEATURE_LEVEL _featureLevel;

			switch (featureLevel)
			{
				case FeatureLevels::DX9_1:
					_featureLevel = ::D3D_FEATURE_LEVEL_9_1;
					break;

				case FeatureLevels::DX9_2:
					_featureLevel = ::D3D_FEATURE_LEVEL_9_2;
					break;

				case FeatureLevels::DX9_3:
					_featureLevel = ::D3D_FEATURE_LEVEL_9_3;
					break;

				case FeatureLevels::DX10:
					_featureLevel = ::D3D_FEATURE_LEVEL_10_0;
					break;

				case FeatureLevels::DX10_1:
					_featureLevel = ::D3D_FEATURE_LEVEL_10_1;
					break;

				case FeatureLevels::DX11:
					_featureLevel = ::D3D_FEATURE_LEVEL_11_0;
					break;

				case FeatureLevels::DX11_1:
					_featureLevel = ::D3D_FEATURE_LEVEL_11_1;
					break;

				case FeatureLevels::DX12:
					_featureLevel = ::D3D_FEATURE_LEVEL_12_0;
					break;

				case FeatureLevels::DX12_1:
					_featureLevel = ::D3D_FEATURE_LEVEL_12_1;
					break;

				default:
					throw NotImplementedException();
			}

			::DXGI_SWAP_CHAIN_DESC description =
			{
				.BufferDesc =
				{
					.Width       = 0,
					.Height      = 0,
					.RefreshRate =
					{
						.Numerator   = 0,
						.Denominator = 1
					},
					.Format           = ::DXGI_FORMAT_B8G8R8A8_UNORM,
					.ScanlineOrdering = ::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
					.Scaling          = ::DXGI_MODE_SCALING_UNSPECIFIED
				},
				.SampleDesc =
				{
					.Count = Math::Clamp<::UINT>(static_cast<::UINT>(msaaLevel), 1, D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
				},
				.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount  = 1,
				.OutputWindow = hWnd,
				.Windowed     = TRUE,
				.SwapEffect   = ::DXGI_SWAP_EFFECT_DISCARD,
				.Flags        = ::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			};

			switch (antiAliasing)
			{
				case AntiAliasing::None:
					description.SampleDesc.Quality = 0;
					break;

				case AntiAliasing::Center:
					description.SampleDesc.Quality = ::D3D11_CENTER_MULTISAMPLE_PATTERN;
					break;

				case AntiAliasing::Standard:
					description.SampleDesc.Quality = ::D3D11_STANDARD_MULTISAMPLE_PATTERN;
					break;

				default:
					throw NotImplementedException();
			}

			Device::Type*        lpDevice;
			SwapChain::Type*     lpSwapChain;
			DeviceContext::Type* lpDeviceContext;

			if (FAILED(::D3D11CreateDeviceAndSwapChain(nullptr, ::D3D_DRIVER_TYPE_HARDWARE, nullptr, flags.Value, &_featureLevel, 1, D3D11_SDK_VERSION, &description, &lpSwapChain, &lpDevice, nullptr, &lpDeviceContext)))
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

			device        = lpDevice;
			swapChain     = lpSwapChain;
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

			TargetView::Type* lpTargetView;

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

		// @throw AL::Exception
		Void CreateTexture2D(Direct3DTexture2D& texture, const Void* lpBuffer, uint32 width, uint32 height, ::DXGI_FORMAT format)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct3D target not created"
			);

			Direct3DTexture2D::Type* resource;
			::D3D11_TEXTURE2D_DESC   description =
			{
				.Width      = width,
				.Height     = height,
				.MipLevels  = 1,
				.ArraySize  = 1,
				.Format     = format,
				.SampleDesc =
				{
					.Count = 1
				},
				.Usage     = ::D3D11_USAGE_DEFAULT,
				.BindFlags = ::D3D11_BIND_SHADER_RESOURCE
			};
			::D3D11_SUBRESOURCE_DATA resource_data =
			{
				.pSysMem          = lpBuffer,
				.SysMemPitch      = width * 4,
				.SysMemSlicePitch = 0
			};

			if (FAILED(GetDevice()->CreateTexture2D(&description, &resource_data, &resource)))
			{

				throw Exception(
					"Error creating ID3D11Texture2D"
				);
			}

			texture = resource;
		}

		// @throw AL::Exception
		Void CreateShaderResourceView(Direct3DShaderResourceView& view, const Direct3DTexture2D& texture, ::DXGI_FORMAT format)
		{
			AL_ASSERT(
				IsCreated(),
				"Direct3D not created"
			);

			AL_ASSERT(
				IsTargetCreated(),
				"Direct3D target not created"
			);

			::D3D11_TEXTURE2D_DESC texture_description;
			texture->GetDesc(&texture_description);

			Direct3DShaderResourceView::Type* resource;
			::D3D11_SHADER_RESOURCE_VIEW_DESC description =
			{
				.Format        = format,
				.ViewDimension = ::D3D11_SRV_DIMENSION_TEXTURE2D,
				.Texture2D =
				{
					.MostDetailedMip = 0,
					.MipLevels       = texture_description.MipLevels
				}
			};

			if (FAILED(GetDevice()->CreateShaderResourceView(texture, &description, &resource)))
			{

				throw Exception(
					"Error creating ID3D11ShaderResourceView"
				);
			}

			view = resource;
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
		auto Present(Bool vsync = False)
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

			if (FAILED(hResult))
			{
				switch (hResult)
				{
					case DXGI_STATUS_OCCLUDED:
					case DXGI_ERROR_WAS_STILL_DRAWING:
						return hResult;
				}

				throw Exception(
					"Error presenting swap chain"
				);
			}

			return hResult;
		}
	};
}
