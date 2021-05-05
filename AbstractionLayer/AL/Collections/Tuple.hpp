#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename ... TYPES>
	class Tuple
	{
		template<size_t I, typename T>
		struct Entry
		{
			T Value;

			Entry()
			{
			}

			Entry(T&& value)
				: Value(
					Move(value)
				)
			{
			}
		};

		template<size_t, typename ...>
		struct EntryList;
		template<size_t I>
		struct EntryList<I>
		{
		};
		template<size_t I, typename T_NODE, typename ... T_NODES>
		struct EntryList<I, T_NODE, T_NODES ...>
			: public Entry<I, T_NODE>,
			public EntryList<I + 1, T_NODES ...>
		{
			EntryList()
			{
			}

			EntryList(T_NODE&& value, T_NODES&& ... values)
				: Entry<I, T_NODE>(
					Move(value)
				),
				EntryList<I + 1, T_NODES ...>(
					Move(values) ...
				)
			{
			}
		};

		EntryList<0, TYPES ...> entries;

	public:
		Tuple()
		{
		}

		Tuple(Tuple&& tuple)
			: entries(
				Move(tuple.entries)
			)
		{
		}

		Tuple(const Tuple& tuple)
			: entries(
				tuple.entries
			)
		{
		}

		explicit Tuple(TYPES&& ... values)
			: entries(
				Move(values) ...
			)
		{
		}

		virtual ~Tuple()
		{
		}

		template<size_t I>
		auto& Get()
		{
			typedef typename Get_Type_Sequence<I, TYPES ...>::Type TYPE;

			return static_cast<Entry<I, TYPE>&>(entries).Value;
		}
		template<size_t I>
		auto& Get() const
		{
			typedef typename Get_Type_Sequence<I, TYPES ...>::Type TYPE;

			return static_cast<const Entry<I, TYPE>&>(entries).Value;
		}

		template<size_t I>
		void Set(typename Get_Type_Sequence<I, TYPES ...>::Type&& value)
		{
			static_cast<Entry<I, typename Get_Type_Sequence<I, TYPES ...>::Type>&>(entries).Value = Move(
				value
			);
		}
		template<size_t I>
		void Set(const typename Get_Type_Sequence<I, TYPES ...>::Type& value)
		{
			static_cast<Entry<I, typename Get_Type_Sequence<I, TYPES ...>::Type>&>(entries).Value = value;
		}

		template<typename F>
		auto Invoke(const F& func)
		{
			return Invoke(
				func,
				typename Make_Index_Sequence<sizeof ...(TYPES)>::Type{}
			);
		}

		auto& operator = (Tuple&& tuple)
		{
			entries = Move(
				tuple.entries
			);

			return *this;
		}
		auto& operator = (const Tuple& tuple)
		{
			entries = tuple.entries;

			return *this;
		}

	private:
		template<typename F, size_t ... I>
		auto Invoke(const F& func, Index_Sequence<I ...>)
		{
			return func(
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
		static constexpr void Set(void)
		{
		}

		template<typename F>
		auto Invoke(const F& func)
		{
			return func();
		}

		auto& operator = (Tuple&& tuple)
		{

			return *this;
		}
		auto& operator = (const Tuple& tuple)
		{

			return *this;
		}
	};
}
