#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"
#include "String.hpp"

#define AL_ENCRYPTED_STRING(__value__)					AL_ENCRYPTED_STRING_EX(__value__, 0x45)
#define AL_ENCRYPTED_STRING_EX(__value__, __seed__)		AL::Collections::EncryptedString<__seed__, (sizeof(__value__) / sizeof(AL::String::Char)) - 1>(__value__).Decrypt()

#define AL_ENCRYPTED_WSTRING(__value__)					AL_ENCRYPTED_WSTRING_EX(__value__, 0x45)
#define AL_ENCRYPTED_WSTRING_EX(__value__, __seed__)	AL::Collections::EncryptedWString<__seed__, (sizeof(__value__) / sizeof(AL::WString::Char)) - 1>(__value__).Decrypt()

namespace AL::Collections
{
	template<typename T_CHAR, uint8 SEED, size_t LENGTH>
	class _EncryptedString
	{
		const T_CHAR buffer[LENGTH];

		template<size_t ... INDEXES>
		constexpr _EncryptedString(const T_CHAR(&value)[LENGTH + 1], Index_Sequence<INDEXES ...>)
			: buffer {
				T_CHAR((value[INDEXES] + static_cast<Char>(INDEXES)) ^ SEED) ...,
			}
		{
		}

		_EncryptedString(_EncryptedString&&) = delete;
		_EncryptedString(const _EncryptedString&) = delete;

	public:
		typedef T_CHAR        Char;
		typedef _String<Char> String;

		explicit constexpr _EncryptedString(const Char(&value)[LENGTH + 1])
			: _EncryptedString(
				value,
				typename Make_Index_Sequence<LENGTH>::Type {}
			)
		{
		}

		virtual ~_EncryptedString()
		{
		}

		auto Decrypt() const
		{
			Array<Char> buffer(
				LENGTH
			);

			auto lpBuffer = &this->buffer[0];
			auto lpDecodedBuffer = &buffer[0];

			for (size_t i = 0; i < LENGTH; ++i, ++lpBuffer, ++lpDecodedBuffer)
			{
				*lpDecodedBuffer = (*lpBuffer ^ SEED) - static_cast<Char>(i);
			}

			return String(
				&buffer[0],
				LENGTH
			);
		}
	};

	template<uint8 SEED, size_t LENGTH>
	using EncryptedString = _EncryptedString<String::Char, SEED, LENGTH>;

	template<uint8 SEED, size_t LENGTH>
	using EncryptedWString = _EncryptedString<WString::Char, SEED, LENGTH>;
}
