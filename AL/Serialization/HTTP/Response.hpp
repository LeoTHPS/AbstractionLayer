#pragma once
#include "AL/Common.hpp"

#include "Query.hpp"
#include "Header.hpp"
#include "Versions.hpp"
#include "StatusCodes.hpp"
#include "ContentTypes.hpp"

namespace AL::Serialization::HTTP
{
	enum class ResponseTypes : uint8
	{
		CGI,
		HTTP
	};

	class Response
	{
		String content;
		String contentType = CONTENT_TYPE_HTML;

	public:
		// @throw AL::Exception
		static Response FromString(const String& string)
		{
			return FromString(
				string.ToWString()
			);
		}
		// @throw AL::Exception
		static Response FromString(const WString& wstring);

		Response()
		{
		}

		virtual ~Response()
		{
		}

		auto& GetContent() const
		{
			return content;
		}

		auto& GetContentType() const
		{
			return contentType;
		}

		Void SetContent(String&& value)
		{
			content = Move(value);
		}
		Void SetContent(const String& value)
		{
			SetContent(String(value));
		}

		Void SetContentType(String&& value)
		{
			contentType = Move(value);
		}
		Void SetContentType(const String& value)
		{
			SetContentType(String(value));
		}

		String  ToString(ResponseTypes type) const
		{
			return ToWString(type).ToString();
		}
		WString ToWString(ResponseTypes type) const
		{
			WStringBuilder sb;

			if (type == ResponseTypes::HTTP)
			{
				// TODO: add missing fields
			}

			sb << L"Content-Type: " << GetContentType().ToWString() << L"\n\n";
			sb << GetContent().ToWString();

			return sb.ToString();
		}
	};
}
