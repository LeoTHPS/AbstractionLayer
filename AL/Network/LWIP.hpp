#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "SocketException.hpp"

#include "AL/OS/Timer.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/Hardware/PicoW/CYW43.hpp"

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
	#include <lwip/tcp.h>
	#include <lwip/udp.h>
	#include <lwip/pbuf.h>
#endif

namespace AL::Network
{
	class LWIP
	{
		LWIP() = delete;

	public:
		enum class SocketTypes : uint8
		{
			Tcp, Udp
		};

		class Socket
		{
			SocketTypes     type;
			AddressFamilies addressFamily;

			Socket(const Socket&) = delete;

		public:
			Socket(Socket&& socket)
				: type(
					socket.type
				),
				addressFamily(
					socket.addressFamily
				)
			{
			}

			Socket(SocketTypes type, AddressFamilies addressFamily)
				: type(
					type
				),
				addressFamily(
					addressFamily
				)
			{
			}

			virtual ~Socket()
			{
			}

			virtual Bool IsOpen() const = 0;

			virtual SocketTypes GetType() const
			{
				return type;
			}

			virtual Void* GetHandle() const = 0;

			virtual AddressFamilies GetAddressFamily() const
			{
				return addressFamily;
			}

			// @throw AL::Exception
			virtual Void Open() = 0;

			virtual Void Close() = 0;

			Socket& operator = (Socket&& socket)
			{
				Close();

				type          = socket.type;
				addressFamily = socket.addressFamily;

				return *this;
			}
		};

