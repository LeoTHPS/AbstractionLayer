#pragma once
#include "AL/Common.hpp"

#include <queue>

namespace AL::Collections
{
	template<typename T>
	class Queue
	{
		typedef std::queue<T> Container;

		Container container;

	public:
		typedef T Type;

		auto GetSize() const
		{
			return container.size();
		}

		auto GetBack()
		{
			return GetSize() ? &container.back() : nullptr;
		}
		auto GetBack() const
		{
			return GetSize() ? &container.back() : nullptr;
		}

		auto GetFront()
		{
			return GetSize() ? &container.front() : nullptr;
		}
		auto GetFront() const
		{
			return GetSize() ? &container.front() : nullptr;
		}

		void Clear()
		{
			while (container.size())
			{
				container.pop();
			}
		}

		void Swap(Queue& queue)
		{
			container.swap(
				queue.container
			);
		}

		void Enqueue(Type&& value)
		{
			container.push(
				Move(value)
			);
		}
		void Enqueue(const Type& value)
		{
			container.push(
				value
			);
		}

		template<typename ... TArgs>
		void Emplace(TArgs ... args)
		{
			container.emplace(
				Forward<TArgs>(args) ...
			);
		}

		bool Dequeue()
		{
			if (GetSize())
			{
				container.pop();

				return true;
			}

			return false;
		}
		bool Dequeue(Type& value)
		{
			if (GetSize())
			{
				value = Move(
					container.front()
				);

				container.pop();

				return true;
			}

			return false;
		}

		bool operator == (const Queue& queue) const
		{
			return container == queue.container;
		}
		bool operator != (const Queue& queue) const
		{
			return !operator==(queue);
		}
	};
}
