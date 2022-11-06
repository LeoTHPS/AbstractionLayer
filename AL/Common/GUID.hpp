#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"

namespace AL
{
	template<size_t S>
	struct __Get_GUID_Container_Type;
	template<>
	struct __Get_GUID_Container_Type<1>
	{
		typedef uint8 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<2>
	{
		typedef uint16 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<3>
	{
		typedef uint32 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<4>
	{
		typedef uint32 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<5>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<6>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<7>
	{
		typedef uint64 Type;
	};
	template<>
	struct __Get_GUID_Container_Type<8>
	{
		typedef uint64 Type;
	};

	template<size_t SIZE, typename ... T_FIELDS>
	struct __Get_GUID_Container_Size;
	template<size_t SIZE>
	struct __Get_GUID_Container_Size<SIZE>
	{
		static constexpr size_t Value = SIZE;
	};
	template<size_t SIZE, typename T_FIELD, typename ... T_FIELDS>
	struct __Get_GUID_Container_Size<SIZE, T_FIELD, T_FIELDS ...>
		: public __Get_GUID_Container_Size<SIZE + sizeof(T_FIELD), T_FIELDS ...>
	{
	};

	template<typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Container_Size = __Get_GUID_Container_Size<sizeof(T_COUNTER), T_FIELDS ...>;

	template<typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Container_Type = __Get_GUID_Container_Type<_Get_GUID_Container_Size<T_COUNTER, T_FIELDS ...>::Value>;

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Mask
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;
		typedef typename Get_Type_Sequence<I, T_FIELDS ..., T_COUNTER>::Type     TypeField;
		typedef typename Get_Enum_Or_Integer_Base<TypeField>::Type               TypeFieldBase;

		static constexpr Type Value = TypeFieldBase(~0);
	};

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Bit_Count
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;
		typedef typename Get_Type_Sequence<I, T_FIELDS ..., T_COUNTER>::Type     TypeField;

		static constexpr Type Value = sizeof(TypeField) * 8;
	};

	template<size_t I, size_t I_END, size_t BIT_COUNT, typename T_COUNTER, typename ... T_FIELDS>
	struct __Get_GUID_Fields_Bit_Count
		: public __Get_GUID_Fields_Bit_Count<I + 1, I_END, BIT_COUNT + _Get_GUID_Field_Bit_Count<I, T_COUNTER, T_FIELDS ...>::Value, T_COUNTER, T_FIELDS ...>
	{
	};
	template<size_t I_END, size_t BIT_COUNT, typename T_COUNTER, typename ... T_FIELDS>
	struct __Get_GUID_Fields_Bit_Count<I_END, I_END, BIT_COUNT, T_COUNTER, T_FIELDS ...>
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = static_cast<Type>(
			BIT_COUNT + _Get_GUID_Field_Bit_Count<I_END, T_COUNTER, T_FIELDS ...>::Value
		);
	};

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	using _Get_GUID_Fields_Bit_Count = __Get_GUID_Fields_Bit_Count<0, I, 0, T_COUNTER, T_FIELDS ...>;

	template<size_t I, typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Shift_Count
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = _Get_GUID_Fields_Bit_Count<sizeof ...(T_FIELDS), T_COUNTER, T_FIELDS ...>::Value - _Get_GUID_Fields_Bit_Count<I, T_COUNTER, T_FIELDS ...>::Value;
	};
	template<typename T_COUNTER, typename ... T_FIELDS>
	struct _Get_GUID_Field_Shift_Count<0, T_COUNTER, T_FIELDS ...>
	{
		typedef typename _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...>::Type Type;

		static constexpr Type Value = _Get_GUID_Fields_Bit_Count<sizeof ...(T_FIELDS), T_COUNTER, T_FIELDS ...>::Value - _Get_GUID_Field_Bit_Count<0, T_COUNTER, T_FIELDS ...>::Value;
	};