		class TcpSocket
			: public Socket
		{
			enum class IOFlags : uint8
			{
				Open              = 0x01,
				Aborted           = 0x02,
				Timeout           = 0x04,
				Connected         = 0x08,

				SendInProgress    = 0x10,
				ConnectInProgress = 0x20
			};

			BitMask<IOFlags>          flags;
			OS::Timer                 timer;
			TimeSpan                  timeout;
			Collections::Array<uint8> rxBuffer;
			size_t                    rxBufferSize     = 0;
			size_t                    txBufferSize     = 0;
			size_t                    txBufferCapacity = 0;

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
			::tcp_pcb*                pcb;
			::err_t                   errorCode;
#endif

		public:
			TcpSocket(TcpSocket&& tcpSocket)
				: Socket(
					Move(tcpSocket)
				),
				flags(
					Move(tcpSocket.flags)
				),
				timer(
					Move(tcpSocket.timer)
				),
				timeout(
					Move(tcpSocket.timeout)
				),
				rxBuffer(
					Move(tcpSocket.rxBuffer)
				),
				rxBufferSize(
					tcpSocket.rxBufferSize
				),
				txBufferSize(
					tcpSocket.txBufferSize
				),
				txBufferCapacity(
					tcpSocket.txBufferCapacity
				)
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				,
				pcb(
					tcpSocket.pcb
				),
				errorCode(
					tcpSocket.errorCode
				)
#endif
			{
				tcpSocket.rxBufferSize     = 0;
				tcpSocket.txBufferSize     = 0;
				tcpSocket.txBufferCapacity = 0;
			}

			TcpSocket(AddressFamilies addressFamily, size_t internalBufferSize)
				: Socket(
					SocketTypes::Tcp,
					addressFamily
				),
				rxBuffer(
					internalBufferSize
				)
			{
			}

			virtual ~TcpSocket()
			{
			}

			virtual Bool IsOpen() const override
			{
				return flags.IsSet(
					IOFlags::Open
				);
			}

			virtual Bool IsConnected() const
			{
				return flags.IsSet(
					IOFlags::Connected
				);
			}

			virtual Bool IsConnecting() const
			{
				return flags.IsSet(
					IOFlags::ConnectInProgress
				);
			}

			virtual Void* GetHandle() const override
			{
				return IsOpen() ? pcb : nullptr;
			}

			virtual ErrorCode GetLastErrorCode() const
			{
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				return errorCode;
#else
				return 0;
#endif
			}

			// @throw AL::Exception
			virtual Void Open() override
			{
				AL_ASSERT(
					!IsOpen(),
					"TcpSocket already open"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if ((pcb = ::tcp_new_ip_type(IP_GET_TYPE(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily())))) == nullptr)
				{
					SetLastErrorCode(
						::ERR_MEM
					);

					throw SocketException(
						"tcp_new_ip_type",
						::ERR_MEM
					);
				}

				SetLastErrorCode(
					::ERR_OK
				);

				RegisterEventHandlers();
#else
				throw DependencyMissingException(
					"CYW43_LWIP"
				);
#endif

				flags.Add(
					IOFlags::Open
				);
			}

			virtual Void Close() override
			{
				if (IsOpen())
				{
					if (!flags.IsSet(IOFlags::Aborted))
					{
						if (IsConnected())
						{
							Shutdown(
								True,
								True
							);
						}

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
						Sync([this]()
						{
							::tcp_close(
								pcb
							);

							SetLastErrorCode(
								::ERR_OK
							);
						});
#endif
					}

					flags.Clear();
				}
			}

			// @throw AL::Exception
			virtual Void Connect(const IPEndPoint& ep)
			{
				while (!Connect(ep, TimeSpan::Infinite))
				{
				}
			}
			// @throw AL::Exception
			// @return AL::False on timeout
			virtual Bool Connect(const IPEndPoint& ep, TimeSpan timeout)
			{
				AL_ASSERT(
					!IsConnected(),
					"TcpSocket already connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				Sync([this, &ep, timeout]()
				{
					ErrorCode errorCode;

					auto address = ep.Host.ToNative();

					this->timeout = timer.GetElapsed() + timeout;

					flags.Add(
						IOFlags::ConnectInProgress
					);

					if ((errorCode = ::tcp_connect(pcb, &address, ep.Port, &TcpSocket::OnConnect)) != ::ERR_OK)
					{
						flags.Remove(
							IOFlags::ConnectInProgress
						);

						SetLastErrorCode(
							errorCode
						);

						throw SocketException(
							"tcp_connect",
							errorCode
						);
					}
				});

				while (IsConnecting())
				{
					Poll();
				}

				if (flags.IsSet(IOFlags::Timeout))
				{
					Close();

					try
					{
						Open();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error re-opening TcpSocket"
						);
					}

					SetLastErrorCode(
						::ERR_TIMEOUT
					);

					return False;
				}

				if (!IsConnected())
				{

					throw SocketException(
						"tcp_connect",
						GetLastErrorCode()
					);
				}
#else
				throw NotImplementedException();
#endif

				return True;
			}

			// @throw AL::Exception
			virtual Void Shutdown(Bool rx, Bool tx)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"TcpSocket not connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				Sync([this, rx, tx]()
				{
					ErrorCode errorCode;

					if ((errorCode = ::tcp_shutdown(pcb, rx ? 1 : 0, tx ? 1 : 0)) != ::ERR_OK)
					{
						SetLastErrorCode(
							errorCode
						);

						throw SocketException(
							"tcp_shutdown",
							errorCode
						);
					}

					SetLastErrorCode(
						::ERR_OK
					);
				});
#else
				throw NotImplementedException();
#endif
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"TcpSocket not connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Sync([this, lpBuffer, size]()
				{
					ErrorCode errorCode;

					txBufferSize     = 0;
					txBufferCapacity = size;

					flags.Add(
						IOFlags::SendInProgress
					);

					this->timeout = timer.GetElapsed() + TimeSpan::Infinite;

					if ((errorCode = ::tcp_write(pcb, lpBuffer, static_cast<::u16_t>(size), TCP_WRITE_FLAG_COPY)) != ::ERR_OK)
					{
						flags.Remove(
							IOFlags::SendInProgress
						);

						SetLastErrorCode(
							errorCode
						);

						throw SocketException(
							"tcp_write",
							errorCode
						);
					}
				});

				while (flags.IsSet(IOFlags::SendInProgress))
				{
					Poll();
				}

				if (!IsConnected())
				{
					auto errorCode = GetLastErrorCode();

					Close();

					try
					{
						Open();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error re-opening TcpSocket"
						);
					}

					SetLastErrorCode(
						errorCode
					);

					return False;
				}

				numberOfBytesSent = txBufferSize;
