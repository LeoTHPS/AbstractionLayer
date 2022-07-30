#pragma once
#include "AL/Common.hpp"

#include "Uri.hpp"
#include "Header.hpp"
#include "Response.hpp"
#include "Versions.hpp"
#include "StatusCodes.hpp"
#include "RequestMethods.hpp"

#include "AL/Network/DNS.hpp"
#include "AL/Network/IPAddress.hpp"
#include "AL/Network/TcpSocket.hpp"
#include "AL/Network/SocketExtensions.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Network::HTTP
{
	class Request
	{
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
				Execute_AppendPath(sb, uri);
				sb << " ";
				Execute_AppendVersion(sb, GetVersion());
				sb << CRLF;
				Execute_AppendHeader(sb, GetVersion(), GetHeader(), uri, CRLF);
				sb << CRLF;

				request = sb.ToString();
			}

			// TODO: add message body
			// TODO: add support for more than GET/POST/HEAD
			// TODO: reuse socket if HTTP/1.1

			IPEndPoint serverEP =
			{
				.Port = uri.GetAuthority().Port
			};

			if (serverEP.Port == 0)
			{

				serverEP.Port = 80;
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

			TcpSocket socket(
				serverEP.Host.GetFamily()
			);

			try
			{
				socket.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening Socket"
				);
			}

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
				socket.Close();

				throw Exception(
					Move(exception),
					"Error connecting to %s:%u",
					uri.GetAuthority().Host.GetCString(),
					serverEP.Port
				);
			}

			size_t numberOfBytesSent;

			try
			{
				if (!SocketExtensions::SendAll(socket, request.GetCString(), request.GetSize(), numberOfBytesSent))
				{

					throw Exception(
						"Connection closed"
					);
				}
			}
			catch (Exception& exception)
			{
				socket.Close();

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
						while (socket.Receive(&bufferChunk[0], bufferChunk.GetCapacity() * sizeof(ResponseBuffer::Type), numberOfBytesReceived))
						{
							if (numberOfBytesReceived == 0)
							{

								continue;
							}

							auto numberOfBytesRemaining = buffer.GetCapacity() - bufferSize;

							if (numberOfBytesRemaining < static_cast<size_t>(numberOfBytesReceived))
							{
								buffer.SetSize(
									numberOfBytesReceived - numberOfBytesRemaining
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
						socket.Close();

						throw Exception(
							Move(exception),
							"Error reading Socket"
						);
					}
				}

				socket.Close();

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
		static Void Execute_AppendPath(StringBuilder& sb, const Uri& uri)
		{
			if (uri.GetPath().GetLength() == 0)
			{
				sb << '/';
			}
			else
			{
				sb << uri.GetPath();
			}
		}

		static Void Execute_AppendHeader(StringBuilder& sb, Versions version, const Header& header, const Uri& uri, const char* crlf)
		{
			Bool isHostSet = False;

			for (auto& pair : header)
			{
				sb << pair.Key << ": " << pair.Value;
				sb << crlf;

				if (pair.Key.Compare("Host", True))
				{

					isHostSet = True;
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
		}

		// @throw AL::Exception
		static Void Execute_AppendMethod(StringBuilder& sb, RequestMethods method)
		{
			switch (method)
			{
				case RequestMethods::GET:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_GET;
					break;

				case RequestMethods::HEAD:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_HEAD;
					break;

				case RequestMethods::POST:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_POST;
					break;

				case RequestMethods::PUT:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_PUT;
					break;

				case RequestMethods::DELETE:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_DELETE;
					break;

				case RequestMethods::CONNECT:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_CONNECT;
					break;

				case RequestMethods::OPTIONS:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_OPTIONS;
					break;

				case RequestMethods::TRACE:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_TRACE;
					break;

				case RequestMethods::PATCH:
					sb << AL_NETWORK_HTTP_REQUEST_METHOD_PATCH;
					break;

				default:
					throw NotImplementedException();
			}
		}

		// @throw AL::Exception
		static Void Execute_AppendVersion(StringBuilder& sb, Versions version)
		{
			switch (version)
			{
				case Versions::HTTP_1_0:
					sb << AL_NETWORK_HTTP_VERSION_1_0;
					break;

				case Versions::HTTP_1_1:
					sb << AL_NETWORK_HTTP_VERSION_1_1;
					break;

				default:
					throw NotImplementedException();
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

			if (!Regex::Match(matches, "^HTTP\\/(\\d\\.\\d) (\\d+) (\\w+)$", chunk))
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