	template<typename T_COUNTER, typename ... T_FIELDS>
	class GUID
	{
		typedef _Get_GUID_Container_Size<T_COUNTER, T_FIELDS ...> Get_GUID_Container_Size;
		typedef _Get_GUID_Container_Type<T_COUNTER, T_FIELDS ...> Get_GUID_Container_Type;

		template<size_t I>
		using Get_GUID_Field_Mask = _Get_GUID_Field_Mask<I, T_COUNTER, T_FIELDS ...>;

		template<size_t I>
		using Get_GUID_Field_Shift_Count = _Get_GUID_Field_Shift_Count<I, T_COUNTER, T_FIELDS ...>;

		Collections::Tuple<T_FIELDS ...> fields;
		T_COUNTER                        counter;

		static constexpr size_t FIELD_INDEX_COUNTER = sizeof ...(T_FIELDS);

		template<size_t ... INDEXES>
		GUID(typename Get_GUID_Container_Type::Type value, Index_Sequence<INDEXES ...>) 
			: fields(
				GetFieldFromContainer<INDEXES>(value) ...
			),
			counter(
				(value >> Get_GUID_Field_Shift_Count<FIELD_INDEX_COUNTER>::Value) & Get_GUID_Field_Mask<FIELD_INDEX_COUNTER>::Value
			)
		{
		}

	public:
		typedef Type_Sequence<T_FIELDS ...>            Fields;
		typedef T_COUNTER                              Counter;

		typedef typename Get_GUID_Container_Type::Type Container;

		GUID()
			: GUID(
				0
			)
		{
		}

		GUID(Container value)
			: GUID(
				value,
				typename Make_Index_Sequence<sizeof ...(T_FIELDS)>::Type{}
			)
		{
		}

		GUID(Counter counter, T_FIELDS ... fields)
			: fields(
				Forward<T_FIELDS>(fields) ...
			),
			counter(
				counter
			)
		{
		}

		virtual ~GUID()
		{
		}

		template<size_t I>
		auto GetField() const
		{
			return fields.template Get<I>();
		}

		static constexpr size_t GetFieldCount()
		{
			return sizeof ...(T_FIELDS);
		}

		Counter GetCounter() const
		{
			return counter;
		}

		Container GetValue() const
		{
			auto fields = GetFieldsAsContainer(
				typename Make_Index_Sequence<GetFieldCount()>::Type{}
			);

			auto counter = static_cast<Container>(
				GetCounter()
			);

			return fields | (counter << Get_GUID_Field_Shift_Count<FIELD_INDEX_COUNTER>::Value);
		}

		template<size_t I>
		Void SetField(typename Get_Type_Sequence<I, Fields>::Type value)
		{
			fields.template Set<I>(
				value
			);
		}

		operator Container () const
		{
			return GetValue();
		}

		Bool operator == (const GUID& id) const
		{
			if (GetCounter() != id.GetCounter())
			{

				return False;
			}

			typedef typename Make_Index_Sequence<GetFieldCount()>::Type Field_Count_Index_Sequence;

			if (GetFieldsAsContainer(Field_Count_Index_Sequence{}) != id.GetFieldsAsContainer(Field_Count_Index_Sequence{}))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const GUID& id) const
		{
			if (operator==(id))
			{

				return False;
			}

			return True;
		}

	private:
		template<size_t ... INDEXES>
		constexpr Container GetFieldsAsContainer(Index_Sequence<INDEXES ...>) const
		{
			return ((static_cast<Container>(GetField<INDEXES>()) << Get_GUID_Field_Shift_Count<INDEXES>::Value) | ...);
		}

		template<size_t I, typename T_FIELD = typename Get_Type_Sequence<I, T_FIELDS ...>::Type>
		static constexpr T_FIELD GetFieldFromContainer(Container value)
		{
			return static_cast<T_FIELD>(
				(value >> Get_GUID_Field_Shift_Count<I>::Value) & Get_GUID_Field_Mask<I>::Value
			);
		}
	};
}