#else
				throw NotImplementedException();
#endif

				return True;
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"TcpSocket not connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Poll();

				Sync([this, lpBuffer, size, &numberOfBytesReceived]()
				{
					if ((numberOfBytesReceived = rxBufferSize) > 0)
					{
						if (numberOfBytesReceived > size)
						{

							numberOfBytesReceived = size;
						}

						OS::Console::WriteLine(
							"[LWIP::TcpSocket::Receive] Copying %lu/%lu bytes",
							numberOfBytesReceived,
							rxBufferSize
						);

						memcpy(
							lpBuffer,
							&rxBuffer[0],
							numberOfBytesReceived
						);

						if (numberOfBytesReceived < rxBufferSize)
						{
							memcpy(
								&rxBuffer[numberOfBytesReceived],
								&rxBuffer[0],
								rxBufferSize - numberOfBytesReceived
							);
						}

						rxBufferSize -= numberOfBytesReceived;
					}
				});

				if ((numberOfBytesReceived == 0) && !IsConnected())
				{
					Close();

					return False;
				}
#else
				throw NotImplementedException();
#endif

				return True;
			}

			TcpSocket& operator = (TcpSocket&& tcpSocket)
			{
				Socket::operator=(
					Move(tcpSocket)
				);

				flags = Move(
					tcpSocket.flags
				);

				timer = Move(
					tcpSocket.timer
				);

				timeout = Move(
					tcpSocket.timeout
				);

				rxBuffer = Move(
					tcpSocket.rxBuffer
				);

				rxBufferSize = tcpSocket.rxBufferSize;
				tcpSocket.rxBufferSize = 0;

				txBufferSize = tcpSocket.txBufferSize;
				tcpSocket.txBufferSize = 0;

				txBufferCapacity = tcpSocket.txBufferCapacity;
				tcpSocket.txBufferCapacity = 0;

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				pcb       = tcpSocket.pcb;
				errorCode = tcpSocket.errorCode;
#endif

				return *this;
			}

		private:
			Void RegisterEventHandlers()
			{
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				::tcp_arg(pcb, this);
				::tcp_err(pcb, &TcpSocket::OnError);
				::tcp_poll(pcb, &TcpSocket::OnPoll, 1);
				::tcp_sent(pcb, &TcpSocket::OnSend);
				::tcp_recv(pcb, &TcpSocket::OnReceive);
#endif
			}

			Void SetLastErrorCode(ErrorCode value)
			{
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				errorCode = value;
#endif
			}

		private:
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
			static ::err_t OnPoll(void* lpParam, ::tcp_pcb* pcb)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				if (lpSocket->flags.IsSet(IOFlags::ConnectInProgress))
				{
					if (lpSocket->timer.GetElapsed() >= lpSocket->timeout)
					{
						::tcp_abort(
							pcb
						);

						lpSocket->Close();

						lpSocket->flags.Add(
							IOFlags::Timeout
						);

						return ::ERR_ABRT;
					}
				}

				return ::ERR_OK;
			}

			static void    OnError(void* lpParam, ::err_t errorCode)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				lpSocket->Close();

				lpSocket->SetLastErrorCode(
					errorCode
				);
			}

			static ::err_t OnConnect(void* lpParam, ::tcp_pcb* pcb, ::err_t errorCode)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				if ((errorCode != ::ERR_OK) || (pcb == nullptr))
				{
					lpSocket->SetLastErrorCode(
						errorCode
					);

					lpSocket->flags.Remove(
						IOFlags::ConnectInProgress
					);

					return ::ERR_VAL;
				}

				lpSocket->pcb = pcb;

				lpSocket->flags.Add(
					IOFlags::Connected
				);

				lpSocket->flags.Remove(
					IOFlags::ConnectInProgress
				);

				return ::ERR_OK;
			}

			static ::err_t OnSend(void* lpParam, ::tcp_pcb* pcb, ::u16_t numberOfBytesSent)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				lpSocket->txBufferSize = numberOfBytesSent;

				lpSocket->flags.Remove(
					IOFlags::SendInProgress
				);

				return ::ERR_OK;
			}

			static ::err_t OnReceive(void* lpParam, ::tcp_pcb* pcb, ::pbuf* buffer, ::err_t errorCode)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				if (errorCode != ::ERR_OK)
				{
					::tcp_abort(
						pcb
					);

					lpSocket->flags.Add(
						IOFlags::Aborted
					);

					lpSocket->Close();

					return ::ERR_ABRT;
				}

				size_t bufferSize = 0;

				if (buffer->tot_len > 0)
				{
					size_t bufferCapacity;

					if ((bufferSize = buffer->tot_len) > (bufferCapacity = (lpSocket->rxBuffer.GetCapacity() - lpSocket->rxBufferSize)))
					{

						bufferSize = bufferCapacity;
					}

					if (bufferSize > 0)
					{
						::pbuf_copy_partial(
							buffer,
							&lpSocket->rxBuffer[lpSocket->rxBufferSize],
							static_cast<::u16_t>(bufferSize),
							0
						);

						::tcp_recved(
							pcb,
							static_cast<::u16_t>(bufferSize)
						);
					}
				}

				::pbuf_free(
					buffer
				);

				lpSocket->rxBufferSize += bufferSize;

				return ::ERR_OK;
			}
