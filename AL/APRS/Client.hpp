#pragma once
#include "AL/Common.hpp"

#include "AL/Hardware/UART.hpp"

#include "AL/Network/TcpSocket.hpp"
#include "AL/Network/SocketExtensions.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"

#include "AL/Serialization/APRS/Packet.hpp"
#include "AL/Serialization/APRS/Message.hpp"
#include "AL/Serialization/APRS/Position.hpp"
#include "AL/Serialization/APRS/Telemetry.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#undef SendMessage
#endif

namespace AL::APRS
{
	typedef Serialization::APRS::Packet      Packet;
	typedef Serialization::APRS::Message     Message;
	typedef Serialization::APRS::Position    Position;
	typedef Serialization::APRS::Telemetry   Telemetry;

	typedef Serialization::APRS::DigiPath    DigiPath;
	typedef Serialization::APRS::QConstructs QConstructs;
	typedef Serialization::APRS::PacketTypes PacketTypes;

	inline String DigiPath_ToString(const DigiPath& value)
	{
		return Serialization::APRS::DigiPath_ToString(value);
	}
	inline Bool   DigiPath_FromString(DigiPath& path, const String& value)
	{
		return Serialization::APRS::DigiPath_FromString(path, value);
	}

	inline String QConstruct_ToString(QConstructs value)
	{
		return Serialization::APRS::QConstruct_ToString(value);
	}
	inline Bool   QConstruct_FromString(QConstructs& qConstruct, const String& value)
	{
		return Serialization::APRS::QConstruct_FromString(qConstruct, value);
	}

	enum class ClientConnectionTypes : uint8
	{
		None,
		APRS_IS,
		KISS_Tcp,
		KISS_Serial,
		UserDefined
	};

	enum class ClientDisconnectReasons : uint8
	{
		UserRequested,
		ConnectionLost,
		AuthenticationFailed
	};

	// @throw AL::Exception
	typedef Function<Void()>                                   ClientMessageCallback;
	// @throw AL::Exception
	typedef Function<Bool(const Packet& packet)>               ClientPacketFilterCallback;
	// @throw AL::Exception
	typedef Function<Void(const Packet& packet)>               ClientPacketMonitorCallback;

