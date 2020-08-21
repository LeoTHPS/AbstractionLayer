#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename ... TYPES>
	class Tuple
	{
		template<size_t I, typename TYPE>
		struct Entry
		{
			TYPE Value;

			Entry()
			{
			}

			Entry(TYPE&& value)
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
		template<size_t I, typename TYPE, typename ... TYPES>
		struct EntryList<I, TYPE, TYPES ...>
			: public Entry<I, TYPE>,
			public EntryList<I + 1, TYPES ...>
		{
			EntryList()
			{
			}

			EntryList(TYPE&& value, TYPES&& ... values)
				: Entry<I, TYPE>(
					Move(value)
				),
				EntryList<I + 1, TYPES ...>(
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

		explicit Tuple(TYPES&& ... values)
			: entries(
				Move(values) ...
			)
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

			return static_cast<Entry<I, TYPE>&>(entries).Value;
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
				typename Make_Index_Sequence<sizeof ...(TYPES)>::Type()
			);
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
	};
}
