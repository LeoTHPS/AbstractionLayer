#pragma once
#include "AL/Common.hpp"

#include "IResource.hpp"

#include "Direct3D.hpp"

namespace AL::DirectX
{
	class Direct3D12;

	template<>
	class Direct3D<Direct3D12>
	{
	public:

	};
}
