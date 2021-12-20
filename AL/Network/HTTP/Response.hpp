#pragma once
#include "AL/Common.hpp"

#include "Header.hpp"
#include "Versions.hpp"
#include "StatusCodes.hpp"

namespace AL::Network::HTTP
{
	class Response
	{
		Header      header;
		StatusCodes status;
		String      content;
		Versions    version;

	public:
		Response()
		{
		}

		Response(Response&& response)
			: header(
				Move(response.header)
			),
			status(
				Move(response.status)
			),
			content(
				Move(response.content)
			),
			version(
				response.version
			)
		{
		}
		Response(const Response& response)
			: header(
				response.header
			),
			status(
				response.status
			),
			content(
				response.content
			),
			version(
				response.version
			)
		{
		}

		Response(Versions version, Header&& header, StatusCodes status, String&& content)
			: header(
				Move(header)
			),
			status(
				status
			),
			content(
				Move(content)
			),
			version(
				version
			)
		{
		}

		virtual ~Response()
		{
		}

		auto& GetHeader() const
		{
			return header;
		}

		auto GetStatus() const
		{
			return status;
		}

		auto& GetContent() const
		{
			return content;
		}

		auto GetVersion() const
		{
			return version;
		}

		Response& operator = (Response&& response)
		{
			header = Move(
				response.header
			);

			status = Move(
				response.status
			);

			content = Move(
				response.content
			);

			version = response.version;

			return *this;
		}
		Response& operator = (const Response& response)
		{
			header  = response.header;
			status  = response.status;
			content = response.content;
			version = response.version;

			return *this;
		}

		Bool operator == (const Response& response) const
		{
			if (GetStatus() != response.GetStatus())
			{

				return False;
			}

			if (GetHeader() != response.GetHeader())
			{

				return False;
			}

			if (GetContent() != response.GetContent())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Response& response) const
		{
			if (operator==(response))
			{

				return False;
			}

			return True;
		}
	};
}
