#pragma once
#include "AL/Common.hpp"

#include <atomic>

namespace AL::Collections
{
	template<typename T>
	class MPSCQueue
	{
		struct Node
		{
			T Value;
			std::atomic<Node*> lpNext;

			template<typename ... TArgs>
			explicit Node(TArgs ... args)
				: Value(
					Forward<TArgs>(args) ...
				),
				lpNext(
					nullptr
				)
			{
			}
		};

		std::atomic<size_t> size;
		std::atomic<Node*> back;
		std::atomic<Node*> front;

		MPSCQueue(const MPSCQueue&) = delete;
		
	public:
		typedef T Type;

		MPSCQueue()
			: size(
				0
			),
			back(
				new Node()
			),
			front(
				back.load(
					std::memory_order_relaxed
				)
			)
		{
		}

		MPSCQueue(MPSCQueue&& queue)
			: size(
				queue.size
			),
			back(
				queue.back
			),
			front(
				queue.front
			)
		{
			queue.size = 0;
			queue.back = new Node();
			queue.front = queue.back;
		}

		virtual ~MPSCQueue()
		{
			while (auto lpNode = front.load(std::memory_order_relaxed))
			{
				front.store(
					lpNode->lpNext,
					std::memory_order_relaxed
				);

				delete lpNode;
			}
		}

		auto GetSize() const
		{
			return size.load(
				std::memory_order_relaxed
			);
		}

		auto GetBack()
		{
			return GetSize() ? &back.load(std::memory_order_relaxed)->Value : nullptr;
		}
		auto GetBack() const
		{
			return GetSize() ? &back.load(std::memory_order_relaxed)->Value : nullptr;
		}

		auto GetFront()
		{
			auto lpFront = front.load(
				std::memory_order_relaxed
			);

			auto lpNode = lpFront->lpNext.load(
				std::memory_order_relaxed
			);

			return lpNode ? &lpNode->Value : nullptr;
		}
		auto GetFront() const
		{
			auto lpFront = front.load(
				std::memory_order_relaxed
			);

			auto lpNode = lpFront->lpNext.load(
				std::memory_order_relaxed
			);

			return lpNode ? &lpNode->Value : nullptr;
		}

		void Clear()
		{
			while (Dequeue())
			{
			}
		}

		void Swap(MPSCQueue& queue);

		void Enqueue(Type&& value)
		{
			auto lpNode = new Node(
				Move(value)
			);

			auto prevBack = back.exchange(
				lpNode,
				std::memory_order_acq_rel
			);

			prevBack->lpNext.store(
				lpNode,
				std::memory_order_release
			);

			++size;
		}
		void Enqueue(const Type& value)
		{
			auto lpNode = new Node(
				value
			);

			auto prevBack = back.exchange(
				lpNode,
				std::memory_order_acq_rel
			);

			prevBack->lpNext.store(
				lpNode,
				std::memory_order_release
			);

			++size;
		}

		template<typename ... TArgs>
		void Emplace(TArgs ... args)
		{
			auto lpNode = new Node(
				Forward<TArgs>(args) ...
			);

			auto prevBack = back.exchange(
				lpNode,
				std::memory_order_acq_rel
			);

			prevBack->lpNext.store(
				lpNode,
				std::memory_order_release
			);

			++size;
		}

		bool Dequeue()
		{
			auto lpFront = front.load(
				std::memory_order_relaxed
			);

			if (auto lpNode = lpFront->lpNext.load(std::memory_order_acquire))
			{
				front.store(
					lpNode,
					std::memory_order_release
				);

				delete lpFront;

				--size;

				return true;
			}

			return false;
		}
		bool Dequeue(Type& value)
		{
			auto lpFront = front.load(
				std::memory_order_relaxed
			);

			if (auto lpNode = lpFront->lpNext.load(std::memory_order_acquire))
			{
				value = Move(
					lpNode->Value
				);

				front.store(
					lpNode,
					std::memory_order_release
				);

				delete lpFront;

				--size;

				return true;
			}

			return false;
		}

		auto& operator = (MPSCQueue&& queue)
		{
			size = queue.size;
			queue.size = 0;
			
			back = queue.back;
			queue.back = new Node();

			front = queue.front;
			queue.front = queue.back;

			return *this;
		}

		bool operator == (const MPSCQueue& queue) const
		{
			return this == &queue;
		}
		bool operator != (const MPSCQueue& queue) const
		{
			return !operator==(queue);
		}
	};
}
