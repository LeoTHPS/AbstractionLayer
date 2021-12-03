#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename ... TYPES>
	class Tuple
	{
		template<size_t I, typename T>
		struct Node
		{
			T Value;

			Node()
			{
			}

			explicit Node(T&& value)
				: Value(
					Move(value)
				)
			{
			}

			explicit Node(const T& value)
				: Value(
					value
				)
			{
			}

			virtual ~Node()
			{
			}
		};

		template<size_t I, typename ... _TYPES>
		struct NodeList;
		template<size_t I>
		struct NodeList<I>
		{
		};
		template<size_t I, typename T_NODE, typename ... T_NODES>
		struct NodeList<I, T_NODE, T_NODES ...>
			: public Node<I, T_NODE>,
			public NodeList<I + 1, T_NODES ...>
		{
			NodeList()
			{
			}

			NodeList(T_NODE&& value, T_NODES&& ... values)
				: Node<I, T_NODE>(
					Move(value)
				),
				NodeList<I + 1, T_NODES ...>(
					Move(values) ...
				)
			{
			}

			NodeList(const T_NODE& value, const T_NODES& ... values)
				: Node<I, T_NODE>(
					Move(value)
				),
				NodeList<I + 1, T_NODES ...>(
					Move(values) ...
				)
			{
			}
		};

		NodeList<0, TYPES ...> nodes;

	public:
		Tuple()
		{
		}

		Tuple(Tuple&& tuple)
			: nodes(
				Move(tuple.nodes)
			)
		{
		}

		Tuple(const Tuple& tuple)
			: nodes(
				tuple.nodes
			)
		{
		}

		Tuple(TYPES&& ... values)
			: nodes(
				Move(values) ...
			)
		{
		}

		Tuple(const TYPES& ... values)
			: nodes(
				values ...
			)
		{
		}

		virtual ~Tuple()
		{
		}

		template<size_t I, typename TYPE = typename Get_Type_Sequence<I, TYPES ...>::Type>
		TYPE& Get()
		{
			return static_cast<Node<I, TYPE>&>(nodes).Value;
		}
		template<size_t I, typename TYPE = typename Get_Type_Sequence<I, TYPES ...>::Type>
		const TYPE& Get() const
		{
			return static_cast<const Node<I, TYPE>&>(nodes).Value;
		}

		template<size_t I, typename TYPE = typename Get_Type_Sequence<I, TYPES ...>::Type>
		typename Enable_If<Is_Move_Assignable<TYPE>::Value, Void>::Type Set(TYPE&& value)
		{
			static_cast<Node<I, TYPE>&>(nodes).Value = Move(
				value
			);
		}
		template<size_t I, typename TYPE = typename Get_Type_Sequence<I, TYPES ...>::Type>
		typename Enable_If<Is_Copy_Assignable<TYPE>::Value, Void>::Type Set(const TYPE& value)
		{
			static_cast<Node<I, TYPE>&>(nodes).Value = value;
		}

		template<typename F>
		auto Invoke(const F& function) const
		{
			return Invoke(
				function,
				typename Make_Index_Sequence<sizeof ...(TYPES)>::Type{}
			);
		}
		template<typename T, typename C>
		auto Invoke(T(C::*lpFunction)(TYPES ...), C& instance) const
		{
			return Invoke(
				lpFunction,
				instance,
				typename Make_Index_Sequence<sizeof ...(TYPES)>::Type{}
			);
		}
		template<typename T, typename C>
		auto Invoke(T(C::*lpFunction)(TYPES ...) const, const C& instance) const
		{
			return Invoke(
				lpFunction,
				instance,
				typename Make_Index_Sequence<sizeof ...(TYPES)>::Type{}
			);
		}

		Tuple& operator = (Tuple&& tuple)
		{
			nodes = Move(
				tuple.nodes
			);

			return *this;
		}
		Tuple& operator = (const Tuple& tuple)
		{
			nodes = tuple.nodes;

			return *this;
		}

	private:
		template<typename F, size_t ... I>
		auto Invoke(const F& function, Index_Sequence<I ...>) const
		{
			return function(
				Get<I>() ...
			);
		}
		template<typename T, typename C, size_t ... I>
		auto Invoke(T(C::*lpFunction)(TYPES ...), C& instance, Index_Sequence<I ...>) const
		{
			return (instance.*lpFunction)(
				Get<I>() ...
			);
		}
		template<typename T, typename C, size_t ... I>
		auto Invoke(T(C::*lpFunction)(TYPES ...) const, const C& instance, Index_Sequence<I ...>) const
		{
			return (instance.*lpFunction)(
				Get<I>() ...
			);
		}
	};

	template<>
	class Tuple<>
	{
	public:
		Tuple()
		{
		}

		Tuple(Tuple&& tuple)
		{
		}

		Tuple(const Tuple& tuple)
		{
		}

		virtual ~Tuple()
		{
		}

		template<size_t I>
		static constexpr void Get()
		{
		}

		template<size_t I>
		static constexpr void Set()
		{
		}

		template<typename F>
		auto Invoke(const F& function) const
		{
			return function();
		}
		template<typename T, typename C>
		auto Invoke(T(C::*lpFunction)(), C& instance) const
		{
			return (instance.*lpFunction)();
		}
		template<typename T, typename C>
		auto Invoke(T(C::*lpFunction)() const, const C& instance) const
		{
			return (instance.*lpFunction)();
		}

		Tuple& operator = (Tuple&& tuple)
		{

			return *this;
		}
		Tuple& operator = (const Tuple& tuple)
		{

			return *this;
		}
	};
}
