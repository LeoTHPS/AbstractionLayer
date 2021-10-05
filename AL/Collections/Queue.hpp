#pragma once
#include "AL/Common.hpp"

#include "IQueue.hpp"

namespace AL::Collections
{
	template<typename T>
	class Queue
		: public IQueue<T>
	{
		struct Node
		{
			T     Value;

			Node* lpNext     = nullptr;
			Node* lpPrevious = nullptr;

			template<typename ... TArgs>
			explicit Node(TArgs ... args)
				: Value(
					Forward<TArgs>(args) ...
				)
			{
			}
		};

		size_t size = 0;
		Node*  lpBack;
		Node*  lpFront;

	public:
		typedef typename IQueue<T>::Type Type;

		Queue()
			: lpBack(
				new Node()
			),
			lpFront(
				new Node()
			)
		{
			lpBack->lpNext      = lpFront;
			lpFront->lpPrevious = lpBack;
		}

		Queue(Queue&& queue)
			: size(
				queue.size
			),
			lpBack(
				queue.lpBack
			),
			lpFront(
				queue.lpFront
			)
		{
			queue.size    = 0;
			queue.lpBack  = nullptr;
			queue.lpFront = nullptr;
		}
		Queue(const Queue& queue);

		virtual ~Queue()
		{
			if ((lpBack != nullptr) && (lpFront != nullptr))
			{
				Clear();

				delete lpBack;
				delete lpFront;
			}
		}

		virtual size_t GetSize() const override
		{
			return size;
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

		Void Swap(Queue& queue)
		{
			AL::Swap(
				size,
				queue.size
			);

			AL::Swap(
				lpBack,
				queue.lpBack
			);

			AL::Swap(
				lpFront,
				queue.lpFront
			);
		}

		Void Enqueue(Type&& value)
		{
			auto lpNode = new Node(
				Move(value)
			);

			lpNode->lpNext     = lpBack->lpNext;
			lpNode->lpPrevious = lpBack;

			lpBack->lpNext->lpPrevious = lpNode;
			lpBack->lpNext             = lpNode;

			++size;
		}
		Void Enqueue(const Type& value)
		{
			auto lpNode = new Node(
				value
			);

			lpNode->lpNext     = lpBack->lpNext;
			lpNode->lpPrevious = lpBack;

			lpBack->lpNext->lpPrevious = lpNode;
			lpBack->lpNext             = lpNode;

			++size;
		}

		template<typename ... TArgs>
		inline Void Emplace(TArgs ... args)
		{
			auto lpNode = new Node(
				Forward<TArgs>(args) ...
			);

			lpNode->lpNext     = lpBack->lpNext;
			lpNode->lpPrevious = lpBack;

			lpBack->lpNext->lpPrevious = lpNode;
			lpBack->lpNext             = lpNode;

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
			Node* lpNode;

			if ((lpNode = lpFront->lpPrevious) == lpBack)
			{

				return False;
			}

			value = Move(
				lpNode->Value
			);

			lpFront->lpPrevious         = lpNode->lpPrevious;
			lpFront->lpPrevious->lpNext = lpFront;

			--size;

			delete lpNode;

			return True;
		}

		Queue& operator = (Queue&& queue)
		{
			if ((lpBack != nullptr) && (lpFront != nullptr))
			{
				Clear();

				delete lpBack;
				delete lpFront;
			}

			size = queue.size;
			queue.size = 0;

			lpBack = queue.lpBack;
			queue.lpBack = nullptr;

			lpFront = queue.lpFront;
			queue.lpFront = nullptr;

			return *this;
		}
		Queue& operator = (const Queue& queue);

		Bool operator == (const Queue& queue) const;
		Bool operator != (const Queue& queue) const
		{
			if (operator==(queue))
			{

				return False;
			}

			return True;
		}
	};
}
