#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "SocketException.hpp"

#include "AL/OS/Timer.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"

#include "AL/Hardware/Pico/CYW43.hpp"

#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/pbuf.h>

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
			typedef Void* Handle;

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

			virtual Handle GetHandle() const = 0;

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
			enum class IOFlags : uint16
			{
				Open              = 0x01,
				Bound             = 0x02,
				NoDelay           = 0x04,
				Aborted           = 0x08,
				Timeout           = 0x10,
				Listening         = 0x20,
				Connected         = 0x40,
				ConnectionClosed  = 0x80,

				SendInProgress    = 0x100,
				AcceptInProgress  = 0x200,
				ConnectInProgress = 0x400
			};

			typedef Collections::Array<uint8> RXBuffer;

			struct RXContext
			{
				RXBuffer Buffer;
				size_t   BufferSize;
				size_t   BufferOffset;
			};

			typedef Collections::LinkedList<RXContext> RXContexts;

			BitMask<IOFlags> flags;
			OS::Timer        timer;
			TimeSpan         timeout;
			IPEndPoint       localEP;
			IPEndPoint       remoteEP;
			RXContexts       rxContexts;
			size_t           txBufferSize     = 0;
			size_t           txBufferCapacity = 0;

			::tcp_pcb*       pcb;
			::err_t          errorCode;

		public:
			typedef typename Socket::Handle Handle;

			static constexpr size_t BACKLOG_MAX = 0xFF;

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
				localEP(
					Move(tcpSocket.localEP)
				),
				remoteEP(
					Move(tcpSocket.remoteEP)
				),
				rxContexts(
					Move(tcpSocket.rxContexts)
				),
				txBufferSize(
					tcpSocket.txBufferSize
				),
				txBufferCapacity(
					tcpSocket.txBufferCapacity
				),
				pcb(
					tcpSocket.pcb
				),
				errorCode(
					tcpSocket.errorCode
				)
			{
				tcpSocket.txBufferSize     = 0;
				tcpSocket.txBufferCapacity = 0;
			}

			explicit TcpSocket(AddressFamilies addressFamily)
				: Socket(
					SocketTypes::Tcp,
					addressFamily
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

			virtual Bool IsBound() const
			{
				return flags.IsSet(
					IOFlags::Bound
				);
			}

			virtual Bool IsNoDelay() const
			{
				return flags.IsSet(
					IOFlags::NoDelay
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

			virtual Bool IsListening() const
			{
				return flags.IsSet(
					IOFlags::Listening
				);
			}

			virtual Handle GetHandle() const override
			{
				return IsOpen() ? pcb : nullptr;
			}

			virtual ErrorCode GetLastErrorCode() const
			{
				return errorCode;
			}

			virtual const IPEndPoint& GetLocalEndPoint() const
			{
				return localEP;
			}

			virtual const IPEndPoint& GetRemoteEndPoint() const
			{
				return remoteEP;
			}

			// @throw AL::Exception
			virtual Void Open() override
			{
				AL_ASSERT(
					!IsOpen(),
					"TcpSocket already open"
				);

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

				localEP =
				{
					.Host = IPAddress::Any(),
					.Port = 0
				};

				remoteEP =
				{
					.Host = IPAddress::Any(),
					.Port = 0
				};

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
						Sync(
							::tcp_close,
							pcb
						);

						SetLastErrorCode(
							::ERR_OK
						);
					}

					flags.Clear();
				}
			}

			// @throw AL::Exception
			virtual Void Bind(const IPEndPoint& ep)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					!IsBound(),
					"TcpSocket already bound"
				);

				ErrorCode errorCode;

				auto address = ep.Host.ToNative();

				if ((errorCode = ::tcp_bind(pcb, &address, ep.Port)) != ::ERR_OK)
				{

					throw SocketException(
						"tcp_bind",
						errorCode
					);
				}

				localEP = ep;

				flags.Add(
					IOFlags::Bound
				);
			}

			// @throw AL::Exception
			virtual Void Listen(size_t backlog)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					!IsListening(),
					"TcpSocket already listening"
				);

				if (backlog > BACKLOG_MAX)
				{

					backlog = BACKLOG_MAX;
				}

				::tcp_pcb* pcb;
				::err_t    errorCode;

				if ((pcb = ::tcp_listen_with_backlog_and_err(this->pcb, backlog, &errorCode)) == nullptr)
				{

					throw SocketException(
						"tcp_listen_with_backlog_and_err",
						::ERR_MEM
					);
				}

				if (errorCode != ::ERR_OK)
				{

					throw SocketException(
						"tcp_listen_with_backlog_and_err",
						errorCode
					);
				}

				flags.Add(
					IOFlags::Listening
				);
			}

			// @throw AL::Exception
			// @return AL::False on timeout
			virtual Bool Accept(TcpSocket& socket)
			{
				if (!Accept(socket, TimeSpan::Infinite))
				{

					return False;
				}

				return True;
			}
			// @throw AL::Exception
			// @return AL::False on timeout
			virtual Bool Accept(TcpSocket& socket, TimeSpan timeout)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsListening(),
					"TcpSocket not listening"
				);

				// TODO: implement
				throw NotImplementedException();

				return True;
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
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					!IsConnected(),
					"TcpSocket already connected"
				);

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

					if (GetLastErrorCode() == ::ERR_ABRT)
						break;
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

				localEP =
				{
					.Host = IPAddress::FromNative(pcb->local_ip),
					.Port = pcb->local_port
				};

				remoteEP =
				{
					.Host = IPAddress::FromNative(pcb->remote_ip),
					.Port = pcb->remote_port
				};

				return True;
			}

			// @throw AL::Exception
			virtual Void Shutdown(Bool rx, Bool tx)
			{
				if (IsConnected() && !flags.IsSet(IOFlags::ConnectionClosed))
				{
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

					remoteEP =
					{
						.Host = IPAddress::Any(),
						.Port = 0
					};
				}
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
			{
				if (!Send(lpBuffer, size, numberOfBytesSent, TimeSpan::Infinite))
				{

					return False;
				}

				return True;
			}
			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent, TimeSpan timeout)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"TcpSocket not connected"
				);

				if (CloseIfConnectionClosed())
				{

					return False;
				}

				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Sync([this, lpBuffer, size, timeout]()
				{
					ErrorCode errorCode;

					txBufferSize     = 0;
					txBufferCapacity = size;

					flags.Add(
						IOFlags::SendInProgress
					);

					this->timeout = timer.GetElapsed() + timeout;

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

					if (flags.IsSet(IOFlags::Timeout))
					{
						SetLastErrorCode(
							::ERR_TIMEOUT
						);

						break;
					}

					if (flags.IsSet(IOFlags::ConnectionClosed))
					{

						break;
					}
				}

				if (auto errorCode = GetLastErrorCode(); CloseIfConnectionClosed())
				{
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

				return True;
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
			{
				if (!Receive(lpBuffer, size, numberOfBytesReceived, TimeSpan::Infinite))
				{

					return False;
				}

				return True;
			}
			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived, TimeSpan timeout)
			{
				AL_ASSERT(
					IsOpen(),
					"TcpSocket not open"
				);

				AL_ASSERT(
					IsConnected(),
					"TcpSocket not connected"
				);

				if (CloseIfConnectionClosed())
				{

					return False;
				}

				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				numberOfBytesReceived = 0;

				auto _timeout = timer.GetElapsed() + timeout;

				do
				{
					Poll();

					Sync([this, lpBuffer, size, &numberOfBytesReceived]()
					{
						if (rxContexts.GetSize() > 0)
						{
							size_t rxBufferSize;

							auto& rxContext = *rxContexts.begin();

							if ((rxBufferSize = rxContext.BufferSize) > size)
							{

								rxBufferSize = size;
							}

							memcpy(
								lpBuffer,
								&rxContext.Buffer[rxContext.BufferOffset],
								rxBufferSize
							);

							numberOfBytesReceived   = rxBufferSize;
							rxContext.BufferOffset += rxBufferSize;

							if ((rxContext.BufferSize -= rxBufferSize) == 0)
							{

								rxContexts.PopFront();
							}
						}
					});

					if ((numberOfBytesReceived == 0) && CloseIfConnectionClosed())
					{

						return False;
					}
				} while ((numberOfBytesReceived == 0) && (timer.GetElapsed() < _timeout));

				return True;
			}

			virtual Void SetNoDelay(Bool value)
			{
				if (IsOpen())
				{
					if (value) tcp_nagle_disable(pcb);
					else       tcp_nagle_enable(pcb);
				}

				flags.Set(
					IOFlags::NoDelay,
					value
				);
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

				localEP = Move(
					tcpSocket.localEP
				);

				remoteEP = Move(
					tcpSocket.remoteEP
				);

				rxContexts = Move(
					tcpSocket.rxContexts
				);

				txBufferSize = tcpSocket.txBufferSize;
				tcpSocket.txBufferSize = 0;

				txBufferCapacity = tcpSocket.txBufferCapacity;
				tcpSocket.txBufferCapacity = 0;

				pcb       = tcpSocket.pcb;
				errorCode = tcpSocket.errorCode;

				return *this;
			}

		private:
			Void Abort()
			{
				::tcp_abort(
					pcb
				);

				flags.Add(
					IOFlags::Aborted
				);

				CloseConnection();
			}

			Void SetLastErrorCode(ErrorCode value)
			{
				errorCode = value;
			}

			Void RegisterEventHandlers()
			{
				::tcp_arg(pcb, this);
				::tcp_err(pcb, &TcpSocket::OnError);
				::tcp_poll(pcb, &TcpSocket::OnPoll, 1);
				::tcp_sent(pcb, &TcpSocket::OnSend);
				::tcp_recv(pcb, &TcpSocket::OnReceive);
			}

			Void CloseConnection()
			{
				flags.Add(
					IOFlags::ConnectionClosed
				);
			}

			// @return AL::True on connection closed
			Bool CloseIfConnectionClosed()
			{
				if (flags.IsSet(IOFlags::ConnectionClosed))
				{
					Close();

					return True;
				}

				return False;
			}

		private:
			static ::err_t OnPoll(void* lpParam, ::tcp_pcb* pcb)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				if (lpSocket->flags.IsSet(IOFlags::ConnectInProgress))
				{
					if (lpSocket->timer.GetElapsed() >= lpSocket->timeout)
					{
						// OS::Console::WriteLine(
						// 	"[LWIP::TcpSocket::OnPoll] [lpParam: 0x%p, pcb: 0x%p] Connect timed out",
						// 	lpParam,
						// 	pcb
						// );

						lpSocket->flags.Add(
							IOFlags::Timeout
						);

						lpSocket->Abort();

						return ::ERR_ABRT;
					}
				}
				else if (lpSocket->flags.IsSet(IOFlags::SendInProgress))
				{
					if (lpSocket->timer.GetElapsed() >= lpSocket->timeout)
					{
						// OS::Console::WriteLine(
						// 	"[LWIP::TcpSocket::OnPoll] [lpParam: 0x%p, pcb: 0x%p] Send timed out",
						// 	lpParam,
						// 	pcb
						// );

						lpSocket->flags.Add(
							IOFlags::Timeout
						);

						lpSocket->Abort();

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

				// OS::Console::WriteLine(
				// 	"[LWIP::TcpSocket::OnError] [lpParam: 0x%p, errorCode: %i]",
				// 	lpParam,
				// 	errorCode
				// );

				lpSocket->CloseConnection();

				lpSocket->SetLastErrorCode(
					errorCode
				);
			}

			static ::err_t OnAccept(void* lpParam, ::tcp_pcb* pcb, ::err_t errorCode)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				// OS::Console::WriteLine(
				// 	"[LWIP::TcpSocket::OnAccept] [lpParam: 0x%p, pcb: 0x%p, errorCode: %i]",
				// 	lpParam,
				// 	pcb,
				// 	errorCode
				// );

				// TODO: implement

				return ::ERR_OK;
			}

			static ::err_t OnConnect(void* lpParam, ::tcp_pcb* pcb, ::err_t errorCode)
			{
				auto lpSocket = reinterpret_cast<TcpSocket*>(
					lpParam
				);

				// OS::Console::WriteLine(
				// 	"[LWIP::TcpSocket::OnConnect] [lpParam: 0x%p, pcb: 0x%p, errorCode: %i]",
				// 	lpParam,
				// 	pcb,
				// 	errorCode
				// );

				if ((errorCode != ::ERR_OK) || (pcb == nullptr))
				{
					lpSocket->CloseConnection();

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

				// OS::Console::WriteLine(
				// 	"[LWIP::TcpSocket::OnSend] [lpParam: 0x%p, pcb: 0x%p, numberOfBytesSent: %u]",
				// 	lpParam,
				// 	pcb,
				// 	numberOfBytesSent
				// );

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

				// OS::Console::WriteLine(
				// 	"[LWIP::TcpSocket::OnReceive] [lpParam: 0x%p, pcb: 0x%p, buffer: { tot_len: %u }, errorCode: %i]",
				// 	lpParam,
				// 	pcb,
				// 	buffer->tot_len,
				// 	errorCode
				// );

				if (errorCode != ::ERR_OK)
				{
					lpSocket->Abort();

					return ::ERR_ABRT;
				}

				size_t rxBufferSize = 0;

				if ((rxBufferSize = buffer->tot_len) > 0)
				{
					RXContext rxContext =
					{
						.Buffer       = RXBuffer(rxBufferSize),
						.BufferSize   = rxBufferSize,
						.BufferOffset = 0
					};

					::pbuf_copy_partial(
						buffer,
						&rxContext.Buffer[0],
						static_cast<::u16_t>(rxBufferSize),
						0
					);

					::tcp_recved(
						pcb,
						static_cast<::u16_t>(rxBufferSize)
					);

					OS::Console::WriteLine(
						HexConverter::Encode(
							&rxContext.Buffer[0],
							rxBufferSize
						)
					);

					lpSocket->rxContexts.PushBack(
						Move(rxContext)
					);
				}

				::pbuf_free(
					buffer
				);

				return ::ERR_OK;
			}
		};

		class UdpSocket
			: public Socket
		{
			typedef Collections::Array<uint8> RXBuffer;

			struct RXContext
			{
				IPEndPoint EP;
				RXBuffer   Buffer;
				size_t     BufferSize;
				size_t     BufferOffset;
			};

			typedef Collections::LinkedList<RXContext> RXContexts;

			Bool       isOpen      = False;
			Bool       isBound     = False;
			Bool       isConnected = False;

			IPEndPoint localEP;
			IPEndPoint remoteEP;
			RXContexts rxContexts;

			::udp_pcb* pcb;

		public:
			typedef typename Socket::Handle Handle;

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
				localEP(
					Move(udpSocket.localEP)
				),
				remoteEP(
					Move(udpSocket.remoteEP)
				),
				rxContexts(
					Move(udpSocket.rxContexts)
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
					SocketTypes::Udp,
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

			virtual Bool IsBound() const
			{
				return isBound;
			}

			virtual Bool IsConnected() const
			{
				return isConnected;
			}

			virtual Handle GetHandle() const override
			{
				return IsOpen() ? pcb : nullptr;
			}

			virtual const IPEndPoint& GetLocalEndPoint() const
			{
				return localEP;
			}

			virtual const IPEndPoint& GetRemoteEndPoint() const
			{
				return remoteEP;
			}

			// @throw AL::Exception
			virtual Void Open() override
			{
				AL_ASSERT(
					!IsOpen(),
					"UdpSocket already open"
				);

				if ((pcb = ::udp_new_ip_type(IP_GET_TYPE(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily())))) == nullptr)
				{

					throw SocketException(
						"udp_new_ip_type",
						::ERR_MEM
					);
				}

				RegisterEventHandlers();

				localEP =
				{
					.Host = IPAddress::Any(),
					.Port = 0
				};

				remoteEP =
				{
					.Host = IPAddress::Any(),
					.Port = 0
				};

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

					Sync([this]()
					{
						::udp_remove(
							pcb
						);
					});

					isOpen = False;
				}
			}

			// @throw AL::Exception
			virtual Void Bind(const IPEndPoint& ep)
			{
				AL_ASSERT(
					IsOpen(),
					"UdpSocket not open"
				);

				AL_ASSERT(
					!IsBound(),
					"UdpSocket already bound"
				);

				ErrorCode errorCode;

				auto address = ep.Host.ToNative();

				if ((errorCode = ::udp_bind(pcb, &address, ep.Port)) != ::ERR_OK)
				{

					throw SocketException(
						"udp_bind",
						errorCode
					);
				}

				localEP = ep;

				isBound = True;
			}

			// @throw AL::Exception
			virtual Void Connect(const IPEndPoint& ep)
			{
				AL_ASSERT(
					IsOpen(),
					"UdpSocket not open"
				);

				AL_ASSERT(
					!IsConnected(),
					"UdpSocket already connected"
				);

				Sync([this, &ep]()
				{
					ErrorCode errorCode;

					auto address = ep.Host.ToNative();

					if ((errorCode = ::udp_connect(pcb, &address, ep.Port)) != ::ERR_OK)
					{

						throw SocketException(
							"udp_connect",
							errorCode
						);
					}
				});

				remoteEP = ep;

				isConnected = True;
			}

			virtual Void Disconnect()
			{
				if (IsConnected())
				{
					Sync([this]()
					{
						::udp_disconnect(
							pcb
						);
					});

					remoteEP =
					{
						.Host = IPAddress::Any(),
						.Port = 0
					};

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

				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Sync_Begin();

				auto buffer = ::pbuf_alloc(
					::PBUF_TRANSPORT,
					static_cast<::u16_t>(size),
					::PBUF_RAM
				);

				memcpy(
					buffer->payload,
					buffer,
					size
				);

				ErrorCode errorCode;

				if ((errorCode = ::udp_send(pcb, buffer)) != ::ERR_OK)
				{
					::pbuf_free(
						buffer
					);

					Sync_End();

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

				::pbuf_free(
					buffer
				);

				Sync_End();

				numberOfBytesSent = size;

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

				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Sync_Begin();

				auto buffer = ::pbuf_alloc(
					::PBUF_TRANSPORT,
					static_cast<::u16_t>(size),
					::PBUF_RAM
				);

				memcpy(
					buffer->payload,
					buffer,
					size
				);

				ErrorCode errorCode;

				auto address = ep.Host.ToNative();

				if ((errorCode = ::udp_sendto(pcb, buffer, &address, ep.Port)) != ::ERR_OK)
				{
					::pbuf_free(
						buffer
					);

					Sync_End();

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

				::pbuf_free(
					buffer
				);

				Sync_End();

				numberOfBytesSent = size;

				return True;
			}

			// @throw AL::Exception
			// @return number of bytes received
			virtual size_t Receive(Void* lpBuffer, size_t size, IPEndPoint& ep)
			{
				AL_ASSERT(
					IsOpen(),
					"UdpSocket not open"
				);

				size_t numberOfBytesReceived = 0;

				if (size > Integer<::u16_t>::Maximum)
				{

					size = Integer<::u16_t>::Maximum;
				}

				Poll();

				Sync([this, lpBuffer, size, &ep, &numberOfBytesReceived]()
				{
					if (rxContexts.GetSize() > 0)
					{
						size_t rxBufferSize;

						auto& rxContext = *rxContexts.begin();

						if ((rxBufferSize = rxContext.BufferSize) > size)
						{

							rxBufferSize = size;
						}

						ep = rxContext.EP;

						memcpy(
							lpBuffer,
							&rxContext.Buffer[rxContext.BufferOffset],
							rxBufferSize
						);

						numberOfBytesReceived   = rxBufferSize;
						rxContext.BufferOffset += rxBufferSize;

						if ((rxContext.BufferSize -= rxBufferSize) == 0)
						{

							rxContexts.PopFront();
						}
					}
				});

				return numberOfBytesReceived;
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

				localEP = Move(
					udpSocket.localEP
				);

				remoteEP = Move(
					udpSocket.remoteEP
				);

				rxContexts = Move(
					udpSocket.rxContexts
				);

				pcb = udpSocket.pcb;

				return *this;
			}

		private:
			Void RegisterEventHandlers()
			{
				::udp_recv(pcb, &UdpSocket::OnReceive, this);
			}

		private:
			static void OnReceive(void* lpParam, ::udp_pcb* pcb, ::pbuf* buffer, const ::ip_addr_t* lpAddress, ::u16_t port)
			{
				auto lpSocket = reinterpret_cast<UdpSocket*>(
					lpParam
				);

				// OS::Console::WriteLine(
				// 	"[LWIP::UdpSocket::OnReceive] [lpParam: 0x%p, pcb: 0x%p, buffer: { tot_len: %u }, lpAddress->addr: %lu, port: %u]",
				// 	lpParam,
				// 	pcb,
				// 	buffer->tot_len,
				// 	lpAddress->addr,
				// 	port
				// );

				size_t rxBufferSize = 0;

				if ((rxBufferSize = buffer->tot_len) > 0)
				{
					RXContext rxContext =
					{
						.EP =
						{
							.Host   = IPAddress::FromNative(*lpAddress),
							.Port   = port
						},
						.Buffer     = RXBuffer(rxBufferSize),
						.BufferSize = rxBufferSize
					};

					::pbuf_copy_partial(
						buffer,
						&rxContext.Buffer[0],
						static_cast<::u16_t>(rxBufferSize),
						0
					);

					// OS::Console::WriteLine(
					// 	HexConverter::Encode(
					// 		&rxContext.Buffer[0],
					// 		rxBufferSize
					// 	)
					// );

					lpSocket->rxContexts.PushBack(
						Move(rxContext)
					);
				}

				::pbuf_free(
					buffer
				);
			}
		};

		static Void Poll()
		{
			Hardware::Pico::CYW43::Poll();
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
#if defined(AL_DEPENDENCY_PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_begin();
#endif
		}

		static AL_INLINE Void Sync_End()
		{
#if defined(AL_DEPENDENCY_PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_end();
#endif
		}
	};
}
