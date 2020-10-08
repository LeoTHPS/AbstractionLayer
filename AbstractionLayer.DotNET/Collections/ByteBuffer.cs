using System;
using System.Text;
using System.Runtime.InteropServices;

namespace AL.Collections
{
    public class ByteBuffer
    {
        byte[] buffer;
        uint readPosition = 0;
        uint writePosition = 0;
        
        public Endians Endian { get; private set; }

        public uint Size
        {
            get
            {
                return writePosition;
            }
        }

        public uint Capacity
        {
            get
            {
                return (uint)buffer.Length;
            }
            set
            {
                var newBuffer = new byte[value];
                var newBufferSize = ((int)value >= buffer.Length) ? buffer.Length : (int)value;

                if (readPosition > newBufferSize)
                {

                    readPosition = (uint)newBufferSize;
                }

                if (writePosition > newBufferSize)
                {

                    writePosition = (uint)newBufferSize;
                }

                Buffer.BlockCopy(buffer, 0, newBuffer, 0, newBufferSize);
                buffer = newBuffer;
            }
        }
        
        public uint ReadPosition
        {
            get
            {
                return readPosition;
            }
            set
            {
                if (value > Capacity)
                    value = Capacity;

                readPosition = value;
            }
        }

        public uint WritePosition
        {
            get
            {
                return writePosition;
            }
            set
            {
                if (value > Capacity)
                    value = Capacity;

                writePosition = value;
            }
        }

        public ByteBuffer(Endians endian)
            : this(endian, 0)
        {
        }

        public ByteBuffer(Endians endian, uint capacity)
        {
            Endian = endian;
            buffer = new byte[capacity];
        }

        public ByteBuffer(Endians endian, byte[] buffer)
        {
            Endian = endian;
            this.buffer = buffer;
            writePosition = (uint)buffer.Length;
        }

        public bool PeekEnum<T>(out T value) where T : Enum
        {
            var buffer = new byte[Marshal.SizeOf<T>()];
            
            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = FromEndian<T>(
                Endian,
                buffer
            );

            return true;
        }

