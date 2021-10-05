#pragma once
#include "AL/Common.hpp"

#include <regex>

namespace AL
{
	template<typename T>
	class _Regex
	{
		typedef ::std::basic_regex<typename T::Char> Regex;

		typedef ::std::match_results<const typename T::Char*> MatchResults;

		Regex regex;

	public:
		typedef typename T::Char           Char;
		typedef T                          String;
		typedef Collections::Array<String> MatchCollection;

		static Bool IsMatch(const String& pattern, const String& buffer)
		{
			if (!_Regex(pattern).IsMatch(buffer))
			{

				return False;
			}

			return True;
		}

		static Bool Match(MatchCollection& match, const String& pattern, const String& buffer)
		{
			if (!_Regex(pattern).Match(match, buffer))
			{

				return False;
			}

			return True;
		}
		template<size_t S>
		static Bool Match(MatchCollection& match, const Char(&pattern)[S], const String& buffer)
		{
			if (!_Regex(pattern).Match(match, buffer))
			{

				return False;
			}

			return True;
		}

		_Regex(const String& pattern)
			: regex(
				pattern.GetCString(),
				pattern.GetLength()
			)
		{
		}

		template<size_t S>
		_Regex(const Char(&pattern)[S])
			: regex(
				&pattern[0],
				S - 1
			)
		{
		}

		virtual ~_Regex()
		{
		}

		Bool IsMatch(const String& buffer) const
		{
			MatchResults matches;

			if (!::std::regex_match(buffer.GetCString(), matches, regex))
			{

				return False;
			}

			return True;
		}

		Bool Match(MatchCollection& match, const String& buffer) const
		{
			MatchResults matches;

			if (::std::regex_search(buffer.GetCString(), matches, regex))
			{
				match.SetSize(
					matches.size()
				);

				size_t i = 0;

				for (auto it = matches.begin(); it != matches.end(); ++it, ++i)
				{
					match[i] = String(
						it->str().c_str(),
						it->str().length()
					);
				}

				return True;
			}

			return False;
		}

		auto& operator = (const String& pattern)
		{
			regex = Regex(
				pattern.GetCString(),
				pattern.GetLength()
			);

			return *this;
		}

		Bool operator == (const _Regex& regex) const
		{
			if (this->regex != regex.regex)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const _Regex& regex) const
		{
			if (operator==(regex))
			{

				return False;
			}

			return True;
		}
	};

	typedef _Regex<String>  Regex;
	typedef _Regex<WString> RegexW;
}
