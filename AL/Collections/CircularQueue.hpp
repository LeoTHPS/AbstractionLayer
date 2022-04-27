#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"
#include "IQueue.hpp"

namespace AL::Collections
{
	template<typename T>
	class CircularQueue
		: public IQueue<T>
	{
		size_t   size  = 0;
		size_t   first = 0;
		size_t   last;
		Array<T> container;

	public:
		typedef typename IQueue<T>::Type Type;

		explicit CircularQueue(size_t capacity)
			: last(
				capacity - 1
			),
			container(
				capacity
			)
		{
		}

		CircularQueue(CircularQueue&& circularQueue)
			: size(
				circularQueue.size
			),
			first(
				circularQueue.first
			),
			last(
				circularQueue.last
			),
			container(
				Move(circularQueue.container)
			)
		{
			circularQueue.size  = 0;
			circularQueue.first = 0;
			circularQueue.last  = 0;
		}
		CircularQueue(const CircularQueue& circularQueue)
			: size(
				circularQueue.size
			),
			first(
				circularQueue.first
			),
			last(
				circularQueue.last
			),
			container(
				circularQueue.container
			)
		{
		}

		virtual ~CircularQueue()
		{
		}

		virtual size_t GetSize() const override
		{
			return size;
		}

		virtual size_t GetCapacity() const override
		{
			return container.GetCapacity();
		}

		Void Clear()
		{
			size  = 0;
			first = 0;
			last  = GetCapacity() - 1;
		}

		Void Swap(CircularQueue& circularQueue)
		{
			AL::Swap(
				size,
				circularQueue.size
			);

			AL::Swap(
				first,
				circularQueue.first
			);

			AL::Swap(
				last,
				circularQueue.last
			);

			AL::Swap(
				container,
				circularQueue.container
			);
		}

		Bool Enqueue(Type&& value)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = Move(
				value
			);

			++size;

			return True;
		}
		Bool Enqueue(const Type& value)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = value;

			++size;

			return True;
		}

		template<typename ... TArgs>
		inline Bool Emplace(TArgs ... args)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = Type(
				Forward<TArgs>(args) ...
			);

			++size;

			return True;
		}

		Bool Dequeue()
		{
			if (GetSize() == 0)
			{

				return False;
			}

			if (++first == GetCapacity())
			{

				first = 0;
			}

			--size;

			return True;
		}
		Bool Dequeue(Type& value)
		{
			if (GetSize() == 0)
			{

				return False;
			}

			value = Move(
				container[first]
			);

			if (++first == GetCapacity())
			{

				first = 0;
			}

			--size;

			return True;
		}

		CircularQueue& operator = (CircularQueue&& circularQueue)
		{
			size = circularQueue.size;
			circularQueue.size = 0;

			first = circularQueue.first;
			circularQueue.first = 0;

			last = circularQueue.last;
			circularQueue.last = 0;

			container = Move(
				circularQueue.container
			);

			return *this;
		}
		CircularQueue& operator = (const CircularQueue& circularQueue)
		{
			size      = circularQueue.size;
			first     = circularQueue.first;
			last      = circularQueue.last;
			container = circularQueue.container;

			return *this;
		}

		Bool operator == (const CircularQueue& circularQueue) const;
		Bool operator != (const CircularQueue& circularQueue) const
		{
			if (operator==(circularQueue))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T, size_t S>
	class CircularQueue<T[S]>
		: public IQueue<T>
	{
		size_t      size  = 0;
		size_t      first = 0;
		size_t      last  = S - 1;
		Array<T[S]> container;

	public:
		typedef typename IQueue<T>::Type Type;

		CircularQueue()
		{
		}

		CircularQueue(CircularQueue&& circularQueue)
			: size(
				circularQueue.size
			),
			first(
				circularQueue.first
			),
			last(
				circularQueue.last
			),
			container(
				Move(circularQueue.container)
			)
		{
			circularQueue.size  = 0;
			circularQueue.first = 0;
			circularQueue.last  = 0;
		}
		CircularQueue(const CircularQueue& circularQueue)
			: size(
				circularQueue.size
			),
			first(
				circularQueue.first
			),
			last(
				circularQueue.last
			),
			container(
				circularQueue.container
			)
		{
		}

		virtual ~CircularQueue()
		{
		}

		virtual size_t GetSize() const override
		{
			return size;
		}

		virtual size_t GetCapacity() const override
		{
			return container.GetCapacity();
		}

		Void Clear()
		{
			size  = 0;
			first = 0;
			last  = GetCapacity() - 1;
		}

		Void Swap(CircularQueue& circularQueue)
		{
			AL::Swap(
				size,
				circularQueue.size
			);

			AL::Swap(
				first,
				circularQueue.first
			);

			AL::Swap(
				last,
				circularQueue.last
			);

			AL::Swap(
				container,
				circularQueue.container
			);
		}

		Bool Enqueue(Type&& value)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = Move(
				value
			);

			++size;

			return True;
		}
		Bool Enqueue(const Type& value)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = value;

			++size;

			return True;
		}

		template<typename ... TArgs>
		inline Bool Emplace(TArgs ... args)
		{
			auto capacity = GetCapacity();

			if (capacity == GetSize())
			{

				return False;
			}

			if (++last == capacity)
			{

				last = 0;
			}

			container[last] = Type(
				Forward<TArgs>(args) ...
			);

			++size;

			return True;
		}

		Bool Dequeue()
		{
			if (GetSize() == 0)
			{

				return False;
			}

			if (++first == GetCapacity())
			{

				first = 0;
			}

			--size;

			return True;
		}
		Bool Dequeue(Type& value)
		{
			if (GetSize() == 0)
			{

				return False;
			}

			value = Move(
				container[first]
			);

			if (++first == GetCapacity())
			{

				first = 0;
			}

			--size;

			return True;
		}

		CircularQueue& operator = (CircularQueue&& circularQueue)
		{
			size = circularQueue.size;
			circularQueue.size = 0;

			first = circularQueue.first;
			circularQueue.first = 0;

			last = circularQueue.last;
			circularQueue.last = 0;

			container = Move(
				circularQueue.container
			);

			return *this;
		}
		CircularQueue& operator = (const CircularQueue& circularQueue)
		{
			size      = circularQueue.size;
			first     = circularQueue.first;
			last      = circularQueue.last;
			container = circularQueue.container;

			return *this;
		}

		Bool operator == (const CircularQueue& circularQueue) const;
		Bool operator != (const CircularQueue& circularQueue) const
		{
			if (operator==(circularQueue))
			{

				return False;
			}

			return True;
		}
	};
}