        public bool PeekBool(out bool value)
        {
            var buffer = new byte[Marshal.SizeOf<bool>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToBoolean(
                buffer,
                0
            );

            return true;
        }

        public bool PeekInt8(out sbyte value)
        {
            var buffer = new byte[1];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = (sbyte)buffer[0];

            return true;
        }
        public bool PeekInt16(out short value)
        {
            var buffer = new byte[Marshal.SizeOf<short>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt16(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool PeekInt32(out int value)
        {
            var buffer = new byte[Marshal.SizeOf<int>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt32(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool PeekInt64(out long value)
        {
            var buffer = new byte[Marshal.SizeOf<long>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt64(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool PeekUInt8(out byte value)
        {
            var buffer = new byte[1];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = buffer[0];

            return true;
        }
        public bool PeekUInt16(out ushort value)
        {
            var buffer = new byte[Marshal.SizeOf<ushort>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt16(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool PeekUInt32(out uint value)
        {
            var buffer = new byte[Marshal.SizeOf<uint>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt32(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool PeekUInt64(out ulong value)
        {
            var buffer = new byte[Marshal.SizeOf<ulong>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt64(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool PeekFloat(out float value)
        {
            var buffer = new byte[Marshal.SizeOf<float>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToSingle(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool PeekDouble(out double value)
        {
            var buffer = new byte[Marshal.SizeOf<double>()];

            if (!Peek(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToDouble(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool PeekString(out string value)
        {
            if (!PeekUInt32(out uint length))
            {
                value = null;

                return false;
            }

            var buffer = new byte[length];

            if (!Peek(ref buffer, ReadPosition + 4))
            {
                value = null;

                return false;
            }

            value = Encoding.ASCII.GetString(
                buffer
            );

            return true;
        }
        public bool PeekWString(out string value)
        {
            if (!PeekUInt32(out uint length))
            {
                value = null;

                return false;
            }

            var buffer = new byte[length * 2];

            if (!Peek(ref buffer, ReadPosition + 4))
            {
                value = null;

                return false;
            }

            value = Encoding.Unicode.GetString(
                buffer
            );

            return true;
        }

        public bool Peek(ref byte[] buffer)
        {
            return Peek(
                ref buffer,
                ReadPosition
            );
        }
        public bool Peek(ref byte[] buffer, uint index)
        {
            if ((index + buffer.Length) <= Capacity)
            {
                Buffer.BlockCopy(
                    this.buffer,
                    (int)ReadPosition,
                    buffer,
                    0,
                    buffer.Length
                );
                
                return true;
            }

            return false;
        }

        public bool ReadEnum<T>(out T value) where T : Enum
        {
            var buffer = new byte[Marshal.SizeOf<T>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = FromEndian<T>(
                Endian,
                buffer
            );

            return true;
        }

        public bool ReadBool(out bool value)
        {
            var buffer = new byte[Marshal.SizeOf<bool>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToBoolean(
                buffer,
                0
            );

            return true;
        }

        public bool ReadInt8(out sbyte value)
        {
            var buffer = new byte[1];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = (sbyte)buffer[0];

            return true;
        }
        public bool ReadInt16(out short value)
        {
            var buffer = new byte[Marshal.SizeOf<short>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt16(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool ReadInt32(out int value)
        {
            var buffer = new byte[Marshal.SizeOf<int>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt32(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool ReadInt64(out long value)
        {
            var buffer = new byte[Marshal.SizeOf<long>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToInt64(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool ReadUInt8(out byte value)
        {
            var buffer = new byte[1];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = buffer[0];

            return true;
        }
        public bool ReadUInt16(out ushort value)
        {
            var buffer = new byte[Marshal.SizeOf<ushort>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt16(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool ReadUInt32(out uint value)
        {
            var buffer = new byte[Marshal.SizeOf<uint>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt32(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool ReadUInt64(out ulong value)
        {
            var buffer = new byte[Marshal.SizeOf<ulong>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToUInt64(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool ReadFloat(out float value)
        {
            var buffer = new byte[Marshal.SizeOf<float>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToSingle(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }
        public bool ReadDouble(out double value)
        {
            var buffer = new byte[Marshal.SizeOf<double>()];

            if (!Read(ref buffer))
            {
                value = default;

                return false;
            }

            value = BitConverter.ToDouble(
                buffer,
                0
            );

            value = FromEndian(
                Endian,
                value
            );

            return true;
        }

        public bool ReadString(out string value)
        {
            if (!ReadUInt32(out uint length))
            {
                value = null;

                return false;
            }

            var buffer = new byte[length];

            if (!Read(ref buffer))
            {
                value = null;

                return false;
            }

            value = Encoding.ASCII.GetString(
                buffer
            );

            return true;
        }
        public bool ReadWString(out string value)
        {
            if (!ReadUInt32(out uint length))
            {
                value = null;

                return false;
            }

            var buffer = new byte[length * 2];

            if (!Read(ref buffer))
            {
                value = null;

                return false;
            }

            value = Encoding.Unicode.GetString(
                buffer
            );

            return true;
        }

        public bool Read(ref byte[] buffer)
        {
            if ((buffer.Length > 0) && ((ReadPosition + buffer.Length) <= Capacity))
            {
                Buffer.BlockCopy(
                    this.buffer,
                    (int)ReadPosition,
                    buffer,
                    0,
                    buffer.Length
                );
                
                readPosition += (uint)buffer.Length;

                return true;
            }

            return buffer.Length == 0;
        }

        public void WriteEnum<T>(T value) where T : Enum
        {
            value = ToEndian(Endian, value);

            var underlyingType = Enum.GetUnderlyingType(
                typeof(T)
            );
            
            if (underlyingType == typeof(sbyte))
            {
                var buffer = new byte[1];
                buffer[0] = (byte)Convert.ToSByte(value);

                Write(buffer);
            }
            else if (underlyingType == typeof(byte))
            {
                var buffer = new byte[1];
                buffer[0] = Convert.ToByte(value);

                Write(buffer);
            }
            else if (underlyingType == typeof(short))
            {
                var buffer = BitConverter.GetBytes(
                    Convert.ToUInt16(value)
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(ushort))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToUInt16(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(int))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToUInt32(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(uint))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToUInt32(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(long))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToUInt64(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(ulong))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToUInt64(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(float))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToSingle(value))
                );

                Write(buffer);
            }
            else if (underlyingType == typeof(double))
            {
                var buffer = BitConverter.GetBytes(
                    ToEndian(Endian, Convert.ToDouble(value))
                );

                Write(buffer);
            }
            else
            {

                throw new ArgumentException("Unsupported enum base type");
            }
        }

        public void WriteBool(bool value)
        {
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }

        public void WriteInt8(sbyte value)
        {
            var buffer = new byte[1];
            buffer[0] = (byte)value;

            Write(buffer);
        }
        public void WriteInt16(short value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }
        public void WriteInt32(int value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }
        public void WriteInt64(long value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }

        public void WriteUInt8(byte value)
        {
            var buffer = new byte[1];
            buffer[0] = value;

            Write(buffer);
        }
        public void WriteUInt16(ushort value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }
        public void WriteUInt32(uint value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }
        public void WriteUInt64(ulong value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }

        public void WriteFloat(float value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }
        public void WriteDouble(double value)
        {
            value = ToEndian(Endian, value);
            var buffer = BitConverter.GetBytes(value);

            Write(buffer);
        }

        public void WriteString(string value)
        {
            WriteUInt32((uint)value.Length);

            var buffer = Encoding.ASCII.GetBytes(
                value
            );

            Write(buffer);
        }
        public void WriteWString(string value)
        {
            WriteUInt32((uint)value.Length);

            var buffer = Encoding.Unicode.GetBytes(
                value
            );

            Write(buffer);
        }

        public void Write(byte[] buffer)
        {
            if (buffer.Length > 0)
            {
                if ((writePosition += (uint)buffer.Length) >= Capacity)
                {

                    Capacity = writePosition;
                }

                Buffer.BlockCopy(
                    buffer,
                    0,
                    this.buffer,
                    (int)(writePosition - buffer.Length),
                    buffer.Length
                );
            }
        }

        public byte[] ToArray()
        {
            var array = new byte[WritePosition];
            Buffer.BlockCopy(buffer, 0, array, 0, array.Length);

            return array;
        }

        static T ToEndian<T>(Endians endian, byte[] buffer) where T : Enum
        {
            var underlyingType = Enum.GetUnderlyingType(typeof(T));

            if (underlyingType == typeof(short))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToInt16(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ushort))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToUInt16(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(int))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToInt32(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(uint))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToUInt32(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(long))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToInt64(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ulong))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToUInt64(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(float))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToSingle(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(double))
            {
                return (T)Convert.ChangeType(
                    ToEndian(endian, BitConverter.ToDouble(buffer, 0)),
                    typeof(T)
                );
            }

            throw new ArgumentException("Unsupported enum base type");
        }
        public static T ToEndian<T>(Endians endian, T value) where T : Enum
        {
            var underlyingType = Enum.GetUnderlyingType(
                typeof(T)
            );
            
            if (underlyingType == typeof(short))
            {
                var underlyingValue = (short)Convert.ChangeType(
                    value,
                    typeof(short)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(ushort))
            {
                var underlyingValue = (ushort)Convert.ChangeType(
                    value,
                    typeof(ushort)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(int))
            {
                var underlyingValue = (int)Convert.ChangeType(
                    value,
                    typeof(int)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(uint))
            {
                var underlyingValue = (uint)Convert.ChangeType(
                    value,
                    typeof(uint)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(long))
            {
                var underlyingValue = (long)Convert.ChangeType(
                    value,
                    typeof(long)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(ulong))
            {
                var underlyingValue = (ulong)Convert.ChangeType(
                    value,
                    typeof(ulong)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(float))
            {
                var underlyingValue = (float)Convert.ChangeType(
                    value,
                    typeof(float)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }
            else if (underlyingType == typeof(double))
            {
                var underlyingValue = (double)Convert.ChangeType(
                    value,
                    typeof(double)
                );

                return (T)Enum.ToObject(
                    typeof(T),
                    ToEndian(
                        endian,
                        underlyingValue
                    )
                );
            }

            return value;
        }
        public static short ToEndian(Endians endian, short value)
        {
            return (short)ToEndian(endian, (ushort)value);
        }
        public static ushort ToEndian(Endians endian, ushort value)
        {
            if (endian != Endians.Machine)
            {
                return (ushort)(
                    (ushort)((value & (ushort)0x00FFu) << 8) |
                    (ushort)((value & (ushort)0xFF00u) >> 8)
                );
            }

            return value;
        }
        public static int ToEndian(Endians endian, int value)
        {
            return (int)ToEndian(endian, (uint)value);
        }
        public static uint ToEndian(Endians endian, uint value)
        {
            if (endian != Endians.Machine)
            {
                return (
                    ((value & 0xFF000000u) >> 24) |
                    ((value & 0x00FF0000u) >> 8) |
                    ((value & 0x0000FF00u) << 8) |
                    ((value & 0x000000FFu) << 24)
                );
            }

            return value;
        }
        public static long ToEndian(Endians endian, long value)
        {
            return (long)ToEndian(endian, (ulong)value);
        }
        public static ulong ToEndian(Endians endian, ulong value)
        {
            if (endian != Endians.Machine)
            {
                return (
                    ((value & 0xFF00000000000000ul) >> 56) |
                    ((value & 0x00FF000000000000ul) >> 40) |
                    ((value & 0x0000FF0000000000ul) >> 24) |
                    ((value & 0x000000FF00000000ul) >> 8) |
                    ((value & 0x00000000FF000000ul) << 8) |
                    ((value & 0x0000000000FF0000ul) << 24) |
                    ((value & 0x000000000000FF00ul) << 40) |
                    ((value & 0x00000000000000FFul) << 56)
                );
            }

            return value;
        }
        public static float ToEndian(Endians endian, float value)
        {
            if (endian != Endians.Machine)
            {
                var bytes = BitConverter.GetBytes(value);
                bytes[0] += bytes[3];
                bytes[1] += bytes[2];
                bytes[2] -= bytes[1];
                bytes[3] -= bytes[0];

                return BitConverter.ToSingle(bytes, 0);
            }

            return value;
        }
        public static double ToEndian(Endians endian, double value)
        {
            if (endian != Endians.Machine)
            {
                var bytes = BitConverter.GetBytes(value);
                bytes[0] += bytes[7];
                bytes[1] += bytes[6];
                bytes[2] += bytes[5];
                bytes[3] += bytes[4];
                bytes[4] -= bytes[3];
                bytes[5] -= bytes[2];
                bytes[6] -= bytes[1];
                bytes[7] -= bytes[0];

                return BitConverter.ToSingle(bytes, 0);
            }

            return value;
        }
        
        static T FromEndian<T>(Endians endian, byte[] buffer) where T : Enum
        {
            var underlyingType = Enum.GetUnderlyingType(typeof(T));

            if (underlyingType == typeof(short))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToInt16(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ushort))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToUInt16(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(int))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToInt32(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(uint))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToUInt32(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(long))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToInt64(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ulong))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToUInt64(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(float))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToSingle(buffer, 0)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(double))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, BitConverter.ToDouble(buffer, 0)),
                    typeof(T)
                );
            }

            throw new ArgumentException("Unsupported enum base type");
        }
        public static T FromEndian<T>(Endians endian, T value) where T : Enum
        {
            var underlyingType = Enum.GetUnderlyingType(typeof(T));

            if (underlyingType == typeof(short))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToInt16(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ushort))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToUInt16(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(int))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToInt32(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(uint))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToUInt32(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(long))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToInt64(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(ulong))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToUInt64(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(float))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToSingle(value)),
                    typeof(T)
                );
            }
            else if (underlyingType == typeof(double))
            {
                return (T)Convert.ChangeType(
                    FromEndian(endian, Convert.ToDouble(value)),
                    typeof(T)
                );
            }

            return value;
        }
        public static short FromEndian(Endians endian, short value)
        {
            return (short)FromEndian(endian, (ushort)value);
        }
        public static ushort FromEndian(Endians endian, ushort value)
        {
            if (endian != Endians.Machine)
            {
                return (ushort)(
                    (ushort)((value & (ushort)0x00FFu) << 8) |
                    (ushort)((value & (ushort)0xFF00u) >> 8)
                );
            }

            return value;
        }
        public static int FromEndian(Endians endian, int value)
        {
            return (int)FromEndian(endian, (uint)value);
        }
        public static uint FromEndian(Endians endian, uint value)
        {
            if (endian != Endians.Machine)
            {
                return (
                    ((value & 0xFF000000u) >> 24) |
                    ((value & 0x00FF0000u) >> 8) |
                    ((value & 0x0000FF00u) << 8) |
                    ((value & 0x000000FFu) << 24)
                );
            }

            return value;
        }
        public static long FromEndian(Endians endian, long value)
        {
            return (long)FromEndian(endian, (ulong)value);
        }
        public static ulong FromEndian(Endians endian, ulong value)
        {
            if (endian != Endians.Machine)
            {
                return (
                    ((value & 0xFF00000000000000ul) >> 56) |
                    ((value & 0x00FF000000000000ul) >> 40) |
                    ((value & 0x0000FF0000000000ul) >> 24) |
                    ((value & 0x000000FF00000000ul) >> 8) |
                    ((value & 0x00000000FF000000ul) << 8) |
                    ((value & 0x0000000000FF0000ul) << 24) |
                    ((value & 0x000000000000FF00ul) << 40) |
                    ((value & 0x00000000000000FFul) << 56)
                );
            }

            return value;
        }
        public static float FromEndian(Endians endian, float value)
        {
            if (endian != Endians.Machine)
            {
                var bytes = BitConverter.GetBytes(value);
                bytes[0] += bytes[3];
                bytes[1] += bytes[2];
                bytes[2] -= bytes[1];
                bytes[3] -= bytes[0];
                
                return BitConverter.ToSingle(bytes, 0);
            }

            return value;
        }
        public static double FromEndian(Endians endian, double value)
        {
            if (endian != Endians.Machine)
            {
                var bytes = BitConverter.GetBytes(value);
                bytes[0] += bytes[7];
                bytes[1] += bytes[6];
                bytes[2] += bytes[5];
                bytes[3] += bytes[4];
                bytes[4] -= bytes[3];
                bytes[5] -= bytes[2];
                bytes[6] -= bytes[1];
                bytes[7] -= bytes[0];

                return BitConverter.ToSingle(bytes, 0);
            }

            return value;
        }
    }
}
