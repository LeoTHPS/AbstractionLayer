#pragma once
#include "AL/Common.hpp"

#include "Resource.hpp"
#include "FeatureLevels.hpp"

#include "AL/Graphics/Color.hpp"
#include "AL/Graphics/Vector.hpp"
#include "AL/Graphics/Rectangle.hpp"

#if __has_include(<d3d10_1.h>)
	#include <d3d10_1.h>

	#define AL_DEPENDENCY_DIRECT3D_10_1

	#define AL_DEPENDENCY_DIRECT3D_10_VERSION 10.1
#elif __has_include(<d3d10.h>)
	#include <d3d10.h>

	#define AL_DEPENDENCY_DIRECT3D_10

	#define AL_DEPENDENCY_DIRECT3D_10_VERSION 10.0
#endif

namespace AL::DirectX
{
	template<typename T, typename TReleaser = ResourceReleaser<T>>
	using Direct3D10Resource = Resource<T, TReleaser>;

	class Direct3D10
	{
		Direct3D10(Direct3D10&&) = delete;
		Direct3D10(const Direct3D10&) = delete;

	public:
		Direct3D10()
		{
		}

		virtual ~Direct3D10()
		{
		}

		bool IsCreated() const;

		bool IsTargetCreated() const;

		// @throw AL::Exceptions::Exception
		void SetClientSize(uint32 width, uint32 height);

		// @throw AL::Exceptions::Exception
		void Create(HWND hWnd, FeatureLevels featureLevel = FeatureLevels::DX10);

		void Destroy();

		// @throw AL::Exceptions::Exception
		void CreateTarget();

		void DestroyTarget();

		void Clear(Graphics::Color color);

		// @throw AL::Exceptions::Exception
		void Present(bool vsync = false);
	};
}
