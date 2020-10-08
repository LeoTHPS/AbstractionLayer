using System;

namespace AL.Algorithms
{
    public class Isaac
    {
        uint[] mm = new uint[256];
        uint aa = 0;
        uint bb = 0;
        uint cc = 0;

        public Isaac()
            : this(0)
        {
        }

        public Isaac(uint seed)
            : this(
                  CreateSeed(seed)
              )
        {
        }

        // Seed must be up to 256 elements long
        public Isaac(uint[] seed)
        {
            const uint GOLDEN_RATIO = 0x9E3779B9;
            
            aa = bb = cc = 0;

            uint a = GOLDEN_RATIO, b = GOLDEN_RATIO,
                c = GOLDEN_RATIO, d = GOLDEN_RATIO,
                e = GOLDEN_RATIO, f = GOLDEN_RATIO,
                g = GOLDEN_RATIO, h = GOLDEN_RATIO;

            mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);
            mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);
            mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);
            mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);

            for (int i = 0; i < 256; i += 8)
            {
                a += seed[i]; b += seed[i + 1];
                c += seed[i + 2]; d += seed[i + 3];
                e += seed[i + 4]; f += seed[i + 5];
                g += seed[i + 6]; h += seed[i + 7];

                mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);

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

                mix(ref a, ref b, ref c, ref d, ref e, ref f, ref g, ref h);

                mm[i] = a; mm[i + 1] = b;
                mm[i + 2] = c; mm[i + 3] = d;
                mm[i + 4] = e; mm[i + 5] = f;
                mm[i + 6] = g; mm[i + 7] = h;
            }

            Next();
        }

        public uint Next()
        {
            cc += 1; bb += cc;

            for (int i = 0; i < 256; ++i)
            {
                var x = mm[i];

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

                uint y = mm[i] = mm[(x >> 2) % 256] + aa + bb;

                bb = mm[(y >> 10) % 256] + x;
            }

            return bb;
        }
        public uint Next(uint min, uint max)
        {
            if ((min == uint.MinValue) && (max == uint.MaxValue))
                return Next();

            return min + (Next() % (max - min + 1));
        }

        static uint[] CreateSeed(uint seed)
        {
            var random = new Random(
                (int)seed
            );

            var _seed = new uint[256];

            for (int i = 0; i < _seed.Length; ++i)
            {
                _seed[i] = (uint)random.Next();
            }

            return _seed;
        }

        static void mix(ref uint a, ref uint b, ref uint c, ref uint d, ref uint e, ref uint f, ref uint g, ref uint h)
        {
            a ^= b << 11;
            d += a;
            b += c;
            b ^= c >> 2;
            e += b;
            c += d;
            c ^= d << 8;
            f += c;
            d += e;
            d ^= e >> 16;
            g += d;
            e += f;
            e ^= f << 10;
            h += e;
            f += g;
            f ^= g >> 4;
            a += f;
            g += h;
            g ^= h << 8;
            b += g;
            h += a;
            h ^= a >> 9;
            c += h;
            a += b;
        }
    }

    public class Isaac64
        : Isaac
    {
        public new ulong Next()
        {
            return base.Next();
        }
        public ulong Next(ulong min, ulong max)
        {
            var diff = max - min;

            ulong rLow = base.Next(
                0,
                0xFFFFFFFF
            );
            ulong rHigh = base.Next(
                0,
                0xFFFFFFFF
            );
            ulong vLow = diff & 0xFFFFFFFF;
            ulong vHigh = diff >> 32;

            ulong value = (rHigh * vLow) >> 32;
            value += (rLow * vHigh) >> 32;
            value += rHigh * vHigh;
            value += min;

            return value;
        }
    }
}
