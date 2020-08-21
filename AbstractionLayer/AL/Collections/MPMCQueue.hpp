#pragma once
#include "AL/Common.hpp"

#include "Queue.hpp"

#include <mutex>

namespace AL::Collections
{
	template<typename T>
	class MPMCQueue
	{
		Queue<T> container;
		std::mutex containerMutex;

	public:
		typedef T Type;

		auto GetSize() const
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.size();
		}

		auto GetBack()
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.GetBack();
		}
		auto GetBack() const
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.GetBack();
		}

		auto GetFront()
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.GetFront();
		}
		auto GetFront() const
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.GetFront();
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			container.Clear();
		}

		void Swap(MPMCQueue& queue)
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			container.Swap(
				queue.container
			);
		}

		void Enqueue(Type&& value)
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			container.Enqueue(
				Move(value)
			);
		}
		void Enqueue(const Type& value)
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			container.Enqueue(
				value
			);
		}

		template<typename ... TArgs>
		void Emplace(TArgs ... args)
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			container.Emplace<TArgs>(
				Forward<TArgs>(args) ...
			);
		}

		bool Dequeue()
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.Dequeue();
		}
		bool Dequeue(Type& value)
		{
			std::lock_guard<std::mutex> lock(
				containerMutex
			);

			return container.Dequeue(
				value
			);
		}

		bool operator == (const MPMCQueue& queue) const
		{
			return container == queue.container;
		}
		bool operator != (const MPMCQueue& queue) const
		{
			return !operator==(queue);
		}
	};
}
