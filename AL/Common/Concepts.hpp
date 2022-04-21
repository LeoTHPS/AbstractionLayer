#pragma once
#include "AL/Common.hpp"

namespace AL
{
	template<typename T>
	concept Is_POD_V = Is_POD<T>::Value;

	template<typename T>
	concept Is_Array_V = Is_Array<T>::Value;

	template<typename T_1, typename T_2>
	concept Is_Type_V = Is_Type<T_1, T_2>::Value;

	template<typename T>
	concept Is_Enum_V = Is_Enum<T>::Value;

	template<typename T>
	concept Is_Integer_V = Is_Integer<T>::Value;

	template<typename T>
	concept Is_Enum_Or_Integer_V = Is_Enum_Or_Integer<T>::Value;

	template<typename T>
	concept Is_Float_V = Is_Float<T>::Value;

	template<typename T>
	concept Is_Double_V = Is_Double<T>::Value;

	template<typename T>
	concept Is_Decimal_V = Is_Decimal<T>::Value;

	template<typename T>
	concept Is_Signed_V = Is_Signed<T>::Value;

	template<typename T>
	concept Is_Unsigned_V = Is_Unsigned<T>::Value;

	template<typename T>
	concept Is_Const_V = Is_Const<T>::Value;

	template<typename T>
	concept Is_Pointer_V = Is_Pointer<T>::Value;

	template<typename T>
	concept Is_Reference_V = Is_Reference<T>::Value;

	template<typename BASE, typename T>
	concept Is_Base_Of_V = Is_Base_Of<BASE, T>::Value;

	template<typename TYPE, typename TYPE2>
	concept Is_Convertible_V = Is_Convertible<TYPE, TYPE2>::Value;

	template<typename T>
	concept Is_Copy_Constructible_V = Is_Copy_Constructible<T>::Value;

	template<typename T>
	concept Is_Copy_Assignable_V = Is_Copy_Assignable<T>::Value;

	template<typename T>
	concept Is_Move_Constructible_V = Is_Move_Constructible<T>::Value;

	template<typename T>
	concept Is_Move_Assignable_V = Is_Move_Assignable<T>::Value;
}
