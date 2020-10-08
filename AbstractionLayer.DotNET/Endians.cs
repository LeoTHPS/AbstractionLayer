namespace AL
{
    public enum Endians
        : uint
    {
        Big,
        Little,

        Machine = ((0xFFFFFFFF & 0x00000001) == 0x00000001) ? Little : Big
    }
}
