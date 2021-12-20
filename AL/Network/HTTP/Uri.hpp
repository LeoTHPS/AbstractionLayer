#pragma once
#include "AL/Common.hpp"

namespace AL::Network::HTTP
{
	struct UriAuthority
	{
		String Host;
		uint16 Port;
		String Username;
		String Password;
	};

	class Uri
	{
		String       path;
		String       query;
		String       scheme;
		String       fragment;
		UriAuthority authority;

	public:
		// @throw AL::Exception
		static Uri FromString(const String& string)
		{
			Regex::MatchCollection matches;

			if (!Regex::Match(matches, "^([^:]+):(\\/\\/([^\\/]+))(\\/\\S*?)$", string))
			{

				throw Exception(
					"Invalid format"
				);
			}

			Uri uri;
			uri.scheme = matches[1];

			auto path      = matches[4];
			auto authority = matches[3];

			if (Regex::Match(matches, "^\\/([^?]*?)\\?([^#]*?)#(\\S*?)$", path))
			{
				uri.path     = matches[1];
				uri.query    = matches[2];
				uri.fragment = matches[3];
			}
			else if (Regex::Match(matches, "^\\/([^#]*?)#(\\S*?)$", path))
			{
				uri.path = matches[1];
				uri.query.Clear();
				uri.fragment = matches[2];
			}
			else if (Regex::Match(matches, "^\\/(\\S*?)$", path))
			{
				uri.path = matches[1];
				uri.query.Clear();
				uri.fragment.Clear();
			}
			else
			{

				throw Exception(
					"Invalid path format"
				);
			}

			if (Regex::Match(matches, "^([^@]+?)@([^:]+?):(\\d+?)$", authority))
			{
				uri.authority.Username = matches[1];
				uri.authority.Password.Clear();
				uri.authority.Host     = matches[2];
				uri.authority.Port     = AL::FromString<uint16>(
					matches[3]
				);
			}
			else if (Regex::Match(matches, "^([^@]+?)@(\\S+?)$", authority))
			{
				uri.authority.Username = matches[1];
				uri.authority.Password.Clear();
				uri.authority.Host     = matches[2];
				uri.authority.Port     = 0;
			}
			else if (Regex::Match(matches, "^(\\S+?)$", authority))
			{
				uri.authority.Username.Clear();
				uri.authority.Password.Clear();
				uri.authority.Host = matches[1];
				uri.authority.Port = 0;
			}
			else
			{

				throw Exception(
					"Invalid authority format"
				);
			}

			return uri;
		}

		Uri()
		{
		}

		Uri(Uri&& uri)
			: path(
				Move(uri.path)
			),
			query(
				Move(uri.query)
			),
			scheme(
				Move(uri.scheme)
			),
			fragment(
				Move(uri.fragment)
			),
			authority(
				Move(uri.authority)
			)
		{
		}
		Uri(const Uri& uri)
			: path(
				uri.path
			),
			query(
				uri.query
			),
			scheme(
				uri.scheme
			),
			fragment(
				uri.fragment
			),
			authority(
				uri.authority
			)
		{
		}

		auto& GetPath() const
		{
			return path;
		}

		auto& GetQuery() const
		{
			return query;
		}

		auto& GetScheme() const
		{
			return scheme;
		}

		auto& GetFragment() const
		{
			return fragment;
		}

		auto& GetAuthority() const
		{
			return authority;
		}

		virtual ~Uri()
		{
		}

		String ToString() const
		{
			StringBuilder sb;

			ToString_AppendScheme(
				sb,
				GetScheme()
			);

			ToString_AppendAuthority(
				sb,
				GetAuthority()
			);

			ToString_AppendPathQueryFragment(
				sb,
				GetPath(),
				GetQuery(),
				GetFragment()
			);

			return sb.ToString();
		}

		Uri& operator = (Uri&& uri)
		{
			path = Move(
				uri.path
			);

			query = Move(
				uri.query
			);

			scheme = Move(
				uri.scheme
			);

			fragment = Move(
				uri.fragment
			);

			{
				authority.Host = Move(
					uri.authority.Host
				);

				authority.Port =
					uri.authority.Port;

				authority.Username = Move(
					uri.authority.Username
				);

				authority.Password = Move(
					uri.authority.Password
				);
			}

			return *this;
		}
		Uri& operator = (const Uri& uri)
		{
			path     = uri.path;
			query    = uri.query;
			scheme   = uri.scheme;
			fragment = uri.fragment;

			{
				authority.Host     = uri.authority.Host;
				authority.Port     = uri.authority.Port;
				authority.Username = uri.authority.Username;
				authority.Password = uri.authority.Password;
			}

			return *this;
		}

		Bool operator == (const Uri& uri) const
		{
			if (GetPath() != uri.GetPath())
			{

				return False;
			}

			if (GetQuery() != uri.GetQuery())
			{

				return False;
			}

			if (GetScheme() != uri.GetScheme())
			{

				return False;
			}

			if (GetFragment() != uri.GetFragment())
			{

				return False;
			}

			{
				auto& authority = GetAuthority();
				auto& uri_authority = uri.GetAuthority();

				if (authority.Host != uri_authority.Host)
				{

					return False;
				}

				if (authority.Port != uri_authority.Port)
				{

					return False;
				}

				if (authority.Username != uri_authority.Username)
				{

					return False;
				}

				if (authority.Password != uri_authority.Password)
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const Uri& uri) const
		{
			if (operator==(uri))
			{

				return False;
			}

			return True;
		}

	private:
		static Void ToString_AppendScheme(StringBuilder& sb, const String& scheme)
		{
			sb << scheme << ':';
		}
		static Void ToString_AppendAuthority(StringBuilder& sb, const UriAuthority& authority)
		{
			sb << "//";

			if (authority.Username.GetLength() != 0)
			{
				sb << authority.Username << '@' << authority.Host;

				if (authority.Port != 0)
				{

					sb << ':' << authority.Port;
				}
			}
			else if (authority.Port != 0)
			{

				sb << authority.Host << ':' << authority.Port;
			}
			else
			{

				sb << authority.Host;
			}
		}
		static Void ToString_AppendPathQueryFragment(StringBuilder& sb, const String& path, const String& query, const String& fragment)
		{
			sb << '/' << path;

			if (query.GetLength() != 0)
			{

				sb << '?' << query;
			}

			if (fragment.GetLength() != 0)
			{

				sb << '#' << fragment;
			}
		}
	};
}
