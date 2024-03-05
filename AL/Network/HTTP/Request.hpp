#pragma once
#include "AL/Common.hpp"

#include "Response.hpp"

#if AL_HAS_INCLUDE(<openssl/ssl.h>)
	#include "AL/OpenSSL/SSL.hpp"

	#define AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED
#endif

#include "AL/Collections/Array.hpp"

#include "AL/Network/DNS.hpp"
#include "AL/Network/IPAddress.hpp"
#include "AL/Network/TcpSocket.hpp"
#include "AL/Network/SocketExtensions.hpp"

#include "AL/Serialization/HTTP/Request.hpp"

namespace AL::Network::HTTP
{
	typedef Serialization::Uri                  Uri;
	typedef Serialization::UriQuery             UriQuery;
	typedef Serialization::UriAuthority         UriAuthority;

	typedef Serialization::HTTP::Header         Header;
	typedef Serialization::HTTP::Versions       Versions;
	typedef Serialization::HTTP::StatusCodes    StatusCodes;
	typedef Serialization::HTTP::RequestMethods RequestMethods;

	class Request
	{
		class Socket
		{
			Bool         isConnected  = False;
			Bool         isSslEnabled;

#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
			OpenSSL::SSL ssl;
#endif
			TcpSocket    socket;

			Socket(Socket&&) = delete;
			Socket(const Socket&) = delete;

		public:
			Socket(AddressFamilies addressFamily, Bool enableSSL)
				: isSslEnabled(
					enableSSL
				),
#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
				ssl(
					OpenSSL::Modes::Client,
					OpenSSL::Protocols::TLS
				),
#endif
				socket(
					addressFamily
				)
			{
			}

			virtual ~Socket()
			{
				if (IsConnected())
				{

					Disconnect();
				}
			}

			Bool IsConnected() const
			{
				return isConnected;
			}

			// @throw AL::Exception
			Bool Connect(const IPEndPoint& ep)
			{
				AL_ASSERT(
					!IsConnected(),
					"Socket already connected"
				);

				try
				{
					socket.Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening TcpSocket"
					);
				}

#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
				if (isSslEnabled)
				{
					try
					{
						ssl.Create();
					}
					catch (Exception& exception)
					{
						socket.Close();

						throw Exception(
							Move(exception),
							"Error creating OpenSSL::SSL"
						);
					}

					try
					{
						ssl.SetFD(
							socket.GetHandle()
						);
					}
					catch (Exception& exception)
					{
						ssl.Destroy();
						socket.Close();

						throw Exception(
							Move(exception),
							"Error setting OpenSSL::SSL file descriptor"
						);
					}
				}
#endif

				try
				{
					if (!socket.Connect(ep))
					{

						throw Exception(
							"Connection timed out"
						);
					}
				}
				catch (Exception& exception)
				{
#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
					ssl.Destroy();
#endif
					socket.Close();

					throw Exception(
						Move(exception),
						"Error connecting to %s:%u",
						ep.Host.ToString().GetCString(),
						ep.Port
					);
				}

#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
				if (isSslEnabled)
				{
					try
					{
						ssl.Connect();
					}
					catch (Exception& exception)
					{
						socket.Close();

						throw Exception(
							Move(exception),
							"Error connecting OpenSSL::SSL"
						);
					}
				}
#endif

				isConnected = True;

				return True;
			}

			Void Disconnect()
			{
				if (IsConnected())
				{
#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
					ssl.Destroy();
#endif
					socket.Close();

					isConnected = False;
				}
			}

			// @throw AL::Exception
			Bool Read(Void* lpBuffer, size_t size, size_t& numberOfBytesRead)
			{
				AL_ASSERT(
					IsConnected(),
					"Socket not connected"
				);

#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
				if (isSslEnabled)
				{
					try
					{
						if (!ssl.Read(lpBuffer, size, numberOfBytesRead))
						{
							Disconnect();

							return False;
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error reading OpenSSL::SSL"
						);
					}
				}
				else
#endif
				{
					try
					{
						if (!socket.Receive(lpBuffer, size, numberOfBytesRead))
						{
							Disconnect();

							return False;
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error reading TcpSocket"
						);
					}
				}

				return True;
			}

			// @throw AL::Exception
			Bool Write(const Void* lpBuffer, size_t size)
			{
				AL_ASSERT(
					IsConnected(),
					"Socket not connected"
				);

				size_t numberOfBytesSent;

#if defined(AL_NETWORK_HTTP_REQUEST_OPENSSL_ENABLED)
				if (isSslEnabled)
				{
					numberOfBytesSent = 0;

					try
					{
						for (size_t _numberOfBytesSent = 0; numberOfBytesSent < size; numberOfBytesSent += _numberOfBytesSent)
						{
							if (!ssl.Write(&reinterpret_cast<const uint8*>(lpBuffer)[numberOfBytesSent], size - numberOfBytesSent, _numberOfBytesSent))
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
							"Error writing OpenSSL::SSL"
						);
					}
				}
				else
#endif
				{
					try
					{
						if (!SocketExtensions::SendAll(socket, lpBuffer, size, numberOfBytesSent))
						{
							Disconnect();

							return False;
						}
					}
					catch (Exception& exception)
					{

						throw Exception(
							Move(exception),
							"Error writing TcpSocket"
						);
					}
				}

				return True;
			}
		};

