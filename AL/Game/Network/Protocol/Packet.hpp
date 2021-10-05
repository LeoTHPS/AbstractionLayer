#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/ByteBuffer.hpp"

namespace AL::Game::Network::Protocol
{
#pragma pack(push, 1)
	template<typename T_OPCODE>
	struct PacketHeader
	{
		T_OPCODE OPCode;
		uint16   DataSize;
	};
#pragma pack(pop)

	template<typename T_OPCODE, Endians ENDIAN>
	class Packet
	{
		typedef Collections::ByteBuffer<ENDIAN> ByteBuffer;

		PacketHeader<T_OPCODE> header;
		ByteBuffer             buffer;

		Packet(const Packet&) = delete;

	public:
		typedef T_OPCODE             OPCode;
		typedef PacketHeader<OPCode> Header;

		static Bool FromBuffer(Packet& packet, const Void* lpBuffer, size_t size)
		{
			auto packetHeader     = *reinterpret_cast<const PacketHeader<T_OPCODE>*>(lpBuffer);
			packetHeader.OPCode   = ByteBuffer::FromEndian(packetHeader.OPCode);
			packetHeader.DataSize = ByteBuffer::FromEndian(packetHeader.DataSize);

			if (packetHeader.DataSize != (size - sizeof(Header)))
			{

				return False;
			}

			packet.header = Move(packetHeader);
			packet.buffer = ByteBuffer(lpBuffer, size);
			packet.buffer.SetReadPosition(sizeof(Header));

			return True;
		}

		Packet()
			: buffer(
				sizeof(Header)
			)
		{
			buffer.Write(header.OPCode);
			buffer.Write(header.DataSize);
		}

		Packet(Packet&& packet)
			: header(
				Move(packet.header)
			),
			buffer(
				Move(packet.buffer)
			)
		{
		}

		Packet(OPCode opcode, uint16 size)
			: buffer(
				sizeof(Header) + size
			)
		{
			header.OPCode   = opcode;
			header.DataSize = 0;

			buffer.Write(header.OPCode);
			buffer.Write(header.DataSize);
		}

		virtual ~Packet()
		{
		}

		auto GetOPCode() const
		{
			return header.OPCode;
		}

		auto GetBuffer() const
		{
			return buffer.GetBuffer();
		}

		auto GetBufferSize() const
		{
			return buffer.GetWritePosition();
		}

		size_t GetReadPosition() const
		{
			return buffer.GetReadPosition() - sizeof(Header);
		}

		size_t GetWritePosition() const
		{
			return buffer.GetWritePosition() - sizeof(Header);
		}

		Void Clear()
		{
			buffer.SetReadPosition(
				sizeof(Header)
			);

			buffer.SetWritePosition(
				sizeof(Header)
			);
		}

		Void Finalize()
		{
			typedef typename Get_Enum_Or_Integer_Base<decltype(Header::DataSize)>::Type T_PACKET_DATA_SIZE;
			typedef Integer<T_PACKET_DATA_SIZE>                                         T_PACKET_DATA_SIZE_INTEGER;

			auto writePosition = buffer.GetWritePosition();

			if (writePosition > T_PACKET_DATA_SIZE_INTEGER::Maximum)
			{

				writePosition = T_PACKET_DATA_SIZE_INTEGER::Maximum;
			}

			buffer.SetWritePosition(0);
			buffer.Write(header.OPCode);
			buffer.Write(static_cast<decltype(Header::DataSize)>(writePosition));

			buffer.SetWritePosition(
				writePosition
			);
		}

		template<typename T>
		inline bool Read(T& value)
		{
			if (!buffer.Read(value))
			{

				return False;
			}

			return True;
		}
		inline bool Read(String& value)
		{
			if (!buffer.Read(value))
			{

				return False;
			}

			return True;
		}
		inline bool Read(Void* lpBuffer, size_t size)
		{
			if (!buffer.Read(lpBuffer, size))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		inline bool Write(T value)
		{
			if (!buffer.Write(value))
			{

				return False;
			}

			return True;
		}
		inline bool Write(const String& value)
		{
			if (!buffer.Write(value))
			{

				return False;
			}

			return True;
		}
		inline bool Write(const Void* lpBuffer, size_t size)
		{
			if (!buffer.Write(lpBuffer, size))
			{

				return False;
			}

			return True;
		}

		Bool SetReadPosition(size_t value)
		{
			if (!buffer.SetReadPosition(sizeof(Header) + value))
			{

				return False;
			}

			return True;
		}

		Bool SetWritePosition(size_t value)
		{
			if (!buffer.SetWritePosition(sizeof(Header) + value))
			{

				return False;
			}

			return True;
		}

		Packet& operator = (Packet&& packet)
		{
			header = Move(
				packet.header
			);

			buffer = Move(
				packet.buffer
			);

			return *this;
		}

		Bool operator == (const Packet& packet) const
		{
			if (GetOPCode() != packet.GetOPCode())
			{

				return False;
			}

			if (buffer != packet.buffer)
			{
				
				return False;
			}

			return True;
		}
		Bool operator != (const Packet& packet) const
		{
			if (operator==(packet))
			{

				return False;
			}

			return True;
		}
	};
}
