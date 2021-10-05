#pragma once
#include "AL/Common.hpp"

#include "ICollection.hpp"

namespace AL::Collections
{
	template<typename T>
	class IQueue
		: public ICollectionBase
	{
	public:
		typedef T Type;

		IQueue()
		{
		}

		virtual ~IQueue()
		{
		}
	};
}
