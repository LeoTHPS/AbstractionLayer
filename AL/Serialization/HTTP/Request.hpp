#pragma once
#include "AL/Common.hpp"

#include "Query.hpp"
#include "Header.hpp"
#include "Versions.hpp"
#include "StatusCodes.hpp"
#include "Environment.hpp"
#include "RequestMethods.hpp"

#include "AL/Serialization/Uri.hpp"

namespace AL::Serialization::HTTP
{
	class Request
	{
		Uri            uri;
		Query          query;
		Header         header;
		RequestMethods method;
		Versions       version;
		Environment    environment;

	public:
		// @throw AL::Exception
		static Request FromString(const String& string)
		{
			return FromString(
				string.ToWString()
			);
		}
		// @throw AL::Exception
		static Request FromString(const WString& wstring);

		Request()
		{
		}

		virtual ~Request()
		{
		}

		auto& GetUri() const
		{
			return uri;
		}

		auto& GetQuery() const
		{
			return query;
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

		auto& GetEnvironment() const
		{
			return environment;
		}

		// @throw AL::Exception
		auto& GetQuery(const String& name) const
		{
			auto it = GetQuery().Find(
				name
			);

			if (it == GetQuery().end())
			{

				throw Exception(
					"Query '%s' not found",
					name.GetCString()
				);
			}

			return it->Value;
		}

		// @throw AL::Exception
		auto& GetHeader(const String& name) const
		{
			auto it = GetHeader().Find(
				name
			);

			if (it == GetHeader().end())
			{

				throw Exception(
					"Header '%s' not found",
					name.GetCString()
				);
			}

			return it->Value;
		}

		// @throw AL::Exception
		auto& GetEnvironment(const String& name) const
		{
			auto it = GetEnvironment().Find(
				name
			);

			if (it == GetEnvironment().end())
			{

				throw Exception(
					"Environment '%s' not found",
					name.GetCString()
				);
			}

			return it->Value;
		}

		auto& GetEnvironmentOrDefault(const String& name, const String& value) const
		{
			auto it = GetEnvironment().Find(
				name
			);

			if (it == GetEnvironment().end())
			{

				return value;
			}

			return it->Value;
		}

		Void SetUri(Uri&& value)
		{
			uri = Move(value);
		}
		Void SetUri(const Uri& value)
		{
			SetUri(
				Uri(uri)
			);
		}

		Void SetQuery(Query&& value)
		{
			query = Move(value);
		}
		Void SetQuery(const Query& value)
		{
			SetQuery(
				Query(value)
			);
		}
		Void SetQuery(String&& name, String&& value)
		{
			query[Move(name)] = Move(value);
		}
		Void SetQuery(const String& name, String&& value)
		{
			SetQuery(String(name), Move(value));
		}
		Void SetQuery(const String& name, const String& value)
		{
			SetQuery(String(name), String(value));
		}

		Void SetHeader(Header&& value)
		{
			header = Move(value);
		}
		Void SetHeader(const Header& value)
		{
			SetHeader(
				Header(value)
			);
		}
		Void SetHeader(String&& name, String&& value)
		{
			header[Move(name)] = Move(value);
		}
		Void SetHeader(const String& name, String&& value)
		{
			SetHeader(String(name), Move(value));
		}
		Void SetHeader(const String& name, const String& value)
		{
			SetHeader(String(name), String(value));
		}

		Void SetMethod(RequestMethods value)
		{
			method = value;
		}

		Void SetVersion(Versions value)
		{
			version = value;
		}

		Void SetEnvironment(Environment&& value)
		{
			environment = Move(value);
		}
		Void SetEnvironment(const Environment& value)
		{
			SetEnvironment(
				Environment(value)
			);
		}
		Void SetEnvironment(String&& name, String&& value)
		{
			environment[Move(name)] = Move(value);
		}
		Void SetEnvironment(const String& name, String&& value)
		{
			SetEnvironment(String(name), Move(value));
		}
		Void SetEnvironment(const String& name, const String& value)
		{
			SetEnvironment(String(name), String(value));
		}

		Void RemoveHeader(const String& name)
		{
			header.Remove(name);
		}

		Void RemoveEnvironment(const String& name)
		{
			environment.Remove(name);
		}

		String  ToString() const
		{
			return ToWString().ToString();
		}
		WString ToWString() const;
	};
}
