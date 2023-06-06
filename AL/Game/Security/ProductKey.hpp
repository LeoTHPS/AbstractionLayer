#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/EncryptedString.hpp"

namespace AL::Game::Security
{
	// [S] Seed     48 bits     6 bytes     12 chars
	// [C] Check    24 bits     3 bytes     6  chars
	// [R] Random   8  bits     1 byte      2  chars
	// SCCS-RSSS-SSSR-SCCS-CCSS
	template<typename T_ALGORITHM>
	struct ProductKey
	{
		typedef T_ALGORITHM Algorithm;

		uint64 Seed   : 48;
		uint32 Check  : 24;
		uint8  Random : 8;

		static Bool FromString(ProductKey& key, const String& string)
		{
			if (!Regex::IsMatch(AL_ENCRYPTED_STRING("[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}"), string))
			{

				return False;
			}

			{
				String seed(
					'0',
					16
				);

				seed[4]  = string[0];
				seed[5]  = string[3];
				seed[6]  = string[6];
				seed[7]  = string[7];
				seed[8]  = string[8];
				seed[9]  = string[11];
				seed[10] = string[12];
				seed[11] = string[13];
				seed[12] = string[16];
				seed[13] = string[19];
				seed[14] = string[23];
				seed[15] = string[24];

				key.Seed = HexConverter::Decode<uint64>(
					seed
				);
			}

			{
				String check(
					'0',
					8
				);

				check[2] = string[1];
				check[3] = string[2];
				check[4] = string[17];
				check[5] = string[18];
				check[6] = string[21];
				check[7] = string[22];

				key.Check = HexConverter::Decode<uint32>(
					check
				);
			}

			{
				String random(
					'0',
					2
				);

				random[0] = string[5];
				random[1] = string[14];

				key.Random = HexConverter::Decode<uint8>(
					random
				);
			}

			return True;
		}

		ProductKey()
			: ProductKey(
				0,
				0,
				0
			)
		{
		}

		ProductKey(uint64 seed, uint32 check, uint8 random)
			: Seed(
				seed
			),
			Check(
				check
			),
			Random(
				random
			)
		{
		}

		Bool IsValid() const
		{
			if (Algorithm::Calculate(Seed) != Check)
			{

				return False;
			}

			return True;
		}

		String ToString() const
		{
			auto seed = String::Format(
				"%012X",
				Seed
			);

			auto check = String::Format(
				"%06X",
				Check
			);

			auto random = String::Format(
				"%02X",
				Random
			);

			String string(
				AL_ENCRYPTED_STRING("0000-0000-0000-0000-0000")
			);

			string[0]  = seed[0];
			string[1]  = check[0];
			string[2]  = check[1];
			string[3]  = seed[1];
			string[5]  = random[0];
			string[6]  = seed[2];
			string[7]  = seed[3];
			string[8]  = seed[4];
			string[10] = seed[5];
			string[11] = seed[6];
			string[12] = seed[7];
			string[13] = random[1];
			string[15] = seed[8];
			string[16] = check[2];
			string[17] = check[3];
			string[18] = seed[9];
			string[20] = check[4];
			string[21] = check[5];
			string[22] = seed[10];
			string[23] = seed[11];

			return string;
		}
	};
}