		typedef Collections::Array<typename String::Char> ResponseBuffer;

		Header         header;
		RequestMethods method;
		Versions       version;

		Request(Request&&) = delete;
		Request(const Request&) = delete;

	public:
		Request(Versions version, RequestMethods method)
			: method(
				method
			),
			version(
				version
			)
		{
		}

		virtual ~Request()
		{
		}

		auto& GetHeader()
		{
			return header;
		}
		auto& GetHeader() const
		{
			return header;
		}

		auto GetMethod() const
		{
			return method;
		}

		auto GetVersion() const
		{
			return version;
		}

		// @throw AL::Exception
		Response Execute(const Uri& uri)
		{
			String request;
			{
				static constexpr const char CRLF[] = "\r\n";

				StringBuilder sb;
				Execute_AppendMethod(sb, GetMethod());
				sb << " ";
				Execute_AppendPathAndQuery(sb, uri);
				sb << " ";
				Execute_AppendVersion(sb, GetVersion());
				sb << CRLF;
				Execute_AppendHeader(sb, GetVersion(), GetHeader(), uri, CRLF);
				sb << CRLF;

				request = sb.ToString();
			}

			// TODO: add message body
			// TODO: add support for more than GET/POST/HEAD
			// TODO: optionally reuse socket if HTTP/1.1
			// TODO: do a better job at detecting ports

			IPEndPoint serverEP =
			{
				.Port = uri.GetAuthority().Port
			};

			Bool       enableSSL = False;

			if (serverEP.Port == 0)
			{
				if (uri.GetScheme().Compare("ftp", True))
				{
					serverEP.Port = 21;
				}
				else if (uri.GetScheme().Compare("http", True))
				{
					serverEP.Port = 80;
				}
				else if (uri.GetScheme().Compare("https", True))
				{
					enableSSL    = True;
					serverEP.Port = 443;
				}
				else
				{
					throw NotImplementedException();
				}
			}

			Bool dns_IsInitialized;

			if ((dns_IsInitialized = DNS::IsInitialized()) == False)
			{
				try
				{
					DNS::Init();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error initializing DNS"
					);
				}
			}

			if (!DNS::Resolve(serverEP.Host, uri.GetAuthority().Host))
			{
				if (!dns_IsInitialized)
				{

					DNS::Deinit();
				}

				throw Exception(
					"Error resolving '%s'",
					uri.GetAuthority().Host.GetCString()
				);
			}

			if (!dns_IsInitialized)
			{

				DNS::Deinit();
			}

			Socket socket(
				serverEP.Host.GetFamily(),
				enableSSL
			);

			try
			{
				if (!socket.Connect(serverEP))
				{

					throw Exception(
						"Connection timed out"
					);
				}
			}
			catch (Exception& exception)
			{
				socket.Disconnect();

				throw Exception(
					Move(exception),
					"Error connecting to %s:%u",
					uri.GetAuthority().Host.GetCString(),
					serverEP.Port
				);
			}

			try
			{
				if (!socket.Write(request.GetCString(), request.GetSize()))
				{

					throw Exception(
						"Connection closed"
					);
				}
			}
			catch (Exception& exception)
			{
				socket.Disconnect();

				throw Exception(
					Move(exception),
					"Error writing Socket"
				);
			}

			Response response;
			{
				ResponseBuffer buffer(
					0xFFF
				);

				size_t bufferSize = 0;

				{
					ResponseBuffer bufferChunk(
						buffer.GetCapacity()
					);

					size_t numberOfBytesReceived;

					try
					{
						while (socket.Read(&bufferChunk[0], bufferChunk.GetCapacity() * sizeof(ResponseBuffer::Type), numberOfBytesReceived))
						{
							if (numberOfBytesReceived == 0)
							{

								continue;
							}

							if (auto numberOfBytesRemaining = (buffer.GetCapacity() - bufferSize);
								numberOfBytesRemaining < static_cast<size_t>(numberOfBytesReceived))
							{
								buffer.SetSize(
									buffer.GetCapacity() + (numberOfBytesReceived - numberOfBytesRemaining)
								);
							}

							memcpy(
								&buffer[bufferSize],
								&bufferChunk[0],
								static_cast<size_t>(numberOfBytesReceived)
							);

							bufferSize += numberOfBytesReceived;
						}
					}
					catch (Exception& exception)
					{
						socket.Disconnect();

						throw Exception(
							Move(exception),
							"Error reading Socket"
						);
					}
				}

				socket.Disconnect();

				{
					Header      header;
					StatusCodes status;
					String      content;
					Versions    version;

					{
						String chunk;
						String fieldName;
						String fieldValue;
						size_t bufferOffset = 0;

						// version + status
						{
							if (!Execute_ReadNextResponseChunk(chunk, buffer, bufferSize, bufferOffset))
							{

								throw Exception(
									"Unexpected end of response"
								);
							}

							Execute_ReadVersionAndStatusFromChunk(
								version,
								status,
								chunk
							);
						}

						// header
						do
						{
							if (!Execute_ReadNextResponseChunk(chunk, buffer, bufferSize, bufferOffset))
							{

								throw Exception(
									"Unexpected end of response"
								);
							}

							if (chunk.GetLength() != 0)
							{
								Execute_ReadHeaderFromChunk(
									fieldName,
									fieldValue,
									chunk
								);

								header.Add(
									Move(fieldName),
									Move(fieldValue)
								);
							}
						} while (chunk.GetLength() != 0);

						// content
						Execute_ReadNextResponseChunk(
							content,
							buffer,
							bufferSize,
							bufferOffset,
							True
						);
					}

					response = Response(
						version,
						Move(header),
						status,
						Move(content)
					);
				}
			}

