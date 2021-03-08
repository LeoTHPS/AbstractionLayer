#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"
#include "FeatureLevels.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#if __has_include(<d3d12.h>)
	#include <d3d12.h>

	#define AL_DEPENDENCY_DIRECT3D_12

	#define AL_DEPENDENCY_DIRECT3D_12_VERSION 12.0
#endif

namespace AL::DirectX
{
	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct3D12Resource = Resource<T, TReleaser>;

	class Direct3D12
	{
		Direct3D12(Direct3D12&&) = delete;
		Direct3D12(const Direct3D12&) = delete;

	public:
		Direct3D12()
		{
		}

		virtual ~Direct3D12()
		{
		}

		bool IsCreated() const;

		bool IsTargetCreated() const;

		// @throw AL::Exceptions::Exception
		void SetClientSize(uint32 width, uint32 height);

		// @throw AL::Exceptions::Exception
		void Create(HWND hWnd, FeatureLevels featureLevel = FeatureLevels::DX12);

		void Destroy();

		// @throw AL::Exceptions::Exception
		void CreateTarget();

		void DestroyTarget();

		void Clear(Graphics::Color color);

		// @throw AL::Exceptions::Exception
		void Present(bool vsync = false);
	};
}
