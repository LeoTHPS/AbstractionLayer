#pragma once
#include "AL/Common.hpp"

#include "Iterator.hpp"
#include "ICollection.hpp"

namespace AL::Collections
{
	template<typename T>
	struct LinkedListNode
	{
		T               Value;

		LinkedListNode* lpNext     = nullptr;
		LinkedListNode* lpPrevious = nullptr;

		template<typename ... TArgs>
		explicit LinkedListNode(TArgs ... args)
			: Value(
				Forward<TArgs>(args) ...
			)
		{
		}

		virtual ~LinkedListNode()
		{
		}
	};

	template<typename T>
	struct Get_LinkedListNode_Value_Type;
	template<typename T>
	struct Get_LinkedListNode_Value_Type<LinkedListNode<T>>
	{
		typedef T Type;
	};
	template<typename T>
	struct Get_LinkedListNode_Value_Type<const LinkedListNode<T>>
	{
		typedef const T Type;
	};

	template<typename T>
	class LinkedListIterator
		: public BidirectionalIterator<T>
	{
		typedef typename Get_LinkedListNode_Value_Type<T>::Type _Node_Type;

	public:
		typedef typename BidirectionalIterator<_Node_Type>::pointer           pointer;
		typedef typename BidirectionalIterator<_Node_Type>::reference         reference;
		typedef typename BidirectionalIterator<_Node_Type>::value_type        value_type;
		typedef typename BidirectionalIterator<_Node_Type>::difference_type   difference_type;
		typedef typename BidirectionalIterator<_Node_Type>::iterator_category iterator_category;

		T* lpNode;

		LinkedListIterator()
		{
		}

		LinkedListIterator(T* lpNode)
			: lpNode(
				lpNode
			)
		{
		}

		virtual ~LinkedListIterator()
		{
		}

		reference operator * () const
		{
			return lpNode->Value;
		}
		pointer   operator -> () const
		{
			return &lpNode->Value;
		}

		LinkedListIterator& operator ++ ()
		{
			lpNode = lpNode->lpNext;

			return *this;
		}
		LinkedListIterator& operator -- ()
		{
			lpNode = lpNode->lpPrevious;

			return *this;
		}

		LinkedListIterator operator ++ (int)
		{
			auto it = *this;

			lpNode = lpNode->lpNext;

			return it;
		}
		LinkedListIterator operator -- (int)
		{
			auto it = *this;

			lpNode = lpNode->lpPrevious;

			return it;
		}

		Bool operator == (const LinkedListIterator& it) const
		{
			if (lpNode != it.lpNode)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const LinkedListIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class LinkedListReverseIterator
		: public BidirectionalIterator<T>
	{
		typedef typename Get_LinkedListNode_Value_Type<T>::Type _Node_Type;

	public:
		typedef typename BidirectionalIterator<_Node_Type>::pointer           pointer;
		typedef typename BidirectionalIterator<_Node_Type>::reference         reference;
		typedef typename BidirectionalIterator<_Node_Type>::value_type        value_type;
		typedef typename BidirectionalIterator<_Node_Type>::difference_type   difference_type;
		typedef typename BidirectionalIterator<_Node_Type>::iterator_category iterator_category;

		T* lpNode;

		LinkedListReverseIterator()
		{
		}

		LinkedListReverseIterator(T* lpNode)
			: lpNode(
				lpNode
			)
		{
		}

		virtual ~LinkedListReverseIterator()
		{
		}

		reference operator * () const
		{
			return lpNode->Value;
		}
		pointer   operator -> () const
		{
			return &lpNode->Value;
		}

		LinkedListReverseIterator& operator ++ ()
		{
			lpNode = lpNode->lpPrevious;

			return *this;
		}
		LinkedListReverseIterator& operator -- ()
		{
			lpNode = lpNode->lpNext;

			return *this;
		}

		LinkedListReverseIterator operator ++ (int)
		{
			auto it = *this;

			lpNode = lpNode->lpPrevious;

			return it;
		}
		LinkedListReverseIterator operator -- (int)
		{
			auto it = *this;

			lpNode = lpNode->lpNext;

			return it;
		}

		Bool operator == (const LinkedListReverseIterator& it) const
		{
			if (lpNode != it.lpNode)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const LinkedListReverseIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class LinkedList
		: public IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>
	{
		LinkedListNode<T>* lpLast;
		LinkedListNode<T>* lpFirst;
		size_t             size = 0;

	public:
		typedef typename IBidirectionalCollection<T, LinkedListIterator, LinkedListReverseIterator>::Type                                 Type;

		typedef typename IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>::Collection           Collection;

		typedef typename IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>::Iterator             Iterator;
		typedef typename IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>::ConstIterator        ConstIterator;

		typedef typename IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>::ReverseIterator      ReverseIterator;
		typedef typename IBidirectionalCollection<LinkedListNode<T>, LinkedListIterator, LinkedListReverseIterator>::ConstReverseIterator ConstReverseIterator;

		LinkedList()
			: lpLast(
				new LinkedListNode<T>()
			),
			lpFirst(
				new LinkedListNode<T>()
			)
		{
			lpLast->lpPrevious = lpFirst;
			lpFirst->lpNext    = lpLast;
		}

		LinkedList(LinkedList&& linkedList)
			: lpLast(
				linkedList.lpLast
			),
			lpFirst(
				linkedList.lpFirst
			),
			size(
				linkedList.size
			)
		{
			linkedList.lpLast  = nullptr;
			linkedList.lpFirst = nullptr;
			linkedList.size    = 0;
		}
		LinkedList(const LinkedList& linkedList)
			: LinkedList()
		{
			for (auto& value : linkedList)
			{
				PushBack(
					value
				);
			}
		}

		template<size_t S>
		LinkedList(const Type(&values)[S])
			: LinkedList(
				&values[0],
				S
			)
		{
		}
		LinkedList(const Type* lpValues, size_t count)
			: LinkedList()
		{
			for (size_t i = 0; i < count; ++i, ++lpValues)
			{
				PushBack(
					*lpValues
				);
			}
		}

		virtual ~LinkedList()
		{
			if ((lpFirst != nullptr) && (lpLast != nullptr))
			{
				for (auto lpNode = lpFirst; lpNode != nullptr; )
				{
					auto lpNext = lpNode->lpNext;

					delete lpNode;

					lpNode = lpNext;
				}
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

		Iterator      Find(const Type& value)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return end();
		}
		ConstIterator Find(const Type& value) const
		{
			for (auto it = cbegin(); it != cend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return cend();
		}

		ReverseIterator      FindLast(const Type& value)
		{
			for (auto it = rbegin(); it != rend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return rend();
		}
		ConstReverseIterator FindLast(const Type& value) const
		{
			for (auto it = crbegin(); it != crend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return crend();
		}

		Bool Contains(const Type& value) const
		{
			for (auto lpNode = lpFirst->lpNext; lpNode != lpLast; lpNode = lpNode->lpNext)
			{
				if (lpNode->Value == value)
				{

					return True;
				}
			}

			return False;
		}

		Void Clear()
		{
			for (auto lpNode = lpFirst->lpNext; lpNode != lpLast; )
			{
				auto lpNext = lpNode->lpNext;

				delete lpNode;

				lpNode = lpNext;
			}

			size               = 0;
			lpLast->lpPrevious = lpFirst;
			lpFirst->lpNext    = lpLast;
		}

		Void PopBack()
		{
			Erase(
				--end()
			);
		}

		Void PopFront()
		{
			Erase(
				begin()
			);
		}

		Void PushBack(Type&& value)
		{
			Insert(
				end(),
				Move(value)
			);
		}
		Void PushBack(const Type& value)
		{
			Insert(
				end(),
				value
			);
		}

		Void PushFront(Type&& value)
		{
			Insert(
				begin(),
				Move(value)
			);
		}
		Void PushFront(const Type& value)
		{
			Insert(
				begin(),
				value
			);
		}

		template<typename T_ITERATOR, typename ... TArgs>
		Void Emplace(T_ITERATOR it, TArgs ... args)
		{
			Type value(
				Forward<TArgs>(args) ...
			);

			Insert(
				it,
				Move(value)
			);
		}

		template<typename ... TArgs>
		Void EmplaceBack(TArgs ... args)
		{
			Emplace(
				end(),
				Forward<TArgs>(args) ...
			);
		}

		template<typename ... TArgs>
		Void EmplaceFront(TArgs ... args)
		{
			Emplace(
				begin(),
				Forward<TArgs>(args) ...
			);
		}

		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR it)
		{
			auto lpNode     = it.lpNode;
			auto lpNextNode = lpNode->lpNext;
			auto lpPrevNode = lpNode->lpPrevious;

			delete lpNode;

			lpPrevNode->lpNext     = lpNextNode;
			lpNextNode->lpPrevious = lpPrevNode;

			--size;
		}
		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR first, T_ITERATOR last);

		Void Remove(const Type& value)
		{
			auto it = Find(
				value
			);

			if (it != end())
			{

				Erase(
					it
				);
			}
		}

		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, Type&& value)
		{
			auto lpNextNode = it.lpNode;
			auto lpPrevNode = it.lpNode->lpPrevious;

			auto lpNewNode = new LinkedListNode<T>(
				Move(value)
			);

			lpNewNode->lpNext      = lpNextNode;
			lpNextNode->lpPrevious = lpNewNode;
			lpNewNode->lpPrevious  = lpPrevNode;
			lpPrevNode->lpNext     = lpNewNode;

			++size;

			return T_ITERATOR(
				lpNewNode
			);
		}
		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, const Type& value)
		{
			auto lpNextNode = it.lpNode;
			auto lpPrevNode = it.lpNode->lpPrevious;

			auto lpNewNode = new LinkedListNode<T>(
				value
			);

			lpNewNode->lpNext = lpNextNode;
			lpNextNode->lpPrevious = lpNewNode;
			lpNewNode->lpPrevious = lpPrevNode;
			lpPrevNode->lpNext = lpNewNode;

			++size;

			return T_ITERATOR(
				lpNewNode
			);
		}
		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, const Type* lpValues, size_t count);

		virtual Iterator      begin() override
		{
			return Iterator(
				(lpFirst != nullptr) ? lpFirst->lpNext : nullptr
			);
		}
		virtual ConstIterator begin() const override
		{
			return ConstIterator(
				(lpFirst != nullptr) ? lpFirst->lpNext : nullptr
			);
		}

		virtual Iterator      end() override
		{
			return Iterator(
				(lpFirst != nullptr) ? lpLast : nullptr
			);
		}
		virtual ConstIterator end() const override
		{
			return ConstIterator(
				(lpFirst != nullptr) ? lpLast : nullptr
			);
		}

		virtual ConstIterator cbegin() const override
		{
			return ConstIterator(
				(lpFirst != nullptr) ? lpFirst->lpNext : nullptr
			);
		}

		virtual ConstIterator cend() const override
		{
			return ConstIterator(
				(lpFirst != nullptr) ? lpLast : nullptr
			);
		}

		virtual ReverseIterator      rbegin() override
		{
			return ReverseIterator(
				(lpFirst != nullptr) ? lpLast->lpPrevious : nullptr
			);
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return ConstReverseIterator(
				(lpFirst != nullptr) ? lpLast->lpPrevious : nullptr
			);
		}

		virtual ReverseIterator      rend() override
		{
			return ReverseIterator(
				(lpFirst != nullptr) ? lpFirst : nullptr
			);
		}
		virtual ConstReverseIterator rend() const override
		{
			return ConstReverseIterator(
				(lpFirst != nullptr) ? lpFirst : nullptr
			);
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return ConstReverseIterator(
				(lpFirst != nullptr) ? lpLast->lpPrevious : nullptr
			);
		}

		virtual ConstReverseIterator crend() const override
		{
			return ConstReverseIterator(
				(lpFirst != nullptr) ? lpFirst : nullptr
			);
		}

		LinkedList& operator = (LinkedList&& linkedList)
		{
			if ((lpFirst != nullptr) && (lpLast != nullptr))
			{
				for (auto lpNode = lpFirst; lpNode != nullptr; )
				{
					auto lpNext = lpNode->lpNext;

					delete lpNode;

					lpNode = lpNext;
				}
			}

			lpFirst = linkedList.lpFirst;
			linkedList.lpFirst = nullptr;

			lpLast = linkedList.lpLast;
			linkedList.lpLast = nullptr;

			size = linkedList.size;
			linkedList.size = 0;

			return *this;
		}
		LinkedList& operator = (const LinkedList& linkedList)
		{
			Clear();

			if ((lpFirst == nullptr) && (lpLast == nullptr))
			{
				lpFirst = new LinkedListNode<T>();
				lpLast  = new LinkedListNode<T>();

				lpFirst->lpNext = lpLast;
				lpLast->lpPrevious = lpFirst;
			}

			for (auto& value : linkedList)
			{
				PushBack(
					value
				);
			}

			return *this;
		}

		Bool operator == (const LinkedList& linkedList) const
		{
			if (GetSize() != linkedList.GetSize())
			{

				return False;
			}

			for (auto it1 = cbegin(), it2 = linkedList.cbegin(); it1 != cend(); ++it1, ++it2)
			{
				if (*it1 != *it2)
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const LinkedList& linkedList) const
		{
			if (operator==(linkedList))
			{

				return False;
			}

			return True;
		}
	};
}