	// @throw AL::Exception
	typedef EventHandler<Void(ClientConnectionTypes type)>     ClientOnConnectEventHandler;
	typedef EventHandler<Void(ClientDisconnectReasons reason)> ClientOnDisconnectEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const String& value)>            ClientOnSendEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const String& value)>            ClientOnReceiveEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const Packet& packet)>           ClientOnSendPacketEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Packet& packet)>           ClientOnReceivePacketEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const Message& message)>         ClientOnSendMessageEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Message& message)>         ClientOnReceiveMessageEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const Position& position)>       ClientOnSendPositionEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Position& position)>       ClientOnReceivePositionEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const Telemetry& telemetry)>     ClientOnSendTelemetryEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Telemetry& telemetry)>     ClientOnReceiveTelemetryEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void(const String& value)>            ClientOnReceiveInvalidDataEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Packet& packet)>           ClientOnReceiveInvalidPacketEventHandler;

	class IClientConnection
	{
		IClientConnection(IClientConnection&&) = delete;
		IClientConnection(const IClientConnection&) = delete;

	public:
		IClientConnection()
		{
		}

		virtual ~IClientConnection()
		{
		}

		virtual Bool IsBlocking() const = 0;

		virtual Bool IsConnected() const = 0;

		virtual Void Disconnect() = 0;

		// @throw AL::Exception
		virtual Void SetBlocking(Bool set) = 0;

		// @throw AL::Exception
		// @return 0 on connection closed
		// @return -1 if would block
		virtual int Read(String& value) = 0;

		// @throw AL::Exception
		// @return AL::False on connection closed
		virtual Bool Write(const String& value) = 0;
	};

	class Client
	{
		class Connection_APRS_IS
			: public IClientConnection
		{
			static constexpr String::Char EOL[] = { '\r', '\n' };

			Network::TcpSocket socket;

		public:
			Connection_APRS_IS()
				: socket(
					Network::AddressFamilies::NotSpecified
				)
			{
			}

			virtual Bool IsBlocking() const override
			{
				return socket.IsBlocking();
			}

			virtual Bool IsConnected() const override
			{
				return socket.IsConnected();
			}

			// @throw AL::Exception
			// @return AL::False on timeout
			virtual Bool Connect(const Network::IPEndPoint& ep)
			{
				AL_ASSERT(
					!IsConnected(),
					"Connection_APRS_IS already connected"
				);

				Bool isBlocking = socket.IsBlocking();

				Network::TcpSocket socket(
					ep.Host.GetFamily()
				);

				socket.SetBlocking(
					isBlocking
				);

				try
				{
					socket.Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening Network::TcpSocket"
					);
				}

				try
				{
					if (!socket.Connect(ep))
					{
						socket.Close();

						return False;
					}
				}
				catch (Exception& exception)
				{
					socket.Close();

					throw Exception(
						Move(exception),
						"Error connecting Network::TcpSocket"
					);
				}

				this->socket = Move(
					socket
				);

				return True;
			}

			virtual Void Disconnect() override
			{
				if (IsConnected())
				{

					socket.Close();
				}
			}

			// @throw AL::Exception
			virtual Void SetBlocking(Bool set) override
			{
				socket.SetBlocking(set);
			}

			// @throw AL::Exception
			// @return 0 on connection closed
			// @return -1 if would block
			virtual int Read(String& value) override
			{
				AL_ASSERT(
					IsConnected(),
					"Connection_APRS_IS not connected"
				);

				String::Char buffer[2];
				size_t       numberOfBytesReceived;
				auto         buffer_Update = [&buffer]()
				{
					if ((buffer[0] == EOL[0]) && (buffer[1] == EOL[1]))
					{

						return False;
					}

					buffer[0] = buffer[1];

					return True;
				};

				try
				{
					if (!Network::SocketExtensions::TryReceiveAll(socket, &buffer[1], 1, numberOfBytesReceived))
					{
						Disconnect();

						return 0;
					}

					if (numberOfBytesReceived == 0)
					{

						return -1;
					}

					value.Clear();

					value.Append(
						buffer[1]
					);

					buffer_Update();

					do
					{
						if (!Network::SocketExtensions::ReceiveAll(socket, &buffer[1], 1, numberOfBytesReceived))
						{
							Disconnect();

							return 0;
						}

						value.Append(
							buffer[1]
						);
					} while (buffer_Update());

					value.Erase(
						value.GetLength() - 2,
						2
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error reading Network::TcpSocket"
					);
				}

				return 1;
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Write(const String& value) override
			{
				AL_ASSERT(
					IsConnected(),
					"Connection_APRS_IS not connected"
				);

				size_t numberOfBytesSent;

				try
				{
					if (!Network::SocketExtensions::SendAll(socket, value.GetCString(), value.GetLength(), numberOfBytesSent) ||
						!Network::SocketExtensions::SendAll(socket, EOL, sizeof(EOL), numberOfBytesSent))
					{

						return False;
					}
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error writing Network::TcpSocket"
					);
				}

				return True;
			}
		};

		class Connection_KISS
			: public IClientConnection
		{
			enum COMMANDS : uint8
			{
				// The following bytes should be transmitted by the TNC.
				// The maximum number of bytes, thus the size of the encapsulated packet, is determined by the amount of memory in the TNC.
				COMMAND_DATA             = 0x00,

				// The next byte is the time to hold up the TX after the FCS has been sent, in 10 ms units.
				// This command is obsolete, and is included here only for compatibility with some existing implementations.
				COMMAND_SET_TX_TAIL      = 0x04,
				// The next byte is the transmitter keyup delay in 10 ms units.
				// The default start-up value is 50 (i.e., 500 ms).
				COMMAND_SET_TX_DELAY     = 0x01,
				// The next byte is the slot interval in 10 ms units.
				// The default is 10 (i.e., 100ms).
				COMMAND_SET_SLOT_TIME    = 0x03,
				// The next byte is 0 for half duplex, nonzero for full duplex.
				// The default is 0 (i.e., half duplex).
				COMMAND_SET_FULL_DUPLEX  = 0x05,
				// Specific for each TNC.
				// In the TNC-1, this command sets the modem speed.
				// Other implementations may use this function for other hardware-specific functions.
				COMMAND_SET_HARDWARE     = 0x06,
				// The next byte is the persistence parameter, p, scaled to the range 0 - 255 with the following formula:
				// P = p * 256 - 1
				// The default value is P = 63 (i.e., p = 0.25).
				COMMAND_SET_PERSISTENCE  = 0x02,

				// Exit KISS and return control to a higher-level program.
				// This is useful only when KISS is incorporated into the TNC along with other applications.
				COMMAND_RETURN           = 0xFF
			};

			enum SPECIAL_CHARACTERS : uint8
			{
				SPECIAL_CHARACTER_FRAME_END               = 0xC0,
				SPECIAL_CHARACTER_FRAME_ESCAPE            = 0xDB,
				SPECIAL_CHARACTER_TRANSPOSED_FRAME_END    = 0xDC,
				SPECIAL_CHARACTER_TRANSPOSED_FRAME_ESCAPE = 0xDD
			};

			typedef Collections::Array<uint8> FrameBuffer;

			class IKISSConnection
			{
				IKISSConnection(IKISSConnection&&) = delete;
				IKISSConnection(const IKISSConnection&) = delete;

			public:
				IKISSConnection()
				{
				}

				virtual ~IKISSConnection()
				{
				}

				virtual Bool IsBlocking() const = 0;

				virtual Bool IsConnected() const = 0;

				virtual Void Disconnect() = 0;

				// @throw AL::Exception
				virtual Void SetBlocking(Bool set) = 0;

				// @throw AL::Exception
				// @return 0 on connection closed
				// @return -1 if would block
				virtual int ReadCommand(uint8& command, FrameBuffer& value) = 0;

				// @throw AL::Exception
				// @return AL::False on connection closed
				virtual Bool WriteCommand(uint8 command, const FrameBuffer& value) = 0;
			};

			class Tcp
				: public IKISSConnection
			{
				Network::TcpSocket socket;

			public:
				Tcp()
					: socket(
						Network::AddressFamilies::NotSpecified
					)
				{
				}

				virtual Bool IsBlocking() const override
				{
					return socket.IsBlocking();
				}

				virtual Bool IsConnected() const override
				{
					return socket.IsConnected();
				}

				// @throw AL::Exception
				// @return AL::False on timeout
				virtual Bool Connect(const Network::IPEndPoint& ep)
				{
					AL_ASSERT(
						!IsConnected(),
						"Connection_KISS::Tcp already connected"
					);

					Bool isBlocking = socket.IsBlocking();

					Network::TcpSocket socket(
						ep.Host.GetFamily()
					);

					socket.SetBlocking(
						isBlocking
					);

					try
					{
						socket.Open();
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error opening Network::TcpSocket"
						);
					}

					try
					{
						if (!socket.Connect(ep))
						{
							socket.Close();

							return False;
						}
					}
					catch (Exception& exception)
					{
						socket.Close();

						throw Exception(
							Move(exception),
							"Error connecting Network::TcpSocket"
						);
					}

					this->socket = Move(
						socket
					);

					return True;
				}

				virtual Void Disconnect() override
				{
					if (IsConnected())
					{

						socket.Close();
					}
				}

				// @throw AL::Exception
				virtual Void SetBlocking(Bool set) override
				{
					socket.SetBlocking(set);
				}

				// @throw AL::Exception
				// @return 0 on connection closed
				// @return -1 if would block
				virtual int ReadCommand(uint8& command, FrameBuffer& value) override
				{
					AL_ASSERT(
						IsConnected(),
						"Connection_KISS::Tcp not connected"
					);

					uint8  byte;
					size_t valueSize = 0;
					size_t numberOfBytesReceived;

					auto value_Append = [&value, &valueSize](uint8 _byte)
					{
						value.SetSize(++valueSize);
						value[valueSize - 1] = _byte;
					};

					try
					{
						{
							if (!Network::SocketExtensions::TryReceiveAll(socket, &byte, 1, numberOfBytesReceived))
							{
								Disconnect();

								return 0;
							}

							if (numberOfBytesReceived == 0)
							{

								return -1;
							}

							if (byte != SPECIAL_CHARACTER_FRAME_END)
							{
								Disconnect();

								return 0;
							}
						}

						if (!Network::SocketExtensions::ReceiveAll(socket, &command, sizeof(command), numberOfBytesReceived))
						{
							Disconnect();

							return 0;
						}

						{
							do
							{
								if (!Network::SocketExtensions::ReceiveAll(socket, &byte, 1, numberOfBytesReceived))
								{
									Disconnect();

									return 0;
								}

								if (byte == SPECIAL_CHARACTER_FRAME_ESCAPE)
								{
									if (!Network::SocketExtensions::ReceiveAll(socket, &byte, 1, numberOfBytesReceived))
									{
										Disconnect();

										return 0;
									}

									switch (byte)
									{
										case SPECIAL_CHARACTER_FRAME_ESCAPE:
											return -1;

										case SPECIAL_CHARACTER_TRANSPOSED_FRAME_END:
											value_Append(SPECIAL_CHARACTER_FRAME_END);
											continue;

										case SPECIAL_CHARACTER_TRANSPOSED_FRAME_ESCAPE:
											value_Append(SPECIAL_CHARACTER_FRAME_ESCAPE);
											continue;

										default:
											Disconnect();
											return 0;
									}
								}
								else if (byte != SPECIAL_CHARACTER_FRAME_END)
								{
									value_Append(
										byte
									);
								}
							} while (byte != SPECIAL_CHARACTER_FRAME_END);
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error reading Network::TcpSocket"
						);
					}

					return 1;
				}

				// @throw AL::Exception
				// @return AL::False on connection closed
				virtual Bool WriteCommand(uint8 command, const FrameBuffer& value) override
				{
					AL_ASSERT(
						IsConnected(),
						"Connection_KISS::Tcp not connected"
					);

					auto function = [this, command, &value]()
					{
						if (!WriteOnce(command, False))
						{

							return False;
						}

						for (auto c : value)
						{
							if (!WriteOnce(static_cast<uint8>(c), True))
							{

								return False;
							}
						}

						return True;
					};

					if (!WriteFrame(function))
					{

						return False;
					}

					return True;
				}

			private:
				// @throw AL::Exception
				// @return AL::False on connection closed
				Bool WriteOnce(uint8 value, Bool escape)
				{
					size_t numberOfBytesSent;

					if (escape)
					{
						uint8 buffer[2] =
						{
							SPECIAL_CHARACTER_FRAME_ESCAPE
						};

						try
						{
							if (value == SPECIAL_CHARACTER_FRAME_END)
							{
								buffer[1] = SPECIAL_CHARACTER_TRANSPOSED_FRAME_END;

								if (!Network::SocketExtensions::SendAll(socket, &buffer[0], sizeof(buffer), numberOfBytesSent))
								{
									Disconnect();

									return False;
								}
							}
							else if (value == SPECIAL_CHARACTER_FRAME_ESCAPE)
							{
								buffer[1] = SPECIAL_CHARACTER_TRANSPOSED_FRAME_ESCAPE;

								if (!Network::SocketExtensions::SendAll(socket, &buffer[0], sizeof(buffer), numberOfBytesSent))
								{
									Disconnect();

									return False;
								}
							}
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error writing Network::TcpSocket"
							);
						}
					}

					if (!Network::SocketExtensions::SendAll(socket, &value, sizeof(value), numberOfBytesSent))
					{
						Disconnect();

						return False;
					}

					return True;
				}

				// @throw AL::Exception
				// @return AL::False on connection closed
				template<typename F>
				Bool WriteFrame(F&& function)
				{
					uint8  eof = SPECIAL_CHARACTER_FRAME_END;
					size_t numberOfBytesSent;

					try
					{
						if (!Network::SocketExtensions::SendAll(socket, &eof, sizeof(eof), numberOfBytesSent))
						{
							Disconnect();

							return False;
						}

						if (!function())
						{
							Disconnect();

							return False;
						}

						if (!Network::SocketExtensions::SendAll(socket, &eof, sizeof(eof), numberOfBytesSent))
						{
							Disconnect();

							return False;
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error writing Network::TcpSocket"
						);
					}

					return True;
				}
			};

			class Serial
				: public IKISSConnection
			{
				Bool                  isBlocking  = False;
				Bool                  isConnected = False;

				Hardware::UARTDevice* lpDevice;

			public:
				Serial()
				{
				}

				virtual Bool IsBlocking() const override
				{
					return isBlocking;
				}

				virtual Bool IsConnected() const override
				{
					return isConnected;
				}

				// @throw AL::Exception
#if defined(AL_PLATFORM_PICO)
				virtual Void Connect(::uart_inst_t* uart, uint8 rx, uint8 tx, uint32 speed)
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
				virtual Void Connect(const String& device, uint32 speed)
#endif
				{
					AL_ASSERT(
						!IsConnected(),
						"Connection_KISS::Serial already connected"
					);

#if defined(AL_PLATFORM_PICO)
					lpDevice = new Hardware::UARTDevice(
						uart,
						rx,
						tx,
						speed,
						Hardware::UARTDeviceFlags::None
					);
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
					lpDevice = new Hardware::UARTDevice(
						FileSystem::Path(device),
						speed,
						Hardware::UARTDeviceFlags::None
					);
#endif

					try
					{
						lpDevice->Open();
					}
					catch (Exception& exception)
					{
						delete lpDevice;

						throw Exception(
							Move(exception),
							"Error opening Hardware::UARTDevice"
						);
					}

					isConnected = True;
				}

				virtual Void Disconnect() override
				{
					if (IsConnected())
					{
						lpDevice->Close();
						delete lpDevice;

						isConnected = False;
					}
				}

				// @throw AL::Exception
				virtual Void SetBlocking(Bool set) override
				{
					isBlocking = set;
				}

				// @throw AL::Exception
				// @return 0 on connection closed
				// @return -1 if would block
				virtual int ReadCommand(uint8& command, FrameBuffer& value) override
				{
					AL_ASSERT(
						IsConnected(),
						"Connection_KISS::Serial not connected"
					);

					uint8  byte;
					size_t valueSize = 0;
					size_t numberOfBytesReceived;

					auto value_Append = [&value, &valueSize](uint8 _byte)
					{
						value.SetSize(++valueSize);
						value[valueSize - 1] = _byte;
					};

					auto device_TryReadAll = [this](void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
					{
						if ((numberOfBytesReceived = lpDevice->TryRead(lpBuffer, size)) != 0)
						{
							for (size_t totalBytesReceived = numberOfBytesReceived; totalBytesReceived < size; totalBytesReceived += numberOfBytesReceived)
							{
								numberOfBytesReceived = lpDevice->TryRead(
									&reinterpret_cast<uint8*>(lpBuffer)[totalBytesReceived],
									size - totalBytesReceived
								);
							}
						}
					};

					try
					{
						{
							device_TryReadAll(&byte, 1, numberOfBytesReceived);

							if (numberOfBytesReceived == 0)
							{

								return -1;
							}

							if (byte != SPECIAL_CHARACTER_FRAME_END)
							{
								Disconnect();

								return 0;
							}
						}

						lpDevice->Read(&command, sizeof(command));

						{
							do
							{
								lpDevice->Read(&byte, 1);

								if (byte == SPECIAL_CHARACTER_FRAME_ESCAPE)
								{
									lpDevice->Read(&byte, 1);

									switch (byte)
									{
										case SPECIAL_CHARACTER_FRAME_ESCAPE:
											return -1;

										case SPECIAL_CHARACTER_TRANSPOSED_FRAME_END:
											value_Append(SPECIAL_CHARACTER_FRAME_END);
											continue;

										case SPECIAL_CHARACTER_TRANSPOSED_FRAME_ESCAPE:
											value_Append(SPECIAL_CHARACTER_FRAME_ESCAPE);
											continue;

										default:
											Disconnect();
											return 0;
									}
								}
								else if (byte != SPECIAL_CHARACTER_FRAME_END)
								{
									value_Append(
										byte
									);
								}
							} while (byte != SPECIAL_CHARACTER_FRAME_END);
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error reading Hardware::UARTDevice"
						);
					}

					return 1;
				}

				// @throw AL::Exception
				// @return AL::False on connection closed
				virtual Bool WriteCommand(uint8 command, const FrameBuffer& value) override
				{
					AL_ASSERT(
						IsConnected(),
						"Connection_KISS::Serial not connected"
					);

					WriteFrame([this, command, &value]()
					{
						WriteOnce(command, False);

						for (auto c : value)
						{
							WriteOnce(
								static_cast<uint8>(c),
								True
							);
						}

						return True;
					});

					return True;
				}

			private:
				// @throw AL::Exception
				Void WriteOnce(uint8 value, Bool escape)
				{
					if (escape)
					{
						uint8 buffer[2] =
						{
							SPECIAL_CHARACTER_FRAME_ESCAPE
						};

						try
						{
							switch (value)
							{
								case SPECIAL_CHARACTER_FRAME_END:
									buffer[1] = SPECIAL_CHARACTER_TRANSPOSED_FRAME_END;
									lpDevice->Write(buffer);
									return;

								case SPECIAL_CHARACTER_FRAME_ESCAPE:
									buffer[1] = SPECIAL_CHARACTER_TRANSPOSED_FRAME_ESCAPE;
									lpDevice->Write(buffer);
									return;
							}
						}
						catch (Exception& exception)
						{

							throw Exception(
								Move(exception),
								"Error writing Hardware::UARTDevice"
							);
						}
					}

					try
					{
						lpDevice->Write(
							value
						);
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error writing Hardware::UARTDevice"
						);
					}
				}

				// @throw AL::Exception
				template<typename F>
				Void WriteFrame(F&& function)
				{
					try
					{
						lpDevice->Write(
							SPECIAL_CHARACTER_FRAME_END
						);

						function();

						lpDevice->Write(
							SPECIAL_CHARACTER_FRAME_END
						);
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error writing Hardware::UARTDevice"
						);
					}
				}
			};

			Bool             isBlocking  = True;
			Bool             isConnected = False;

			IKISSConnection* lpConnection = nullptr;

		public:
			Connection_KISS()
			{
			}

			virtual ~Connection_KISS()
			{
				if (IsConnected())
				{

					Disconnect();
				}
			}

			virtual Bool IsBlocking() const override
			{
				return isBlocking;
			}

			virtual Bool IsConnected() const override
			{
				return isConnected;
			}

			virtual Void Disconnect() override
			{
				if (IsConnected())
				{
					lpConnection->Disconnect();

					delete lpConnection;
					lpConnection = nullptr;

					isConnected = False;
				}
			}

			// @throw AL::Exception
			virtual Void SetBlocking(Bool set) override
			{
				if (IsConnected())
				{

					lpConnection->SetBlocking(
						set
					);
				}

				isBlocking = set;
			}

			// @throw AL::Exception
			// @return AL::False on timeout
			virtual Bool ConnectTcp(const Network::IPEndPoint& ep)
			{
				AL_ASSERT(
					!IsConnected(),
					"Connection_KISS already connected"
				);

				auto lpConnection = new Tcp();
				lpConnection->SetBlocking(IsBlocking());

				try
				{
					if (!lpConnection->Connect(ep))
					{
						delete lpConnection;

						return False;
					}
				}
				catch (Exception& exception)
				{
					delete lpConnection;

					throw;
				}

				this->lpConnection = lpConnection;

				isConnected = True;

				return True;
			}

			// @throw AL::Exception
