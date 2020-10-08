namespace AL.Algorithms
{
    public static class FNV32
    {
        const uint Prime = 0x1000193;
        const uint Offset = 0x811C9DC5;
        
        public static uint Calculate(byte[] value)
        {
            var hash = Offset;

            foreach (var @byte in value)
            {
                hash ^= @byte;
                hash *= Prime;
            }

            return hash;
        }
        public static uint Calculate(string value)
        {
            var hash = Offset;

            foreach (var @char in value)
            {
                hash ^= @char;
                hash *= Prime;
            }
            
            return hash;
        }
    }

    public static class FNV64
    {
        const ulong Prime = 0x100000001B3;
        const ulong Offset = 0xCBF29CE484222325;

        public static ulong Calculate(byte[] value)
        {
            var hash = Offset;

            foreach (var @byte in value)
            {
                hash ^= @byte;
                hash *= Prime;
            }

            return hash;
        }
        public static ulong Calculate(string value)
        {
            var hash = Offset;

            foreach (var @char in value)
            {
                hash ^= @char;
                hash *= Prime;
            }

            return hash;
        }
    }
}
