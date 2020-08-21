#pragma once
#include "AL/Common.hpp"

namespace AL::Algorithms
{
	class Sha1
	{
		enum
		{
			shaSuccess = 0,
			shaNull,
			shaInputTooLong,
			shaStateError
		};

		int computed;
		int corrupted;
		uint32 lengthLow;
		uint32 lengthHigh;
		uint8 messageBlock[64];
		uint16 messageBlockIndex;
		uint32 intermediateHash[5];

	public:
		typedef Collections::Array<uint8[20]> Hash;

		Sha1()
		{
			Reset();
		}

		void Reset()
		{
			computed = 0;
			corrupted = 0;

			lengthLow = 0;
			lengthHigh = 0;

			messageBlockIndex = 0;

			intermediateHash[0] = 0x67452301;
			intermediateHash[1] = 0xEFCDAB89;
			intermediateHash[2] = 0x98BADCFE;
			intermediateHash[3] = 0x10325476;
			intermediateHash[4] = 0xC3D2E1F0;
		}

		bool Add(const void* lpBuffer, size_t size)
		{
			if (!lpBuffer || !size)
				return false;

			if (computed)
			{
				corrupted = shaStateError;

				return false;
			}

			if (corrupted)
			{

				return false;
			}

			const uint8* _lpBuffer = reinterpret_cast<const uint8*>(lpBuffer);

			while (size-- && !corrupted)
			{
				messageBlock[messageBlockIndex++] = *_lpBuffer & 0xFF;

				lengthLow += 8;

				if (lengthLow == 0)
				{
					lengthHigh++;

					if (lengthHigh == 0)
					{

						corrupted = 1;
					}
				}

				if (messageBlockIndex == 64)
				{

					ProcessMessageBlock();
				}

				++_lpBuffer;
			}

			return true;
		}

		bool Calculate(Hash& result)
		{
			int i;

			if (corrupted)
			{

				return false;
			}

			if (!computed)
			{
				PadMessage();

				memset(
					&result[0],
					0,
					result.GetSize()
				);

				for (i = 0; i < 64; ++i)
				{

					messageBlock[i] = 0;
				}

				computed = 1;
				lengthLow = 0;
				lengthHigh = 0;
			}

			for (i = 0; i < 20; ++i)
			{
				result[i] = static_cast<uint8>(
					intermediateHash[i >> 2] >> 8 * (3 - (i & 0x03))
				);
			}

			return true;
		}
		bool Calculate(String& result)
		{
			Hash _result;

			if (!Calculate(_result))
			{

				return false;
			}

			result = HexConverter::Encode(
				&_result[0],
				_result.GetSize() * sizeof(Hash::Type)
			);

			return true;
		}

		static bool Calculate(Hash& result, const void* lpBuffer, size_t size)
		{
			Sha1 sha;

			return sha.Add(lpBuffer, size) && sha.Calculate(result);
		}
		static bool Calculate(String& result, const void* lpBuffer, size_t size)
		{
			Sha1 sha;

			return sha.Add(lpBuffer, size) && sha.Calculate(result);
		}

	private:
		void PadMessage()
		{
			if (messageBlockIndex > 55)
			{
				messageBlock[messageBlockIndex++] = 0x80;

				while (messageBlockIndex < 64)
					messageBlock[messageBlockIndex++] = 0;

				ProcessMessageBlock();

				while (messageBlockIndex < 56)
					messageBlock[messageBlockIndex++] = 0;
			}
			else
			{
				messageBlock[messageBlockIndex++] = 0x80;

				while (messageBlockIndex < 56)
					messageBlock[messageBlockIndex++] = 0;
			}

			messageBlock[56] = static_cast<uint8>(lengthHigh >> 24);
			messageBlock[57] = static_cast<uint8>(lengthHigh >> 16);
			messageBlock[58] = static_cast<uint8>(lengthHigh >> 8);
			messageBlock[59] = static_cast<uint8>(lengthHigh);
			messageBlock[60] = static_cast<uint8>(lengthLow >> 24);
			messageBlock[61] = static_cast<uint8>(lengthLow >> 16);
			messageBlock[62] = static_cast<uint8>(lengthLow >> 8);
			messageBlock[63] = static_cast<uint8>(lengthLow);

			ProcessMessageBlock();
		}

		void ProcessMessageBlock()
		{
			const uint32_t K[] =
			{
				0x5A827999,
				0x6ED9EBA1,
				0x8F1BBCDC,
				0xCA62C1D6
			};

			int t;
			uint32_t temp;
			uint32_t W[80];
			uint32_t A, B, C, D, E;

			for (t = 0; t < 16; t++)
			{
				W[t] = messageBlock[t * 4] << 24;
				W[t] |= messageBlock[t * 4 + 1] << 16;
				W[t] |= messageBlock[t * 4 + 2] << 8;
				W[t] |= messageBlock[t * 4 + 3];
			}

			for (t = 16; t < 80; t++)
			{
				W[t] = CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
			}

			A = intermediateHash[0];
			B = intermediateHash[1];
			C = intermediateHash[2];
			D = intermediateHash[3];
			E = intermediateHash[4];

			for (t = 0; t < 20; t++)
			{
				temp = CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];

				E = D;
				D = C;
				C = CircularShift(30, B);
				B = A;
				A = temp;
			}

			for (t = 20; t < 40; t++)
			{
				temp = CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];

				E = D;
				D = C;
				C = CircularShift(30, B);
				B = A;
				A = temp;
			}

			for (t = 40; t < 60; t++)
			{
				temp = CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];

				E = D;
				D = C;
				C = CircularShift(30, B);
				B = A;
				A = temp;
			}

			for (t = 60; t < 80; t++)
			{
				temp = CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];

				E = D;
				D = C;
				C = CircularShift(30, B);
				B = A;
				A = temp;
			}

			messageBlockIndex = 0;

			intermediateHash[0] += A;
			intermediateHash[1] += B;
			intermediateHash[2] += C;
			intermediateHash[3] += D;
			intermediateHash[4] += E;
		}

		static constexpr uint32 CircularShift(uint32 bits, uint32 word)
		{
			return (word << bits) | (word >> (32 - bits));
		}
	};
}