			return response;
		}

	private:
		static Void Execute_AppendHeader(StringBuilder& sb, Versions version, const Header& header, const Uri& uri, const char* crlf)
		{
			Bool isHostSet = False;

			for (auto& pair : header)
			{
				if (pair.Key.Compare("Host", True))
				{
					sb << pair.Key << ": " << pair.Value << crlf;

					isHostSet = True;

					break;
				}
			}

			if (!isHostSet && (version == Versions::HTTP_1_1))
			{
				sb << "Host: " << uri.GetAuthority().Host;

				if (uri.GetAuthority().Port != 0)
				{

					sb << ':' << uri.GetAuthority().Port;
				}

				sb << crlf;
			}

			for (auto& pair : header)
			{
				if (pair.Key.Compare("Host", True))
				{

					continue;
				}

				sb << pair.Key << ": " << pair.Value << crlf;
			}
		}

		// @throw AL::Exception
		static Void Execute_AppendMethod(StringBuilder& sb, RequestMethods method)
		{
			sb << Serialization::HTTP::RequestMethods_ToString(method);
		}

		// @throw AL::Exception
		static Void Execute_AppendVersion(StringBuilder& sb, Versions version)
		{
			sb << Serialization::HTTP::Versions_ToString(version);
		}

		static Void Execute_AppendPathAndQuery(StringBuilder& sb, const Uri& uri)
		{
			sb << '/';

			if (uri.GetPath().GetLength() != 0)
			{

				sb << uri.GetPath();
			}

			if (uri.GetQuery().GetSize() != 0)
			{
				sb << '?';

				size_t i = 0;

				for (auto& pair : uri.GetQuery())
				{
					if (i++ != 0)
						sb << '&';

					sb << pair.Key << '=' << Uri::Encode(pair.Value);
				}
			}
		}

		// @throw AL::Exception
		static Bool Execute_ReadNextResponseChunk(String& chunk, const ResponseBuffer& buffer, size_t size, size_t& offset, Bool readToEnd = False)
		{
			if (offset >= size)
			{

				return False;
			}

			size_t numberOfBytesParsed = 0;
			size_t numberOfCharsToCopy = 0;

			for (; (offset + numberOfBytesParsed) < size; ++numberOfBytesParsed, ++numberOfCharsToCopy)
			{
				if (!readToEnd && (buffer[offset + numberOfBytesParsed] == '\r'))
				{
					if ((offset + numberOfBytesParsed) == size)
					{
						++numberOfBytesParsed;

						break;
					}

					numberOfBytesParsed += 2;

					break;
				}
			}

			chunk.Assign(
				&buffer[offset],
				numberOfCharsToCopy
			);

			offset += numberOfBytesParsed;

			return True;
		}

		// @throw AL::Exception
		static Void Execute_ReadHeaderFromChunk(String& name, String& value, const String& chunk)
		{
			auto semicolonIndex = chunk.IndexOf(
				':'
			);

			if (semicolonIndex == String::NPOS)
			{
				name = chunk;
				value.Clear();
			}
			else
			{
				name = chunk.SubString(
					0,
					semicolonIndex
				);

				value = chunk.SubString(
					semicolonIndex + 2
				);
			}
		}

		// @throw AL::Exception
		static Void Execute_ReadVersionAndStatusFromChunk(Versions& version, StatusCodes& status, const String& chunk)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^HTTP\\/(\\d\\.\\d) (\\d+) ([a-zA-Z ]+)$", chunk))
			{

				throw Exception(
					"Invalid format"
				);
			}

			// version
			{
				auto& versionString = matches[1];

				if (versionString.Compare("1.0"))
				{

					version = Versions::HTTP_1_0;
				}
				else if (versionString.Compare("1.1"))
				{

					version = Versions::HTTP_1_1;
				}
				else
				{

					throw Exception(
						"HTTP version not supported"
					);
				}
			}

			// status
			{
				auto& statusString = matches[2];

				status = FromString<StatusCodes>(
					statusString
				);
			}
		}
	};
}
