#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::DirectX
{
	template<typename T>
	struct ResourceReleaser
	{
		static void AddRef(T* lpResource)
		{
			lpResource->AddRef();
		}

		static void Release(T* lpResource)
		{
			lpResource->Release();
		}
	};

	template<typename T, typename T_RESOURCE_RELEASER = ResourceReleaser<T>>
	class Resource
	{
		T* lpResource;

	public:
		typedef T Type;

		Resource()
			: Resource(
				nullptr
			)
		{
		}

		Resource(Type* lpResource)
			: lpResource(
				lpResource
			)
		{
		}

		Resource(Resource&& resource)
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);
			}

			lpResource = resource.lpResource;
			resource.lpResource = nullptr;
		}

		Resource(const Resource& resource)
			: lpResource(
				resource.lpResource
			)
		{
			if (auto lpResource = resource.lpResource)
			{
				T_RESOURCE_RELEASER::AddRef(lpResource);
			}
		}

		virtual ~Resource()
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);
			}
		}

		bool IsCreated() const
		{
			return lpResource != nullptr;
		}

		void Release()
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);

				this->lpResource = nullptr;
			}
		}

		operator Type* () const
		{
			return lpResource;
		}

		auto operator -> () const
		{
			return lpResource;
		}

		auto& operator = (Type* lpResource)
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);
			}

			this->lpResource = lpResource;
			
			return *this;
		}

		auto& operator = (Resource&& resource)
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);
			}

			lpResource = resource.lpResource;
			resource.lpResource = nullptr;

			return *this;
		}

		auto& operator = (const Resource& resource)
		{
			if (auto lpResource = this->lpResource)
			{
				T_RESOURCE_RELEASER::Release(lpResource);
			}
			
			if (auto lpResource = resource.lpResource)
			{
				T_RESOURCE_RELEASER::AddRef(lpResource);
			}

			lpResource = resource.lpResource;

			return *this;
		}

		bool operator == (const Resource& resource) const
		{
			return lpResource == resource.lpResource;
		}
		bool operator != (const Resource& resource) const
		{
			return !operator==(resource);
		}

	protected:
		Type* GetResource() const
		{
			return lpResource;
		}
	};
}