#if defined(AL_PLATFORM_PICO)
			virtual Void ConnectSerial(::uart_inst_t* uart, uint8 rx, uint8 tx, uint32 speed)
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			virtual Void ConnectSerial(const String& device, uint32 speed)
#endif
			{
				AL_ASSERT(
					!IsConnected(),
					"Connection_KISS already connected"
				);

				auto lpConnection = new Serial();
				lpConnection->SetBlocking(IsBlocking());

				try
				{
#if defined(AL_PLATFORM_PICO)
					lpConnection->Connect(uart, rx, tx, speed);
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
					lpConnection->Connect(device, speed);
#endif
				}
				catch (Exception& exception)
				{
					delete lpConnection;

					throw;
				}

				this->lpConnection = lpConnection;

				isConnected = True;
			}

			// @throw AL::Exception
			// @return 0 on connection closed
			// @return -1 if would block
			virtual int Read(String& value) override
			{
				AL_ASSERT(
					IsConnected(),
					"Connection_KISS not connected"
				);

				String source, tocall, path;

				switch (ReadFrame(source, tocall, path, value))
				{
					case 0:  return 0;
					case -1: return -1;
				}

				value = String::Format(
					"%s>%s,%s:%s",
					source.GetCString(),
					tocall.GetCString(),
					path.GetCString(),
					value.GetCString()
				);

				return 1;
			}

			// @throw AL::Exception
			// @return AL::False on connection closed
			virtual Bool Write(const String& value) override
			{
				AL_ASSERT(
					IsConnected(),
					"Connection_KISS not connected"
				);

				Regex::MatchCollection matches;

				if (!Regex::Match(matches, "^([^>]+)>([^,]+),([^:]+):(.+)$", value))
				{

					throw Exception(
						"Invalid input"
					);
				}

				return WriteFrame(matches[1], matches[2], matches[3], matches[4]);
			}

		private:
			// @throw AL::Exception
			// @return 0 on connection closed
			// @return -1 if would block
			int  ReadFrame(String& source, String& tocall, String& path, String& value)
			{
				FrameBuffer buffer;
				uint8       command;

				switch (lpConnection->ReadCommand(command, buffer))
				{
					case 0:
						Disconnect();
						return 0;

					case -1:
						return -1;
				}

				if ((command & 0x0F) != COMMAND_DATA)
				{

					return -1;
				}

				auto buffer_ReadStation = [&buffer](String& value, size_t offset)
				{
					auto lpBuffer      = &buffer[offset];
					Bool isLastStation = (buffer[offset + 6] & 0x01) == 0x01;

					for (size_t i = 0; offset < buffer.GetSize(); ++offset, ++i, ++lpBuffer)
					{
						*lpBuffer >>= 1;

						if ((i == 6) || (*lpBuffer == 0x20))
						{
							uint8 ssid = *lpBuffer & 0x0F;
							*lpBuffer  = String::END;
							value      = reinterpret_cast<const char*>(&buffer[offset - i]);

							if (ssid != 0)
							{
								if (value.Compare("WIDE1", True))
								{
									value = String::Format(
										"%s1-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE2", True))
								{
									value = String::Format(
										"%s2-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE3", True))
								{
									value = String::Format(
										"%s3-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE4", True))
								{
									value = String::Format(
										"%s4-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE5", True))
								{
									value = String::Format(
										"%s5-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE6", True))
								{
									value = String::Format(
										"%s6-%u",
										value.GetCString(),
										ssid
									);
								}
								else if (value.Compare("WIDE7", True))
								{
									value = String::Format(
										"%s7-%u",
										value.GetCString(),
										ssid
									);
								}
								else
								{
									value = String::Format(
										"%s-%u",
										value.GetCString(),
										ssid
									);
								}
							}

							return isLastStation ? -1 : 1;
						}
					}

					return 0;
				};

				if (!buffer_ReadStation(tocall, 0) || !buffer_ReadStation(source, 7))
				{

					return -1;
				}

				size_t pathSize = 0;
				{
					StringBuilder sb;
					int           result;

					while ((result = buffer_ReadStation(path, 14 + (pathSize * 7))) != 0)
					{
						if (pathSize++ != 0)
							sb << ',';

						sb << path;

						if (result == -1)
							break;
					}

					path = sb.ToString();
				}

				if ((buffer[14 + (pathSize * 7)] != 0x03) || (buffer[14 + (pathSize * 7) + 1] != 0xF0))
				{

					return -1;
				}

				value.Assign(
					reinterpret_cast<const char*>(&buffer[14 + (pathSize * 7) + 2]),
					buffer.GetSize() - (14 + (pathSize * 7) + 2)
				);

				return 1;
			}
			// @throw AL::Exception
			// @return AL::False on connection closed
			Bool WriteFrame(const String& source, const String& tocall, const String& path, const String& value)
			{
				// TODO: implement
				throw NotImplementedException();
			}
		};

		struct _PacketMonitorContext
		{
			ClientPacketFilterCallback  Filter;
			ClientPacketMonitorCallback Callback;
		};

		struct _MessageCallbackContext
		{
			String                Station;
			ClientMessageCallback Callback;
			String                MessageId;
		};

		static constexpr const String::Char SOFTWARE_NAME[]    = "AL.APRS";
		static constexpr const String::Char SOFTWARE_VERSION[] = "0.1";

		Bool                                             isBlocking           = False;
		Bool                                             isConnected          = False;
		Bool                                             isAuthenticated      = False;
		Bool                                             isMonitorModeEnabled = False;
		Bool                                             isCompressionEnabled = False;

		DigiPath                                         path;
		String                                           station;
		String::Char                                     symbolTable;
		String::Char                                     symbolTableKey;

		IClientConnection*                               lpConnection;
		ClientConnectionTypes                            connectionType = ClientConnectionTypes::None;

		Collections::LinkedList<_PacketMonitorContext*>  packetMonitors;
		Collections::LinkedList<_MessageCallbackContext> messageCallbacks;

		Client(const Client&) = delete;

	public:
		// @throw AL::Exception
		Event<ClientOnConnectEventHandler>              OnConnect;
		Event<ClientOnDisconnectEventHandler>           OnDisconnect;

		// @throw AL::Exception
		Event<ClientOnSendEventHandler>                 OnSend;
		// @throw AL::Exception
		Event<ClientOnReceiveEventHandler>              OnReceive;

		// @throw AL::Exception
		Event<ClientOnSendPacketEventHandler>           OnSendPacket;
		// @throw AL::Exception
		Event<ClientOnReceivePacketEventHandler>        OnReceivePacket;

		// @throw AL::Exception
		Event<ClientOnSendMessageEventHandler>          OnSendMessage;
		// @throw AL::Exception
		Event<ClientOnReceiveMessageEventHandler>       OnReceiveMessage;

		// @throw AL::Exception
		Event<ClientOnSendPositionEventHandler>         OnSendPosition;
		// @throw AL::Exception
		Event<ClientOnReceivePositionEventHandler>      OnReceivePosition;

		// @throw AL::Exception
		Event<ClientOnSendTelemetryEventHandler>        OnSendTelemetry;
		// @throw AL::Exception
		Event<ClientOnReceiveTelemetryEventHandler>     OnReceiveTelemetry;

		// @throw AL::Exception
		Event<ClientOnReceiveInvalidDataEventHandler>   OnReceiveInvalidData;
		// @throw AL::Exception
		Event<ClientOnReceiveInvalidPacketEventHandler> OnReceiveInvalidPacket;

		Client(Client&& client)
			: isBlocking(
				client.isBlocking
			),
			isConnected(
				client.isConnected
			),
			isAuthenticated(
				client.isAuthenticated
			),
			isMonitorModeEnabled(
				client.isMonitorModeEnabled
			),
			isCompressionEnabled(
				client.isCompressionEnabled
			),
			path(
				Move(client.path)
			),
			station(
				Move(client.station)
			),
			symbolTable(
				client.symbolTable
			),
			symbolTableKey(
				client.symbolTableKey
			),
			lpConnection(
				client.lpConnection
			),
			connectionType(
				client.connectionType
			),
			packetMonitors(
				Move(client.packetMonitors)
			),
			messageCallbacks(
				Move(client.messageCallbacks)
			),
			OnConnect(
				Move(client.OnConnect)
			),
			OnDisconnect(
				Move(client.OnDisconnect)
			),
			OnSend(
				Move(client.OnSend)
			),
			OnReceive(
				Move(client.OnReceive)
			),
			OnSendPacket(
				Move(client.OnSendPacket)
			),
			OnReceivePacket(
				Move(client.OnReceivePacket)
			),
			OnSendMessage(
				Move(client.OnSendMessage)
			),
			OnReceiveMessage(
				Move(client.OnReceiveMessage)
			),
			OnSendPosition(
				Move(client.OnSendPosition)
			),
			OnReceivePosition(
				Move(client.OnReceivePosition)
			),
			OnSendTelemetry(
				Move(client.OnSendTelemetry)
			),
			OnReceiveTelemetry(
				Move(client.OnReceiveTelemetry)
			),
			OnReceiveInvalidData(
				Move(client.OnReceiveInvalidData)
			),
			OnReceiveInvalidPacket(
				Move(client.OnReceiveInvalidPacket)
			)
		{
			client.isBlocking           = False;
			client.isConnected          = False;
			client.isAuthenticated      = False;
			client.isMonitorModeEnabled = False;
			client.isCompressionEnabled = False;
			client.connectionType       = ClientConnectionTypes::None;

			OnDisconnect.Register([this](ClientDisconnectReasons _reason)
			{
				messageCallbacks.Clear();
			});
		}

		Client(String&& station, DigiPath&& path, String::Char symbolTable, String::Char symbolTableKey)
			: path(
				Move(path)
			),
			station(
				Move(station)
			),
			symbolTable(
				symbolTable
			),
			symbolTableKey(
				symbolTableKey
			)
		{
			SetBlocking(False);
		}

		virtual ~Client()
		{
			if (IsConnected())
			{

				Disconnect();
			}

			for (auto it = packetMonitors.rbegin(); it != packetMonitors.rend(); )
			{
				delete *it;

				packetMonitors.Erase(it++);
			}
		}

		Bool IsBlocking() const
		{
			return isBlocking;
		}

		Bool IsConnected() const
		{
			return isConnected;
		}

		Bool IsAuthenticated() const
		{
			return isAuthenticated;
		}

		Bool IsMonitorModeEnabled() const
		{
			return isMonitorModeEnabled;
		}

		Bool IsCompressionEnabled() const
		{
			return isCompressionEnabled;
		}

		auto& GetPath() const
		{
			return path;
		}

		auto& GetStation() const
		{
			return station;
		}

		auto GetSymbolTable() const
		{
			return symbolTable;
		}

		auto GetSymbolTableKey() const
		{
			return symbolTableKey;
		}

		Void EnableMonitorMode(Bool set = True)
		{
			isMonitorModeEnabled = set;
		}

		Void EnableCompression(Bool set = True)
		{
			isCompressionEnabled = set;
		}

		Void SetPath(DigiPath&& value)
		{
			path = Move(
				value
			);
		}

		Void SetSymbol(String::Char table, String::Char key)
		{
			symbolTable    = table;
			symbolTableKey = key;
		}

		// @throw AL::Exception
		Void SetBlocking(Bool set = True)
		{
			if (IsConnected())
			{
				lpConnection->SetBlocking(
					set
				);
			}

			isBlocking = set;
		}

		Void AddPacketMonitor(ClientPacketFilterCallback&& filter, ClientPacketMonitorCallback&& callback)
		{
			auto lpContext = new _PacketMonitorContext
			{
				.Filter   = Move(filter),
				.Callback = Move(callback)
			};

			packetMonitors.PushBack(lpContext);
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		template<typename T_CONNECTION, typename ... TArgs>
		Bool Connect(TArgs ... args)
		{
			static_assert(
				Is_Base_Of<IClientConnection, T_CONNECTION>::Value,
				"T_CONNECTION must inherit IClientConnection"
			);

			AL_ASSERT(
				!IsConnected(),
				"Client already connected"
			);

			auto lpConnection = new T_CONNECTION();
			lpConnection->SetBlocking(IsBlocking());

			try
			{
				if (!lpConnection->Connect(Forward<TArgs>(args) ...))
				{
					delete lpConnection;

					return False;
				}
			}
			catch (Exception& exception)
			{
				delete lpConnection;

				throw Exception(
					Move(exception),
					"Error connecting to APRS"
				);
			}

			this->lpConnection = lpConnection;
			connectionType     = ClientConnectionTypes::UserDefined;
			isConnected        = True;

			try
			{
				OnConnect.Execute(
					ClientConnectionTypes::UserDefined
				);
			}
			catch (Exception&)
			{
				lpConnection->Disconnect();
				delete lpConnection;

				connectionType = ClientConnectionTypes::None;

				isConnected = False;

				throw;
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool ConnectIS(const Network::IPEndPoint& ep, uint16 passcode, const String& filter)
		{
			AL_ASSERT(
				!IsConnected(),
				"Client already connected"
			);

			auto lpConnection = new Connection_APRS_IS();
			lpConnection->SetBlocking(IsBlocking());

			try
			{
				if (!lpConnection->Connect(ep))
				{
					delete lpConnection;

					return False;
				}
			}
			catch (Exception& exception)
			{
				delete lpConnection;

				throw Exception(
					Move(exception),
					"Error connecting to APRS-IS"
				);
			}

			this->lpConnection = lpConnection;
			connectionType     = ClientConnectionTypes::APRS_IS;
			isConnected        = True;

			try
			{
				switch (Authenticate(passcode, filter))
				{
					case 0:
						Connection_Disconnect(ClientDisconnectReasons::ConnectionLost);
						return False;

					case -1:
						Connection_Disconnect(ClientDisconnectReasons::AuthenticationFailed);
						return False;
				}
			}
			catch (Exception& exception)
			{
				lpConnection->Disconnect();
				delete lpConnection;

				connectionType = ClientConnectionTypes::None;

				isConnected = False;

				throw Exception(
					Move(exception),
					"Error sending authentication"
				);
			}

			isAuthenticated = True;

			try
			{
				OnConnect.Execute(
					ClientConnectionTypes::APRS_IS
				);
			}
			catch (Exception&)
			{
				lpConnection->Disconnect();
				delete lpConnection;

				connectionType = ClientConnectionTypes::None;

				isConnected = False;

				throw;
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
#if defined(AL_PLATFORM_PICO)
		Bool ConnectKISS(::uart_inst_t* uart, uint8 rx, uint8 tx, uint32 speed)
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
		Bool ConnectKISS(const String& device, uint32 speed)
#endif
		{
			AL_ASSERT(
				!IsConnected(),
				"Client already connected"
			);

			auto lpConnection = new Connection_KISS();
			lpConnection->SetBlocking(IsBlocking());

			try
			{
#if defined(AL_PLATFORM_PICO)
				lpConnection->ConnectSerial(
					uart,
					rx,
					tx,
					speed
				);
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
				lpConnection->ConnectSerial(
					device,
					speed
				);
#endif
			}
			catch (Exception& exception)
			{
				delete lpConnection;

				throw Exception(
					Move(exception),
					"Error connecting to serial KISS TNC"
				);
			}

			this->lpConnection = lpConnection;
			connectionType     = ClientConnectionTypes::KISS_Serial;
			isConnected        = True;
			isAuthenticated    = True;

			try
			{
				OnConnect.Execute(
					ClientConnectionTypes::KISS_Serial
				);
			}
			catch (Exception&)
			{
				lpConnection->Disconnect();
				delete lpConnection;

				connectionType = ClientConnectionTypes::None;

				isConnected = False;

				throw;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		Bool ConnectKISS(const Network::IPEndPoint& ep)
		{
			AL_ASSERT(
				!IsConnected(),
				"Client already connected"
			);

			auto lpConnection = new Connection_KISS();
			lpConnection->SetBlocking(IsBlocking());

			try
			{
				if (!lpConnection->ConnectTcp(ep))
				{
					delete lpConnection;

					return False;
				}
			}
			catch (Exception& exception)
			{
				delete lpConnection;

				throw Exception(
					Move(exception),
					"Error connecting to tcp KISS TNC"
				);
			}

			this->lpConnection = lpConnection;
			connectionType     = ClientConnectionTypes::KISS_Tcp;
			isConnected        = True;
			isAuthenticated    = True;

			try
			{
				OnConnect.Execute(
					ClientConnectionTypes::KISS_Tcp
				);
			}
			catch (Exception&)
			{
				lpConnection->Disconnect();
				delete lpConnection;

				connectionType = ClientConnectionTypes::None;

				isConnected = False;

				throw;
			}

			return True;
		}

		Void Disconnect()
		{
			if (IsConnected())
			{
				Connection_Disconnect(
					ClientDisconnectReasons::UserRequested
				);
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Update(TimeSpan delta)
		{
			String line;

			switch (Connection_Read(line))
			{
				case 0:  return False;
				case -1: return True;
			}

			if (line.StartsWith('#'))
			{
				Regex::MatchCollection matches;

				if (Regex::Match(matches, "^# logresp ([^ ]+) ([^ ,]+)", line))
				{
					if (!matches[2].Compare("verified", True))
					{
						Connection_Disconnect(
							ClientDisconnectReasons::AuthenticationFailed
						);

						return False;
					}
				}
			}
			else
			{
				Packet packet;

				try
				{
					packet = Packet::FromString(
						line
					);
				}
				catch (const Exception&)
				{
					OnReceiveInvalidData.Execute(
						line
					);

					return True;
				}

				OnReceivePacket.Execute(
					packet
				);

				for (auto packetMonitor : packetMonitors)
				{
					if (packetMonitor->Filter(packet))
					{
						packetMonitor->Callback(packet);
					}
				}

				switch (packet.GetType())
				{
					case PacketTypes::Unknown:
						break;

					case PacketTypes::Weather:
						// TODO: implement
						break;

					case PacketTypes::Message:
					{
						Message message;

						try
						{
							message = Message::FromPacket(
								Move(packet)
							);
						}
						catch (const Exception&)
						{
							OnReceiveInvalidPacket.Execute(
								packet
							);

							break;
						}

						if (message.GetContent().Compare("ack", True) && (message.GetId().GetLength() != 0))
						{
							if (message.GetDestination().Compare(GetStation(), True))
							{
								ClientMessageCallback callback;

								if (FindScheduledMessageCallback(callback, message.GetSender(), message.GetId()))
									callback();
							}
						}
						else if (message.GetDestination().Compare(GetStation(), True))
						{
							OnReceiveMessage.Execute(
								message
							);

							if ((message.GetId().GetLength() != 0) && !SendMessageAck(message.GetSender(), message.GetId()))
							{

								return False;
							}
						}
						else if (IsMonitorModeEnabled())
						{
							OnReceiveMessage.Execute(
								message
							);
						}
					}
					break;

					case PacketTypes::Position:
					{
						Position position;

						try
						{
							position = Position::FromPacket(
								Move(packet)
							);
						}
						catch (const Exception&)
						{
							OnReceiveInvalidPacket.Execute(
								packet
							);

							break;
						}

						OnReceivePosition.Execute(
							position
						);
					}
					break;

					case PacketTypes::Telemetry:
					{
						Telemetry telemetry;

						try
						{
							telemetry = Telemetry::FromPacket(
								Move(packet)
							);
						}
						catch (const Exception&)
						{
							OnReceiveInvalidPacket.Execute(
								packet
							);

							break;
						}

						OnReceiveTelemetry.Execute(
							telemetry
						);
					}
					break;
				}
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const String& value)
		{
			if (!Connection_Write(value))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const Packet& value)
		{
			auto line = value.ToString(False);

			if (!Connection_Write(line))
			{

				return False;
			}

			OnSendPacket.Execute(
				value
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const Message& value)
		{
			auto line = value.ToString(False);

			if (!Connection_Write(line))
			{

				return False;
			}

			if (!value.GetContent().Compare("ack", True) || (value.GetId().GetLength() == 0))
			{
				OnSendMessage.Execute(
					value
				);
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const Position& value)
		{
			auto line = value.ToString(False);

			if (!Connection_Write(line))
			{

				return False;
			}

			OnSendPosition.Execute(
				value
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const Telemetry& value)
		{
			auto line = value.ToString(False);

			if (!Connection_Write(line))
			{

				return False;
			}

			OnSendTelemetry.Execute(
				value
			);

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendPacket(String&& tocall, String&& content)
		{
			auto packet = Packet::Create(
				String(GetStation()),
				Move(tocall),
				DigiPath(GetPath()),
				Move(content)
			);

			return Send(
				packet
			);
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendPacket(const String& tocall, const String& content)
		{
			return SendPacket(String(tocall), String(content));
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(String&& destination, String&& content)
		{
			auto message = Message::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				Move(destination),
				Move(content)
			);

			if (!Send(message.ToPacket(False)))
			{

				return False;
			}

			OnSendMessage.Execute(
				message
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(const String& destination, const String& content)
		{
			return SendMessage(String(destination), String(content));
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(String&& destination, String&& content, String&& id)
		{
			auto message = Message::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				Move(destination),
				Move(content),
				Move(id)
			);

			if (!Send(message.ToPacket(False)))
			{

				return False;
			}

			OnSendMessage.Execute(
				message
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(const String& destination, const String& content, const String& id)
		{
			return SendMessage(String(destination), String(content), String(id));
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(String&& destination, String&& content, String&& id, ClientMessageCallback&& callback)
		{
			auto message = Message::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				Move(destination),
				Move(content),
				Move(id)
			);

			ScheduleMessageCallback(
				message.GetDestination(),
				message.GetId(),
				Move(callback)
			);

			if (!Send(message.ToPacket(False)))
			{
				FindScheduledMessageCallback(
					callback,
					message.GetDestination(),
					message.GetId()
				);

				return False;
			}

			OnSendMessage.Execute(
				message
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessage(const String& destination, const String& content, const String& id, ClientMessageCallback&& callback)
		{
			return SendMessage(String(destination), String(content), String(id), Move(callback));
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendPosition(int32 altitude, Float latitude, Float longitude)
		{
			return SendPosition(altitude, latitude, longitude, "");
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendPosition(int32 altitude, Float latitude, Float longitude, String&& comment)
		{
			auto position = Position::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				altitude,
				latitude,
				longitude,
				GetSymbolTable(),
				GetSymbolTableKey(),
				Move(comment),
				True,
				IsCompressionEnabled()
			);

			if (!Send(position.ToPacket(False)))
			{

				return False;
			}

			OnSendPosition.Execute(
				position
			);

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendPosition(int32 altitude, Float latitude, Float longitude, const String& comment)
		{
			return SendPosition(altitude, latitude, longitude, String(comment));
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendTelemetry(const uint8(&analog)[5], const Bool(&digital)[8], uint16 sequence)
		{
			auto telemetry = Telemetry::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				analog,
				digital,
				sequence
			);

			if (!Send(telemetry.ToPacket(False)))
			{

				return False;
			}

			OnSendTelemetry.Execute(
				telemetry
			);

			return True;
		}

		Client& operator = (Client&& client)
		{
			if (IsConnected())
			{

				Disconnect();
			}

			isBlocking = client.isBlocking;
			client.isBlocking = False;

			isConnected = client.isConnected;
			client.isConnected = False;

			isAuthenticated = client.isAuthenticated;
			client.isAuthenticated = False;

			isMonitorModeEnabled = client.isMonitorModeEnabled;
			client.isMonitorModeEnabled = False;

			isCompressionEnabled = client.isCompressionEnabled;
			client.isCompressionEnabled = False;

			path             = Move(client.path);
			station          = Move(client.station);
			symbolTable      = client.symbolTable;
			symbolTableKey   = client.symbolTableKey;

			lpConnection     = client.lpConnection;

			connectionType   = client.connectionType;
			client.connectionType = ClientConnectionTypes::None;

			packetMonitors   = Move(client.packetMonitors);
			messageCallbacks = Move(client.messageCallbacks);

			OnConnect              = Move(client.OnConnect);
			OnDisconnect           = Move(client.OnDisconnect);
			OnSend                 = Move(client.OnSend);
			OnReceive              = Move(client.OnReceive);
			OnSendPacket           = Move(client.OnSendPacket);
			OnReceivePacket        = Move(client.OnReceivePacket);
			OnSendMessage          = Move(client.OnSendMessage);
			OnReceiveMessage       = Move(client.OnReceiveMessage);
			OnSendPosition         = Move(client.OnSendPosition);
			OnReceivePosition      = Move(client.OnReceivePosition);
			OnSendTelemetry        = Move(client.OnSendTelemetry);
			OnReceiveTelemetry     = Move(client.OnReceiveTelemetry);
			OnReceiveInvalidData   = Move(client.OnReceiveInvalidData);
			OnReceiveInvalidPacket = Move(client.OnReceiveInvalidPacket);

			return *this;
		}

	private:
		Void ScheduleMessageCallback(const String& station, const String& messageId, ClientMessageCallback&& callback)
		{
			messageCallbacks.PushBack(
				{
					.Station   = station,
					.Callback  = Move(callback),
					.MessageId = messageId
				}
			);
		}

		Bool FindScheduledMessageCallback(ClientMessageCallback& value, const String& station, const String& messageId)
		{
			for (auto it = messageCallbacks.begin(); it != messageCallbacks.end(); )
			{
				if (it->Station.Compare(station, True) && it->MessageId.Compare(messageId))
				{
					value = Move(
						it->Callback
					);

					messageCallbacks.Erase(
						it
					);

					return True;
				}
			}

			return True;
		}

	private:
		// @throw AL::Exception
		// @return 0 on connection closed
		// @return -1 on authentication failed
		int Authenticate(uint16 passcode, const String& filter)
		{
			auto line = String::Format(
				"user %s pass %u vers %s %s filter %s",
				GetStation().GetCString(),
				passcode,
				SOFTWARE_NAME,
				SOFTWARE_VERSION,
				filter.GetCString()
			);

			if (!Connection_Write(line))
			{

				return 0;
			}

			int result;

			while ((result = Connection_Read(line)) != 0)
			{
				if (result == -1)
				{

					continue;
				}

				Regex::MatchCollection matches;

				if (Regex::Match(matches, "^# logresp ([^ ]+) (.+)$", line))
				{
					if (matches[2].StartsWith("verified", True))
					{

						break;
					}

					return -1;
				}
			}

			return result;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool SendMessageAck(const String& destination, const String& id)
		{
			auto message = Message::Create(
				String(GetStation()),
				"APRS",
				DigiPath(GetPath()),
				String(destination),
				"ack",
				String(id)
			);

			return Send(message);
		}

		// @throw AL::Exception
		// @return 0 on connection closed
		// @return -1 if would block
		int Connection_Read(String& value)
		{
			switch (lpConnection->Read(value))
			{
				case 0:
					Connection_Disconnect(ClientDisconnectReasons::ConnectionLost);
					return 0;

				case -1:
					return -1;
			}

			if (IsAuthenticated())
			{
				OnReceive.Execute(
					value
				);
			}

			return 1;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Connection_Write(const String& value)
		{
			if (!lpConnection->Write(value))
			{
				Connection_Disconnect(
					ClientDisconnectReasons::ConnectionLost
				);

				return False;
			}

			if (IsAuthenticated())
			{
				OnSend.Execute(
					value
				);
			}

			return True;
		}

		Void Connection_Disconnect(ClientDisconnectReasons reason)
		{
			lpConnection->Disconnect();
			delete lpConnection;

			connectionType = ClientConnectionTypes::None;

			isConnected     = False;
			isAuthenticated = False;

			OnDisconnect.Execute(
				reason
			);
		}
	};
}
