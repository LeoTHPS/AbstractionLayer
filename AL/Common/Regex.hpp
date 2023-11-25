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

		// @throw AL::Exception
		static Bool IsMatch(const String& pattern, const String& buffer)
		{
			if (!_Regex(pattern).IsMatch(buffer))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		static Bool Match(MatchCollection& match, const String& pattern, const String& buffer)
		{
			if (!_Regex(pattern).Match(match, buffer))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		template<size_t S>
		static Bool Match(MatchCollection& match, const Char(&pattern)[S], const String& buffer)
		{
			if (!_Regex(pattern).Match(match, buffer))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		_Regex(const String& pattern)
			: regex(
				[&pattern]()
				{
					try
					{
						return Regex(
							pattern.GetCString(),
							pattern.GetLength()
						);
					}
					catch (const ::std::regex_error& error)
					{

						throw Exception(
							error.what()
						);
					}
				}()
			)
		{
		}

		// @throw AL::Exception
		template<size_t S>
		_Regex(const Char(&pattern)[S])
			: regex(
				[pattern]()
				{
					try
					{
						return Regex(
							&pattern[0],
							S - 1
						);
					}
					catch (const ::std::regex_error& error)
					{

						throw Exception(
							error.what()
						);
					}
				}()
			)
		{
		}

		virtual ~_Regex()
		{
		}

		// @throw AL::Exception
		Bool IsMatch(const String& buffer) const
		{
			MatchResults matches;

			try
			{
				if (!::std::regex_match(buffer.GetCString(), matches, regex))
				{

					return False;
				}
			}
			catch (const ::std::regex_error& error)
			{

				throw Exception(
					error.what()
				);
			}

			return True;
		}

		// @throw AL::Exception
		Bool Match(MatchCollection& match, const String& buffer) const
		{
			MatchResults matches;

			try
			{
				if (!::std::regex_search(buffer.GetCString(), matches, regex))
				{

					return False;
				}
			}
			catch (const ::std::regex_error& error)
			{

				throw Exception(
					error.what()
				);
			}

			match.SetCapacity(
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
