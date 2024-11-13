#pragma once
#include "AL/Common.hpp"

#include "IQueue.hpp"

#include <atomic>

namespace AL::Collections
{
	template<typename T>
	class MPSCQueue
		: public IQueue<T>
	{
		struct Node
		{
			T                    Value;

			::std::atomic<Node*> Next = nullptr;

			template<typename ... TArgs>
			explicit Node(TArgs ... args)
				: Value(
					Forward<TArgs>(args) ...
				)
			{
			}
		};

		::std::atomic<size_t> size;
		::std::atomic<Node*>  back;
		::std::atomic<Node*>  front;

	public:
		typedef typename IQueue<T>::Type Type;

		MPSCQueue()
			: size(
				0
			),
			back(
				new Node()
			),
			front(
				back.load(
					::std::memory_order_relaxed
				)
			)
		{
		}

		MPSCQueue(MPSCQueue&& queue)
			: size(
				queue.size.exchange(
					0,
					::std::memory_order_relaxed
				)
			),
			back(
				queue.back.exchange(
					nullptr,
					::std::memory_order_relaxed
				)
			),
			front(
				queue.front.exchange(
					nullptr,
					::std::memory_order_relaxed
				)
			)
		{
		}
		MPSCQueue(const MPSCQueue& queue);

		virtual ~MPSCQueue()
		{
			if (auto lpNode = front.load(::std::memory_order_relaxed))
			{
				do
				{
					front.store(
						lpNode->Next,
						::std::memory_order_relaxed
					);

					delete lpNode;
				} while ((lpNode = front.load(::std::memory_order_relaxed)) != nullptr);
			}
		}

		virtual size_t GetSize() const override
		{
			return this->size;
		}

		virtual size_t GetCapacity() const override
		{
			return Integer<size_t>::Maximum;
		}

		Void Clear()
		{
			Type value;

			while (Dequeue(value))
			{
			}
		}

		Void Swap(MPSCQueue& queue)
		{
			AL::Swap(
				size,
				queue.size
			);

			AL::Swap(
				back,
				queue.back
			);

			AL::Swap(
				front,
				queue.front
			);
		}

		Void Enqueue(Type&& value)
		{
			auto lpNode = new Node(
				Move(value)
			);

			auto lpPrevBack = back.exchange(
				lpNode,
				::std::memory_order_acq_rel
			);

			lpPrevBack->Next.store(
				lpNode,
				::std::memory_order_release
			);

			++size;
		}
		Void Enqueue(const Type& value)
		{
			auto lpNode = new Node(
				value
			);

			auto lpPrevBack = back.exchange(
				lpNode,
				::std::memory_order_acq_rel
			);

			lpPrevBack->Next.store(
				lpNode,
				::std::memory_order_release
			);

			++size;
		}

		template<typename ... TArgs>
		inline Void Emplace(TArgs ... args)
		{
			auto lpNode = new Node(
				Forward<TArgs>(args) ...
			);

			auto lpPrevBack = back.exchange(
				lpNode,
				::std::memory_order_acq_rel
			);

			lpPrevBack->Next.store(
				lpNode,
				::std::memory_order_release
			);

			++size;
		}

		Bool Dequeue()
		{
			Type value;

			if (!Dequeue(value))
			{

				return False;
			}

			return True;
		}
		Bool Dequeue(Type& value)
		{
			auto lpFront = front.load(
				::std::memory_order_relaxed
			);
			
			Node* lpNode;

			if ((lpNode = lpFront->Next.load(::std::memory_order_acquire)) == nullptr)
			{

				return False;
			}

			value = Move(
				lpNode->Value
			);

			front.store(
				lpNode,
				::std::memory_order_release
			);

			delete lpFront;

			--size;

			return True;
		}

		MPSCQueue& operator = (MPSCQueue&& queue)
		{
			size.store(
				queue.size.load(
					::std::memory_order_relaxed
				),
				::std::memory_order_relaxed
			);
			queue.size.store(
				0,
				::std::memory_order_relaxed
			);

			back.store(
				queue.back.load(
					::std::memory_order_relaxed
				),
				::std::memory_order_relaxed
			);
			queue.back.store(
				new Node(),
				::std::memory_order_relaxed
			);

			front.store(
				queue.front.load(
					::std::memory_order_relaxed
				),
				::std::memory_order_relaxed
			);
			queue.front.store(
				queue.back.load(
					::std::memory_order_relaxed
				),
				::std::memory_order_relaxed
			);

			return *this;
		}
		MPSCQueue& operator = (const MPSCQueue& queue);

		Bool operator == (const MPSCQueue& queue) const;
		Bool operator != (const MPSCQueue& queue) const
		{
			if (operator==(queue))
			{

				return False;
			}

			return True;
		}
	};
}
