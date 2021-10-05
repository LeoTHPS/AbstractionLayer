#pragma once
#include "AL/Common.hpp"

#include "InputDeviceState.hpp"

namespace AL::Hardware
{
	enum class Keys
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,

		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		Tilde,

		Alt,
		LAlt,
		RAlt,

		Clear,
		Delete,
		Enter,
		Escape,
		Backspace,

		Control,
		LControl,
		RControl,

		Shift,
		LShift,
		RShift,

		Spacebar,
		Tab,

		Up,
		Down,
		Left,
		Right,

		COUNT
	};

	enum class KeyboardEvents
	{
		Char,
		KeyUp,
		KeyDown
	};

	struct KeyboardEvent
	{
		KeyboardEvents Event;

		union
		{
			Keys         Key;
			String::Char Char;
		};
	};

	typedef InputDeviceState<Keys, static_cast<size_t>(Keys::COUNT)> KeyboardState;
}
