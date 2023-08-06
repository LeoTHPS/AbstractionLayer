#pragma once
#include "AL/Common.hpp"

#include "Request.hpp"
#include "Response.hpp"

namespace AL::Network::HTTP
{
	class Client
	{
		Header   header;
		Versions version = Versions::HTTP_1_1;

		Client(Client&&) = delete;
		Client(const Client&) = delete;

	public:
		Client()
		{
		}

		virtual ~Client()
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

		auto GetVersion() const
		{
			return version;
		}

		// @throw AL::Exception
		String DownloadString(const Uri& uri)
		{
			Request request(
				GetVersion(),
				RequestMethods::GET
			);

			if (GetHeader().GetSize() != 0)
			{

				request.GetHeader() = GetHeader();
			}

			Response response;

			try
			{
				response = request.Execute(
					uri
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error executing Request"
				);
			}

			if (response.GetStatus() != StatusCodes::OK)
			{

				throw Exception(
					"Response returned status code %u",
					response.GetStatus()
				);
			}

			return response.GetContent();
		}
	};
}
