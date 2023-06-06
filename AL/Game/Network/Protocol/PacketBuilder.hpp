#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"
#include "PacketHeader.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Queue.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T_OPCODE>
	class PacketBuilder
	{
		typedef T_OPCODE                        _OPCode;
		typedef Protocol::Packet<_OPCode>       _Packet;
		typedef Protocol::PacketHeader<_OPCode> _PacketHeader;

		Collections::Array<uint8>   buffer;
		size_t                      bufferSize = 0;

		Collections::Queue<_Packet> packets;

	public:
		typedef _OPCode OPCode;
		typedef _Packet Packet;

		explicit PacketBuilder(size_t bufferSize)
			: buffer(
				bufferSize
			)
		{
		}

		PacketBuilder(PacketBuilder&& packetBuilder)
			: buffer(
				Move(packetBuilder.buffer)
			),
			bufferSize(
				packetBuilder.bufferSize
			),
			packets(
				Move(packetBuilder.packets)
			)
		{
			packetBuilder.bufferSize = 0;
		}
		PacketBuilder(const PacketBuilder& packetBuilder)
			: buffer(
				packetBuilder.buffer
			),
			bufferSize(
				packetBuilder.bufferSize
			),
			packets(
				packetBuilder.packets
			)
		{
		}

		virtual ~PacketBuilder()
		{
		}

		auto GetCount() const
		{
			return packets.GetSize();
		}

		// @throw AL::Exception
		// @return AL::True on Packet completed
		Bool Append(const Void* lpBuffer, size_t size)
		{
			if (size == 0)
			{

				return False;
			}

			if (bufferSize < sizeof(_PacketHeader))
			{
				auto numberOfHeaderBytesRemaining = sizeof(_PacketHeader) - bufferSize;

				if (size < numberOfHeaderBytesRemaining)
				{
					memcpy(
						&buffer[bufferSize],
						lpBuffer,
						size
					);

					bufferSize += size;
					size        = 0;
					lpBuffer    = &reinterpret_cast<const uint8*>(lpBuffer)[size];
				}
				else
				{
					memcpy(
						&buffer[bufferSize],
						lpBuffer,
						numberOfHeaderBytesRemaining
					);

					bufferSize += numberOfHeaderBytesRemaining;
					size       -= numberOfHeaderBytesRemaining;
					lpBuffer    = &reinterpret_cast<const uint8*>(lpBuffer)[numberOfHeaderBytesRemaining];
				}
			}

			if (bufferSize >= sizeof(_PacketHeader))
			{
				auto lpHeader = reinterpret_cast<const _PacketHeader*>(
					&buffer[0]
				);

				auto header_DataSize = Packet::FromEndian(
					lpHeader->DataSize
				);

				if ((bufferSize + size) >= (sizeof(_PacketHeader) + header_DataSize))
				{
					auto numberOfDataBytesRemaining = (sizeof(_PacketHeader) + header_DataSize) - bufferSize;

					memcpy(
						&buffer[bufferSize],
						lpBuffer,
						numberOfDataBytesRemaining
					);

					auto packet = Packet::FromBuffer(
						&buffer[0],
						sizeof(_PacketHeader) + header_DataSize
					);

					packets.Enqueue(
						Move(packet)
					);

					bufferSize  = 0;
					size       -= numberOfDataBytesRemaining;
					lpBuffer    = &reinterpret_cast<const uint8*>(lpBuffer)[numberOfDataBytesRemaining];

					Append(
						lpBuffer,
						size
					);

					return True;
				}

				memcpy(
					&buffer[bufferSize],
					lpBuffer,
					size
				);

				bufferSize += size;
			}

			return False;
		}

		Bool Dequeue(Packet& packet)
		{
			if (!packets.Dequeue(packet))
			{

				return False;
			}

			return True;
		}

		Void Clear()
		{
			bufferSize = 0;

			packets.Clear();
		}

		PacketBuilder& operator = (PacketBuilder&& packetBuilder)
		{
			buffer = Move(
				packetBuilder.buffer
			);

			bufferSize = packetBuilder.bufferSize;
			packetBuilder.bufferSize = 0;

			packets = Move(
				packetBuilder.packets
			);

			return *this;
		}
		PacketBuilder& operator = (const PacketBuilder& packetBuilder)
		{
			buffer     = packetBuilder.buffer;
			bufferSize = packetBuilder.bufferSize;

			packets    = packetBuilder.packets;

			return *this;
		}

		Bool operator == (const PacketBuilder& packetBuilder) const
		{
			if (bufferSize != packetBuilder.bufferSize)
			{

				return False;
			}

			if (!memcmp(&buffer[0], &packetBuilder.buffer[0], bufferSize))
			{

				return False;
			}

			if (packets != packetBuilder.packets)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const PacketBuilder& packetBuilder) const
		{
			if (operator==(packetBuilder))
			{

				return False;
			}

			return True;
		}
	};
}