#endif
		};

		class UdpSocket
			: public Socket
		{
			Bool       isOpen      = False;
			Bool       isConnected = False;

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
			::udp_pcb* pcb;
#endif

		public:
			UdpSocket(UdpSocket&& udpSocket)
				: Socket(
					Move(udpSocket)
				),
				isOpen(
					udpSocket.isOpen
				),
				isConnected(
					udpSocket.isConnected
				),
				pcb(
					udpSocket.pcb
				)
			{
				udpSocket.isOpen = False;
				udpSocket.isConnected = False;
			}

			explicit UdpSocket(AddressFamilies addressFamily)
				: Socket(
					SocketTypes::Tcp,
					addressFamily
				)
			{
			}

			virtual ~UdpSocket()
			{
			}

			virtual Bool IsOpen() const override
			{
				return isOpen;
			}

			virtual Bool IsConnected() const
			{
				return isConnected;
			}

			virtual Void* GetHandle() const override
			{
				return IsOpen() ? pcb : nullptr;
			}

			// @throw AL::Exception
			virtual Void Open() override
			{
				AL_ASSERT(
					!IsOpen(),
					"UdpSocket already open"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if ((pcb = ::udp_new_ip_type(IP_GET_TYPE(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily())))) == nullptr)
				{

					throw SocketException(
						"udp_new_ip_type",
						::ERR_MEM
					);
				}

				RegisterEventHandlers();
#else
				throw DependencyMissingException(
					"CYW43_LWIP"
				);
#endif

				isOpen = True;
			}

			virtual Void Close() override
			{
				if (IsOpen())
				{
					if (IsConnected())
					{

						Disconnect();
					}

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
					Sync([this]()
					{
						::udp_remove(
							pcb
						);
					});
#endif

					isOpen = False;
				}
			}

			// @throw AL::Exception
			virtual Void Connect(const IPEndPoint& ep)
			{
				AL_ASSERT(
					!IsConnected(),
					"UdpSocket already connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				Sync([this, &ep]()
				{
					ErrorCode errorCode;

					auto address = ep.Host.ToNative();

					if ((errorCode = ::udp_connect(pcb, &address, ep.Port)) == ::ERR_OK)
					{

						throw SocketException(
							"udp_connect",
							errorCode
						);
					}
				});
#else
				throw NotImplementedException();
#endif

				isConnected = True;
			}

			// @throw AL::Exception
			virtual Void Disconnect()
			{
				if (IsOpen())
				{
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
					Sync([this]()
					{
						::udp_disconnect(
							pcb
						);
					});
#endif

					isConnected = False;
				}
			}

			// @throw AL::Exception
			// @return AL::False on no route to host
			virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
			{
				AL_ASSERT(
					IsOpen(),
					"UdpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"UdpSocket not connected"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				auto buffer = ::pbuf_alloc_reference(
					const_cast<Void*>(lpBuffer),
					static_cast<::u16_t>(size),
					::PBUF_RAM
				);

				ErrorCode errorCode;

				Sync_Begin();

				if ((errorCode = ::udp_send(pcb, buffer)) != ::ERR_OK)
				{
					Sync_End();

					::pbuf_free(
						buffer
					);

					if (errorCode == ::ERR_RTE)
					{
						numberOfBytesSent = 0;

						return False;
					}

					throw SocketException(
						"udp_send",
						errorCode
					);
				}

				Sync_End();

				::pbuf_free(
					buffer
				);

				numberOfBytesSent = size;
#else
				throw NotImplementedException();
#endif

				return True;
			}

			// @throw AL::Exception
			// @return AL::False on no route to host
			virtual Bool SendTo(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent, const IPEndPoint& ep)
			{
				AL_ASSERT(
					IsOpen(),
					"UdpSocket not open"
				);

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				auto buffer = ::pbuf_alloc_reference(
					const_cast<Void*>(lpBuffer),
					static_cast<::u16_t>(size),
					::PBUF_RAM
				);

				ErrorCode errorCode;

				auto address = ep.Host.ToNative();

				Sync_Begin();

				if ((errorCode = ::udp_sendto(pcb, buffer, &address, ep.Port)) != ::ERR_OK)
				{
					Sync_End();

					::pbuf_free(
						buffer
					);

					if (errorCode == ::ERR_RTE)
					{
						numberOfBytesSent = 0;

						return False;
					}

					throw SocketException(
						"udp_sendto",
						errorCode
					);
				}

				Sync_End();

				::pbuf_free(
					buffer
				);

				numberOfBytesSent = size;
#else
				throw NotImplementedException();
#endif

				return True;
			}

			UdpSocket& operator = (UdpSocket&& udpSocket)
			{
				Socket::operator=(
					Move(udpSocket)
				);

				isOpen = udpSocket.isOpen;
				udpSocket.isOpen = False;

				isConnected = udpSocket.isConnected;
				udpSocket.isConnected = False;

#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				pcb = udpSocket.pcb;
#endif

				return *this;
			}

		private:
			Void RegisterEventHandlers()
			{
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
				::udp_recv(pcb, &UdpSocket::OnReceive, this);
#endif
			}

		private:
#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP)
			static void OnReceive(void* lpParam, ::udp_pcb* pcb, ::pbuf* buffer, const ::ip_addr_t* lpAddress, ::u16_t port)
			{
				auto lpSocket = reinterpret_cast<UdpSocket*>(
					lpParam
				);

			}
#endif
		};

		static Void Poll()
		{
			Hardware::PicoW::CYW43::Poll();
		}

		template<typename F, typename ... TArgs>
		static AL_INLINE Void Sync(F&& function, TArgs ... args)
		{
			Sync_Begin();

			try
			{
				function(
					Forward<TArgs>(args) ...
				);
			}
			catch (...)
			{
				Sync_End();

				throw;
			}

			Sync_End();
		}

		static AL_INLINE Void Sync_Begin()
		{
#if defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_begin();
#endif
		}

		static AL_INLINE Void Sync_End()
		{
#if defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_end();
#endif
		}
	};
}
