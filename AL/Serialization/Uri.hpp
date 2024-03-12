#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Dictionary.hpp"

namespace AL::Serialization
{
	typedef Collections::Dictionary<String, String> UriQuery;

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
		UriQuery     query;
		String       scheme;
		String       fragment;
		UriAuthority authority;

		Uri(String&& scheme, UriAuthority&& authority, String&& path, UriQuery&& query, String&& fragment)
			: path(
				Move(path)
			),
			query(
				Move(query)
			),
			scheme(
				Move(scheme)
			),
			fragment(
				Move(fragment)
			),
			authority(
				Move(authority)
			)
		{
		}

	public:
		static String Encode(const String& value)
		{
			StringBuilder sb;
			Encode(sb, value);

			return sb.ToString();
		}

		static String Decode(const String& value)
		{
			StringBuilder sb;

			for (size_t i = 0; i < value.GetLength(); )
				i += Decode(sb, value, i);

			return sb.ToString();
		}

		// @throw AL::Exception
		static Uri Create(String&& scheme, UriAuthority&& authority, String&& path)
		{
			return Create(Move(scheme), Move(authority), Move(path), UriQuery());
		}
		// @throw AL::Exception
		static Uri Create(const String& scheme, const UriAuthority& authority, const String& path)
		{
			return Create(String(scheme), UriAuthority(authority), String(path));
		}

		// @throw AL::Exception
		static Uri Create(String&& scheme, UriAuthority&& authority, String&& path, UriQuery&& query)
		{
			return Create(Move(scheme), Move(authority), Move(path), Move(query), "");
		}
		// @throw AL::Exception
		static Uri Create(const String& scheme, const UriAuthority& authority, const String& path, const UriQuery& query)
		{
			return Create(String(scheme), UriAuthority(authority), String(path), UriQuery(query));
		}

		// @throw AL::Exception
		static Uri Create(String&& scheme, UriAuthority&& authority, String&& path, UriQuery&& query, String&& fragment)
		{
			// TODO: validate

			return Uri(Move(scheme), Move(authority), Move(path), Move(query), Move(fragment));
		}
		// @throw AL::Exception
		static Uri Create(const String& scheme, const UriAuthority& authority, const String& path, const String& fragment)
		{
			return Create(String(scheme), UriAuthority(authority), String(path), String(fragment));
		}

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
			uri.path   = Move(matches[4]);
			uri.scheme = Move(matches[1]);

			auto authority = matches[3];

			if (Regex::Match(matches, "^\\/([^?#]*)\\?([^#]*)#(\\S+)$", uri.path))
			{
				uri.path     = Move(matches[1]);
				uri.query    = FromString_Query(matches[2]);
				uri.fragment = Move(matches[3]);
			}
			else if (Regex::Match(matches, "^\\/([^?#]*)\\?(\\S+)$", uri.path))
			{
				uri.path  = Move(matches[1]);
				uri.query = FromString_Query(matches[2]);
			}
			else if (Regex::Match(matches, "^\\/([^?#]*)#(\\S+)$", uri.path))
			{
				uri.path     = Move(matches[1]);
				uri.fragment = Move(matches[2]);
			}
			else if (!Regex::Match(matches, "^\\/([^?#]*)$", uri.path))
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
		// @throw AL::Exception
		static Uri FromString(const WString& wstring)
		{
			return FromString(wstring.ToString());
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

		virtual ~Uri()
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
		template<typename T>
		T     GetQuery(const AL::String& name) const
		{
			auto& value = GetQuery(
				name
			);

			return AL::FromString<T>(
				value
			);
		}

		auto& GetQueryOrDefault(const String& name, const String& value) const
		{
			auto it = GetQuery().Find(
				name
			);

			if (it == GetQuery().end())
			{

				return value;
			}

			return it->Value;
		}
		template<typename T>
		T     GetQueryOrDefault(const AL::String& name, T&& value) const
		{
			auto it = GetQuery().Find(
				name
			);

			if (it == GetQuery().end())
			{

				return value;
			}

			return AL::FromString<T>(
				it->Value
			);
		}

		// @throw AL::Exception
		Void SetQuery(String&& key, String&& value)
		{
			for (auto& pair : query)
			{
				if (pair.Key.Compare(key, True))
				{
					pair.Value = Move(value);

					return;
				}
			}

			query.Add(Move(key), Move(value));
		}
		// @throw AL::Exception
		Void SetQuery(const String& key, String&& value)
		{
			SetQuery(String(key), Move(value));
		}
		// @throw AL::Exception
		Void SetQuery(const String& key, const String& value)
		{
			SetQuery(String(key), String(value));
		}

		String  ToString() const
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
		WString ToWString() const
		{
			return ToString().ToWString();
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
		static Void Encode(StringBuilder& sb, String::Char value)
		{
			if ((value == '*') || (value == '-') || (value == '.') || (value == '_'))
				sb << value;
			if (Math::IsInRange(value, '\x30', '\x39') || Math::IsInRange(value, '\x41', '\x5A') || Math::IsInRange(value, '\x61', '\x7A'))
				sb << value;
			else
			{
				sb << '%';
				sb << String::Format("%02X", value);
			}
		}
		static Void Encode(StringBuilder& sb, const String& value)
		{
			for (auto c : value)
			{
				Encode(
					sb,
					c
				);
			}
		}

		static size_t Decode(StringBuilder& sb, const String& value, size_t index)
		{
			if (value[index] == '%')
			{
				if ((index + 3) < value.GetLength())
					sb << HexConverter::Decode<String::Char>(value, index + 1, 2);

				return 3;
			}

			sb << value[index];

			return 1;
		}

		static UriQuery FromString_Query(const String& value)
		{
			UriQuery query;

			for (auto& chunk : value.Split('&'))
			{
				auto chunks = chunk.Split('=');

				query.Add(Decode(chunks[0]), (chunks.GetSize() > 1) ? Decode(chunks[1]) : "");
			}

			return query;
		}

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
		static Void ToString_AppendPathQueryFragment(StringBuilder& sb, const String& path, const UriQuery& query, const String& fragment)
		{
			sb << '/' << path;

			if (query.GetSize() != 0)
			{
				sb << '?';

				size_t i = 0;

				for (auto& pair : query)
				{
					if (i++ != 0)
					{

						sb << '&';
					}

					Encode(sb, pair.Key);
					sb << '=';
					Encode(sb, pair.Value);
				}
			}

			if (fragment.GetLength() != 0)
			{

				sb << '#' << fragment;
			}
		}
	};
}
