#pragma once
#include "AL/Common.hpp"

#include "Queue.hpp"

namespace AL::Collections
{
	template<typename T>
	class MPMCQueue
	{
		Queue<T> container;
		OS::Mutex containerMutex;

	public:
		typedef T Type;

		MPMCQueue()
		{
		}

		MPMCQueue(MPMCQueue&& queue)
			: container(
				Move(queue.container)
			)
		{
		}

		MPMCQueue(const MPMCQueue& queue)
			: container(
				queue.container
			)
		{
		}

		virtual ~MPMCQueue()
		{
		}

		auto GetSize() const
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.size();
		}

		auto GetBack()
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.GetBack();
		}
		auto GetBack() const
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.GetBack();
		}

		auto GetFront()
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.GetFront();
		}
		auto GetFront() const
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.GetFront();
		}

		void Clear()
		{
			OS::MutexGuard lock(
				containerMutex
			);

			container.Clear();
		}

		void Swap(MPMCQueue& queue)
		{
			OS::MutexGuard lock(
				containerMutex
			);

			container.Swap(
				queue.container
			);
		}

		void Enqueue(Type&& value)
		{
			OS::MutexGuard lock(
				containerMutex
			);

			container.Enqueue(
				Move(value)
			);
		}
		void Enqueue(const Type& value)
		{
			OS::MutexGuard lock(
				containerMutex
			);

			container.Enqueue(
				value
			);
		}

		template<typename ... TArgs>
		void Emplace(TArgs ... args)
		{
			OS::MutexGuard lock(
				containerMutex
			);

			container.Emplace<TArgs>(
				Forward<TArgs>(args) ...
			);
		}

		bool Dequeue()
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.Dequeue();
		}
		bool Dequeue(Type& value)
		{
			OS::MutexGuard lock(
				containerMutex
			);

			return container.Dequeue(
				value
			);
		}

		auto& operator = (MPMCQueue&& queue)
		{
			container = Move(
				queue.container
			);

			return *this;
		}
		auto& operator = (const MPMCQueue& queue)
		{
			container = queue.container;

			return *this;
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
