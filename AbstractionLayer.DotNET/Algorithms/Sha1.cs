using System;

namespace AL.Algorithms
{
    public class Sha1
    {
        const int shaSuccess = 0;
        const int shaNull = 1;
        const int shaInputTooLong = 2;
        const int shaStateError = 3;

        int computed;
        int corrupted;
        uint lengthLow;
        uint lengthHigh;
        byte[] messageBlock = new byte[64];
        ushort messageBlockIndex;
        uint[] intermediateHash = new uint[5];

        public Sha1()
        {
            Reset();
        }

        public void Reset()
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

        public bool Add(byte[] value)
        {
            if ((value == null) || (value.Length == 0))
                return false;

            if (computed != 0)
            {
                corrupted = shaStateError;

                return false;
            }

            if (corrupted != 0)
            {

                return false;
            }
            
            for (int i = 0, size = value.Length; (size-- != 0) && (corrupted == 0); ++i)
            {
                messageBlock[messageBlockIndex++] = (byte)(value[i] & 0xFF);

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
            }

            return true;
        }

        public bool Calculate(out byte[] result)
        {
            int i;

            if (corrupted != 0)
            {
                result = null;

                return false;
            }

            result = new byte[20];

            if (computed == 0)
            {
                PadMessage();
                
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
                result[i] = (byte)(
                    intermediateHash[i >> 2] >> 8 * (3 - (i & 0x03))
                );
            }

            return true;
        }
        public bool Calculate(out string result)
        {
            if (Calculate(out byte[] _result))
            {
                result = BitConverter.ToString(_result)
                    .Replace("-", "");

                return true;
            }

            result = null;

            return false;
        }

        public static bool Calculate(byte[] value, out byte[] result)
        {
            var sha1 = new Sha1();

            if (!sha1.Add(value))
            {
                result = null;

                return false;
            }

            return sha1.Calculate(
                out result
            );
        }
        public static bool Calculate(byte[] value, out string result)
        {
            var sha1 = new Sha1();

            if (!sha1.Add(value))
            {
                result = null;

                return false;
            }

            return sha1.Calculate(
                out result
            );
        }

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

            messageBlock[56] = (byte)(lengthHigh >> 24);
            messageBlock[57] = (byte)(lengthHigh >> 16);
            messageBlock[58] = (byte)(lengthHigh >> 8);
            messageBlock[59] = (byte)(lengthHigh);
            messageBlock[60] = (byte)(lengthLow >> 24);
            messageBlock[61] = (byte)(lengthLow >> 16);
            messageBlock[62] = (byte)(lengthLow >> 8);
            messageBlock[63] = (byte)(lengthLow);

            ProcessMessageBlock();
        }

        void ProcessMessageBlock()
        {
            var K = new uint[]
            {
                0x5A827999,
                0x6ED9EBA1,
                0x8F1BBCDC,
                0xCA62C1D6
            };

            int t;
            uint temp;
            var W = new uint[80];
            uint A, B, C, D, E;

            for (t = 0; t < 16; t++)
            {
                W[t] = (uint)(messageBlock[t * 4] << 24);
                W[t] |= (uint)(messageBlock[t * 4 + 1] << 16);
                W[t] |= (uint)(messageBlock[t * 4 + 2] << 8);
                W[t] |= (uint)(messageBlock[t * 4 + 3]);
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

        static uint CircularShift(uint bits, uint word)
        {
            return (uint)(((int)word << (int)bits) | ((int)word >> (int)(32 - bits)));
        }
    }
}
