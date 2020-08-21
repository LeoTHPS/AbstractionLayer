#pragma once
#include "AL/Common.hpp"

#include "String.hpp"

#define AL_ENCRYPTED_STRING(__value__)					AL_ENCRYPTED_STRING_EX(__value__, 0x45)
#define AL_ENCRYPTED_STRING_EX(__value__, __seed__)		AL::Collections::EncryptedString<__seed__, typename AL::Make_Index_Sequence<(sizeof(__value__) / sizeof(char)) - 1>::Type>(__value__).Decrypt()

#define AL_ENCRYPTED_WSTRING(__value__)					AL_ENCRYPTED_WSTRING_EX(__value__, 0x45)
#define AL_ENCRYPTED_WSTRING_EX(__value__, __seed__)	AL::Collections::EncryptedWString<__seed__, typename AL::Make_Index_Sequence<(sizeof(__value__) / sizeof(wchar_t)) - 1>::Type>(__value__).Decrypt()

namespace AL::Collections
{
	template<typename CHAR, uint8 SEED, typename INDEXES>
	class _EncryptedString;

	template<typename CHAR, uint8 SEED, size_t ... INDEXES>
	class _EncryptedString<CHAR, SEED, Index_Sequence<INDEXES ...>>
	{
		template<typename CHAR, uint8 SEED>
		struct Converter
		{
			template<size_t INDEX>
			static constexpr CHAR Encode(CHAR c)
			{
				return (c + static_cast<CHAR>(INDEX)) ^ SEED;
			}

			static constexpr CHAR Decode(CHAR c, size_t index)
			{
				return (c ^ SEED) - static_cast<CHAR>(index);
			}
		};

		static constexpr size_t LENGTH = sizeof ...(INDEXES);

		const CHAR value[LENGTH];

		_EncryptedString(_EncryptedString&&) = delete;
		_EncryptedString(const _EncryptedString&) = delete;

	public:
		typedef _String<CHAR> String;

		constexpr _EncryptedString(const CHAR(&value)[LENGTH + 1])
			: value{ Converter<CHAR, SEED>::Encode<INDEXES>(value[INDEXES]) ... }
		{
		}

		String Decrypt() const
		{
			String string(
				&value[0],
				LENGTH
			);

			for (size_t i = 0; i < LENGTH; ++i)
			{
				string[i] = Converter<CHAR, SEED>::Decode(
					string[i],
					i
				);
			}

			return string;
		}
	};

	template<uint8 SEED, typename INDEXES>
	using EncryptedString = _EncryptedString<char, SEED, INDEXES>;

	template<uint8 SEED, typename INDEXES>
	using EncryptedWString = _EncryptedString<wchar_t, SEED, INDEXES>;
}
