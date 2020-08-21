#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Algorithms
{
	class Isaac
	{
		uint32 mm[256];
		uint32 aa = 0;
		uint32 bb = 0;
		uint32 cc = 0;

	public:
		typedef Collections::Array<uint32[256]> Seed;

		Isaac()
			: Isaac(0)
		{
		}

		explicit Isaac(uint32 seed)
			: Isaac(
				[](uint32 _seed)
				{
					Seed __seed;

					srand(_seed);

					for (size_t i = 0; i < __seed.GetSize(); i++)
					{
						__seed[i] = rand();
					}

					return __seed;
				}(seed)
			)
		{
		}

		explicit Isaac(const Seed& seed)
		{
			static constexpr uint32 GOLDEN_RATIO = 0x9E3779B9;

			auto mix = [](uint32& _a, uint32& _b, uint32& _c, uint32& _d, uint32& _e, uint32& _f, uint32& _g, uint32& _h)
			{
				_a ^= _b << 11;
				_d += _a;
				_b += _c;
				_b ^= _c >> 2;
				_e += _b;
				_c += _d;
				_c ^= _d << 8;
				_f += _c;
				_d += _e;
				_d ^= _e >> 16;
				_g += _d;
				_e += _f;
				_e ^= _f << 10;
				_h += _e;
				_f += _g;
				_f ^= _g >> 4;
				_a += _f;
				_g += _h;
				_g ^= _h << 8;
				_b += _g;
				_h += _a;
				_h ^= _a >> 9;
				_c += _h;
				_a += _b;
			};

			aa = bb = cc = 0;

			uint32 a = GOLDEN_RATIO, b = GOLDEN_RATIO,
				c = GOLDEN_RATIO, d = GOLDEN_RATIO,
				e = GOLDEN_RATIO, f = GOLDEN_RATIO,
				g = GOLDEN_RATIO, h = GOLDEN_RATIO;

			mix(a, b, c, d, e, f, g, h);
			mix(a, b, c, d, e, f, g, h);
			mix(a, b, c, d, e, f, g, h);
			mix(a, b, c, d, e, f, g, h);

			for (int i = 0; i < 256; i += 8)
			{
				a += seed[i]; b += seed[i + 1];
				c += seed[i + 2]; d += seed[i + 3];
				e += seed[i + 4]; f += seed[i + 5];
				g += seed[i + 6]; h += seed[i + 7];

				mix(a, b, c, d, e, f, g, h);

				mm[i] = a; mm[i + 1] = b;
				mm[i + 2] = c; mm[i + 3] = d;
				mm[i + 4] = e; mm[i + 5] = f;
				mm[i + 6] = g; mm[i + 7] = h;
			}

			for (int i = 0; i < 256; i += 8)
			{
				a += mm[i]; b += mm[i + 1];
				c += mm[i + 2]; d += mm[i + 3];
				e += mm[i + 4]; f += mm[i + 5];
				g += mm[i + 6]; h += mm[i + 7];

				mix(a, b, c, d, e, f, g, h);

				mm[i] = a; mm[i + 1] = b;
				mm[i + 2] = c; mm[i + 3] = d;
				mm[i + 4] = e; mm[i + 5] = f;
				mm[i + 6] = g; mm[i + 7] = h;
			}

			Next();
		}

		uint32 Next()
		{
			cc += 1; bb += cc;

			for (int i = 0; i < 256; ++i)
			{
				auto x = mm[i];

				switch (i % 4)
				{
					case 0:
						aa ^= aa << 13;
						break;

					case 1:
						aa ^= aa >> 6;
						break;

					case 2:
						aa ^= aa << 2;
						break;

					case 3:
						aa ^= aa >> 16;
						break;
				}

				aa = mm[(i + 128) % 256] + aa;

				uint32 y = mm[i] = mm[(x >> 2) % 256] + aa + bb;

				bb = mm[(y >> 10) % 256] + x;
			}

			return bb;
		}
		uint32 Next(uint32 min, uint32 max)
		{
			if ((min == 0) && (max == ~0))
				return Next();

			return min + (Next() % (max - min + 1));
		}
	};

	class Isaac64
		: public Isaac
	{
	public:
		using Isaac::Isaac;

		uint64 Next()
		{
			return Isaac::Next();
		}
		uint64 Next(uint64 min, uint64 max)
		{
			auto diff = max - min;

			uint64 rLow = Isaac::Next(
				0,
				0xFFFFFFFF
			);
			uint64 rHigh = Isaac::Next(
				0,
				0xFFFFFFFF
			);
			uint64 vLow = diff & 0xFFFFFFFF;
			uint64 vHigh = diff >> 32;

			uint64 value = (rHigh * vLow) >> 32;
			value += (rLow * vHigh) >> 32;
			value += rHigh * vHigh;
			value += min;

			return value;
		}
	};
}
