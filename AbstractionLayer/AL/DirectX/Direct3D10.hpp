#pragma once
#include "AL/Common.hpp"

#include "IResource.hpp"

#include "Direct3D.hpp"

namespace AL::DirectX
{
	class Direct3D10;

	template<>
	class Direct3D<Direct3D10>
		: public IDirect3D
	{
	public:

	};
}
