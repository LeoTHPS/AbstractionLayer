#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"

namespace AL::Collections
{
	template<typename T>
	class CircularQueue
	{
		size_t size = 0;
		Array<T> buffer;

		size_t back = -1;
		size_t front = 0;

	public:
		explicit CircularQueue(size_t capacity)
			: buffer(
				capacity
			)
		{
		}

		CircularQueue(CircularQueue&& queue)
			: size(
				queue.size
			),
			buffer(
				Move(queue.buffer)
			),
			back(
				queue.back
			),
			front(
				queue.front
			)
		{
			queue.size = 0;
			queue.back = -1;
			queue.front = 0;
		}

		CircularQueue(const CircularQueue& queue)
			: size(
				queue.size
			),
			buffer(
				queue.buffer
			),
			back(
				queue.back
			),
			front(
				queue.front
			)
		{
		}

		virtual ~CircularQueue()
		{
		}

		auto GetSize() const
		{
			return size;
		}

		auto GetCapacity() const
		{
			return buffer.GetSize();
		}

		auto GetFront()
		{
			return GetSize() ? &buffer[front] : nullptr;
		}
		auto GetFront() const
		{
			return GetSize() ? &buffer[front] : nullptr;
		}

		auto GetBack()
		{
			return GetSize() ? &buffer[back] : nullptr;
		}
		auto GetBack() const
		{
			return GetSize() ? &buffer[back] : nullptr;
		}

		bool Enqueue(T&& value)
		{
			auto capacity = GetCapacity();

			if (GetSize() < capacity)
			{
				++size;

				if (++back == capacity)
				{

					back = 0;
				}

				buffer[back] = Move(
					value
				);

				return true;
			}

			return false;
		}
		bool Enqueue(const T& value)
		{
			auto capacity = GetCapacity();

			if (GetSize() < capacity)
			{
				++size;

				if (++back == capacity)
				{

					back = 0;
				}

				buffer[back] = value;

				return true;
			}

			return false;
		}

		bool Dequeue()
		{
			T value;

			return Dequeue(
				value
			);
		}
		bool Dequeue(T& value)
		{
			if (GetSize())
			{
				--size;

				value = Move(
					buffer[front]
				);

				if (++front == GetCapacity())
				{

					front = 0;
				}

				return true;
			}

			return false;
		}

		void Clear()
		{
			size = 0;
			back = -1;
			front = 0;
		}

		auto& operator = (CircularQueue&& queue)
		{
			size = queue.size;
			buffer = Move(
				queue.buffer
			);
			back = queue.back;
			front = queue.front;

			queue.size = 0;
			queue.back = -1;
			queue.front = 0;

			return *this;
		}
		auto& operator = (const CircularQueue& queue)
		{
			size = queue.size;
			buffer = queue.buffer;
			back = queue.back;
			front = queue.front;

			return *this;
		}

		bool operator == (const CircularQueue& queue) const
		{
			return buffer == queue.buffer;
		}
		bool operator != (const CircularQueue& queue) const
		{
			return !operator==(queue);
		}
	};

	template<typename T, size_t SIZE>
	class CircularQueue<T[SIZE]>
	{
		size_t size = 0;
		Array<T[S]> buffer;

		size_t back = -1;
		size_t front = 0;

	public:
		CircularQueue()
		{
		}

		CircularQueue(CircularQueue&& queue)
			: size(
				queue.size
			),
			buffer(
				Move(queue.buffer)
			),
			back(
				queue.back
			),
			front(
				queue.front
			)
		{
			queue.size = 0;
			queue.back = -1;
			queue.front = 0;
		}

		CircularQueue(const CircularQueue& queue)
			: size(
				queue.size
			),
			buffer(
				queue.buffer
			),
			back(
				queue.back
			),
			front(
				queue.front
			)
		{
		}

		auto GetSize() const
		{
			return size;
		}

		static constexpr auto GetCapacity()
		{
			return S;
		}

		auto GetFront()
		{
			return GetSize() ? &buffer[front] : nullptr;
		}
		auto GetFront() const
		{
			return GetSize() ? &buffer[front] : nullptr;
		}

		auto GetBack()
		{
			return GetSize() ? &buffer[back] : nullptr;
		}
		auto GetBack() const
		{
			return GetSize() ? &buffer[back] : nullptr;
		}

		bool Enqueue(T&& value)
		{
			auto capacity = GetCapacity();

			if (GetSize() < capacity)
			{
				++size;

				if (++back == capacity)
				{

					back = 0;
				}

				buffer[back] = Move(
					value
				);

				return true;
			}

			return false;
		}
		bool Enqueue(const T& value)
		{
			auto capacity = GetCapacity();

			if (GetSize() < capacity)
			{
				++size;

				if (++back == capacity)
				{

					back = 0;
				}

				buffer[back] = value;

				return true;
			}

			return false;
		}

		bool Dequeue()
		{
			T value;

			return Dequeue(
				value
			);
		}
		bool Dequeue(T& value)
		{
			if (GetSize())
			{
				--size;

				value = Move(
					buffer[front]
				);

				if (++front == GetCapacity())
				{

					front = 0;
				}

				return true;
			}

			return false;
		}

		void Clear()
		{
			size = 0;
			back = -1;
			front = 0;
		}

		auto& operator = (CircularQueue&& queue)
		{
			size = queue.size;
			buffer = Move(
				queue.buffer
			);
			back = queue.back;
			front = queue.front;

			queue.size = 0;
			queue.back = -1;
			queue.front = 0;

			return *this;
		}
		auto& operator = (const CircularQueue& queue)
		{
			size = queue.size;
			buffer = queue.buffer;
			back = queue.back;
			front = queue.front;

			return *this;
		}

		bool operator == (const CircularQueue& queue) const
		{
			return buffer == queue.buffer;
		}
		bool operator != (const CircularQueue& queue) const
		{
			return !operator==(queue);
		}
	};
}
