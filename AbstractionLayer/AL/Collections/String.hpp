#pragma once
#include "AL/Common.hpp"

#include "AL/Algorithms/FNV.hpp"

#include <string>

namespace AL::Collections
{
	template<typename CHAR>
	class _String;

	template<typename CHAR>
	struct __String_Constants;
	template<>
	struct __String_Constants<char>
	{
		typedef std::string Container;

		static constexpr char END = '\0';

		static constexpr char LOWERCASE_TABLE[] =
		{
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
			'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
			's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
		};

		static constexpr char UPPERCASE_TABLE[] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
			'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
			'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
		};

		static constexpr bool IsLower(char c)
		{
			return ((c >= 'a') && (c <= 'z')) && (LOWERCASE_TABLE[c - 'a'] == c);
		}

		static constexpr bool IsUpper(char c)
		{
			return ((c >= 'A') && (c <= 'Z')) && (UPPERCASE_TABLE[c - 'A'] == c);
		}

		static constexpr char ToLower(char c)
		{
			return IsUpper(c) ? LOWERCASE_TABLE[c - 'A'] : c;
		}

		static constexpr char ToUpper(char c)
		{
			return IsLower(c) ? UPPERCASE_TABLE[c - 'a'] : c;
		}
	};
	template<>
	struct __String_Constants<wchar_t>
	{
		typedef std::wstring Container;

		static constexpr wchar_t END = L'\0';

		static constexpr wchar_t LOWERCASE_TABLE[] =
		{
			L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i',
			L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r',
			L's', L't', L'u', L'v', L'w', L'x', L'y', L'z'
		};

		static constexpr wchar_t UPPERCASE_TABLE[] =
		{
			L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I',
			L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R',
			L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z'
		};

		static constexpr bool IsLower(wchar_t c)
		{
			return ((c >= L'a') && (c <= L'z')) && (LOWERCASE_TABLE[c - L'a'] == c);
		}

		static constexpr bool IsUpper(wchar_t c)
		{
			return ((c >= L'A') && (c <= L'Z')) && (UPPERCASE_TABLE[c - L'A'] == c);
		}

		static constexpr wchar_t ToLower(wchar_t c)
		{
			return IsUpper(c) ? LOWERCASE_TABLE[c - L'A'] : c;
		}

		static constexpr wchar_t ToUpper(wchar_t c)
		{
			return IsLower(c) ? UPPERCASE_TABLE[c - L'a'] : c;
		}
	};

	template<typename CHAR>
	class __String_Chunks_Helper
	{
	public:
		typedef _String<CHAR> String;

		template<typename ... CHUNK_TYPES>
		static String Combine(CHUNK_TYPES ... chunks)
		{
			String buffer;

			Concat(
				buffer,
				Forward<CHUNK_TYPES>(chunks) ...
			);

			return buffer;
		}

	private:
		static void Concat(String& buffer)
		{
		}
		template<typename ... CHUNK_TYPES>
		static void Concat(String& buffer, const String& chunk, CHUNK_TYPES ... chunks);
	};

	template<typename CHAR>
	class _String
	{
		typedef typename __String_Constants<CHAR>::Container Container;

		Container container;

		explicit _String(Container&& string)
			: container(
				Move(string)
			)
		{
		}

	public:
		typedef CHAR Char;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		static constexpr Char END = __String_Constants<Char>::END;

		static constexpr size_t NPOS = Container::npos;

		static size_t GetLength(const Char* string)
		{
			size_t length = 0;

			while (string[length++] != __String_Constants<Char>::END)
			{
			}

			return length;
		}
		static size_t GetLength(const _String& string)
		{
			return string.GetLength();
		}
		template<size_t SIZE>
		static size_t GetLength(const Char(&string)[SIZE])
		{
			return GetLength(
				&string[0]
			);
		}

		template<typename ... CHUNK_TYPES>
		static _String<Char> Combine(CHUNK_TYPES ... chunks)
		{
			return __String_Chunks_Helper<Char>::Combine(
				chunks ...
			);
		}

		_String()
		{
		}

		_String(const Char* lpString)
			: container(
				lpString
			)
		{
		}

		_String(Char c, size_t count)
			: container(
				count,
				c
			)
		{
		}

		template<size_t SIZE>
		_String(const Char(&string)[SIZE])
			: _String(
				&string[0],
				SIZE - 1
			)
		{
		}

		_String(const Char* lpString, size_t length)
			: container(
				lpString,
				length
			)
		{
		}

		template<typename ITERATOR>
		_String(ITERATOR first, ITERATOR last)
			: container(
				first,
				last
			)
		{
		}

		virtual ~_String()
		{
		}

		auto GetSize() const
		{
			return container.size();
		}

		auto GetLength() const
		{
			return container.length();
		}

		auto GetCapacity() const
		{
			return container.capacity();
		}

		auto& GetString() const
		{
			return container;
		}

		auto GetCString() const
		{
			return container.c_str();
		}

		_String ToUpper() const
		{
			_String string(
				*this
			);

			for (auto& c : string)
			{
				c = __String_Constants<Char>::ToUpper(
					c
				);
			}

			return string;
		}

		_String ToLower() const
		{
			_String string(
				*this
			);

			for (auto& c : string)
			{
				c = __String_Constants<Char>::ToLower(
					c
				);
			}

			return string;
		}

		Iterator Find(Char c)
		{
			auto i = IndexOf(
				c
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}
		Iterator Find(const _String& string)
		{
			auto i = IndexOf(
				string
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}
		template<size_t SIZE>
		Iterator Find(const Char(&string)[SIZE])
		{
			auto i = IndexOf(
				string
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}

		ConstIterator Find(Char c) const
		{
			auto i = IndexOf(
				c
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}
		ConstIterator Find(const _String& string) const
		{
			auto i = IndexOf(
				string
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}
		template<size_t SIZE>
		ConstIterator Find(const Char(&string)[SIZE]) const
		{
			auto i = IndexOf(
				string
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}
		
		Iterator FindLast(Char c)
		{
			auto i = IndexOfLast(
				c
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}
		Iterator FindLast(const _String& string)
		{
			auto i = IndexOfLast(
				string
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}
		template<size_t SIZE>
		Iterator FindLast(const Char(&string)[SIZE])
		{
			auto i = IndexOfLast(
				string
			);

			if (i == NPOS)
			{

				return end();
			}

			return begin() += i;
		}

		ConstIterator FindLast(Char c) const
		{
			auto i = IndexOfLast(
				c
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}
		ConstIterator FindLast(const _String& string) const
		{
			auto i = IndexOfLast(
				string
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}
		template<size_t SIZE>
		ConstIterator FindLast(const Char(&string)[SIZE]) const
		{
			auto i = IndexOfLast(
				string
			);

			if (i == NPOS)
			{

				return cend();
			}

			return cbegin() += i;
		}

		size_t IndexOf(Char c) const
		{
			for (size_t i = 0, length = GetLength(); i < length; ++i)
			{
				if (container[i] == c)
				{

					return i;
				}
			}

			return NPOS;
		}
		size_t IndexOf(const _String& string) const
		{
			return container.find_first_of(
				string.container
			);
		}
		template<size_t SIZE>
		size_t IndexOf(const Char(&string)[SIZE]) const
		{
			return container.find_first_of(
				&string[0]
			);
		}

		size_t IndexOfLast(Char c) const
		{
			if (auto length = GetLength())
			{
				for (size_t i = length; i > 0; --i)
				{
					if (container[i] == c)
					{

						return i;
					}
				}

				if (container[0] == c)
				{

					return 0;
				}
			}

			return NPOS;
		}
		size_t IndexOfLast(const _String& string) const
		{
			return container.find_last_of(
				string.container
			);
		}
		template<size_t SIZE>
		size_t IndexOfLast(const Char(&string)[SIZE]) const
		{
			return container.find_last_of(
				&string[0]
			);
		}
		
		bool Contains(const _String& string) const
		{
			return IndexOf(string) != NPOS;
		}
		template<size_t SIZE>
		bool Contains(const Char(&string)[SIZE]) const
		{
			return IndexOf(string) != NPOS;
		}

		bool Compare(const _String& string, bool ignoreCase = false) const
		{
			if (!ignoreCase)
			{
				for (size_t i = 0; i < string.GetSize(); i++)
				{
					if (container[i] != string[i])
					{

						return false;
					}
				}
			}
			else
			{
				for (size_t i = 0; i < string.GetSize(); i++)
				{
					if (__String_Constants<Char>::ToLower(container[i]) != __String_Constants<Char>::ToLower(string[i]))
					{

						return false;
					}
				}
			}

			return true;
		}
		template<size_t SIZE>
		bool Compare(const Char(&string)[SIZE], bool ignoreCase = false) const
		{
			if (!ignoreCase)
			{
				for (size_t i = 0; i < SIZE; i++)
				{
					if (container[i] != string[i])
					{

						return false;
					}
				}
			}
			else
			{
				for (size_t i = 0; i < SIZE; i++)
				{
					if (__String_Constants<Char>::ToLower(container[i]) != __String_Constants<Char>::ToLower(string[i]))
					{

						return false;
					}
				}
			}

			return true;
		}

		bool StartsWith(Char c) const
		{
			if (!GetLength())
			{

				return false;
			}

			if (container[0] != c)
			{

				return false;
			}

			return true;
		}
		bool StartsWith(const _String& string) const;
		template<size_t SIZE>
		bool StartsWith(const Char(&string)[SIZE]) const;
		
		bool EndsWith(Char c) const
		{
			size_t length;

			if ((length = GetLength()) == 0)
			{
				
				return false;
			}

			if (container[length - 1] != c)
			{

				return false;
			}

			return true;
		}
		bool EndsWith(const _String& string) const;
		template<size_t SIZE>
		bool EndsWith(const Char(&string)[SIZE]) const;

		void Clear()
		{
			container.clear();
		}

		void ShrinkToFit()
		{
			container.shrink_to_fit();
		}

		void Swap(_String& string)
		{
			container.swap(
				string.container
			);
		}

		void Reserve(size_t size)
		{
			container.reserve(
				GetSize() + size
			);
		}

		template<typename ITERATOR>
		void Erase(ITERATOR it)
		{
			container.erase(
				it
			);
		}
		template<typename ITERATOR>
		void Erase(ITERATOR first, ITERATOR last)
		{
			container.erase(
				first,
				last
			);
		}

		template<typename ITERATOR>
		auto Split(ITERATOR it) const
		{
			auto string = container.substr(
				it - container.begin()
			);

			return _String(
				Move(string)
			);
		}
		template<typename ITERATOR>
		auto Split(ITERATOR begin, ITERATOR end) const
		{
			size_t length = end - begin;
			size_t first = begin - container.begin();
			
			auto string = container.substr(
				first,
				length
			);

			return _String(
				Move(string)
			);
		}

		auto SubString(size_t i) const
		{
			return SubString(
				i,
				GetLength()
			);
		}
		auto SubString(size_t begin, size_t end) const
		{
			auto string = container.substr(
				begin,
				end - begin
			);

			return _String(
				Move(string)
			);
		}

		void Append(Char c)
		{
			Append(
				{ c, __String_Constants<Char>::END }
			);
		}
		void Append(const _String& string)
		{
			container.append(
				string.container
			);
		}
		template<size_t SIZE>
		void Append(const Char(&string)[SIZE])
		{
			container.append(
				&string[0]
			);
		}

		template<typename ITERATOR>
		void Insert(ITERATOR it, const _String& string)
		{
			container.insert(
				it,
				string.container
			);
		}
		template<typename ITERATOR, size_t SIZE>
		void Insert(ITERATOR it, const Char(&string)[SIZE])
		{
			container.insert(
				it,
				&string[0],
				SIZE - 1
			);
		}

		void Remove(const _String& string)
		{
			auto i = IndexOf(
				string
			);

			if (i != NPOS)
			{
				Erase(
					i
				);
			}
		}
		template<size_t SIZE>
		void Remove(const Char(&string)[SIZE])
		{
			auto i = IndexOf(
				string
			);

			if (i != NPOS)
			{
				Erase(
					i
				);
			}
		}

		void RemoveLast(const _String& string)
		{
			auto i = IndexOfLast(
				string
			);

			if (i != NPOS)
			{
				Erase(
					i
				);
			}
		}
		template<size_t SIZE>
		void RemoveLast(const Char(&string)[SIZE])
		{
			auto i = IndexOfLast(
				string
			);

			if (i != NPOS)
			{
				Erase(
					i
				);
			}
		}

		void SetSize(size_t size)
		{
			container.resize(
				size
			);
		}

		void SetCapacity(size_t capacity)
		{
			container.reserve(
				capacity
			);
		}

		template<typename ... TArgs>
		static auto Format(const _String& format, TArgs ... args);
		template<size_t SIZE, typename ... TArgs>
		static auto Format(const Char(&format)[SIZE], TArgs ... args);

		Iterator begin()
		{
			return container.begin();
		}
		ConstIterator begin() const
		{
			return container.begin();
		}

		Iterator end()
		{
			return container.end();
		}
		ConstIterator end() const
		{
			return container.end();
		}

		ConstIterator cbegin() const
		{
			return container.cbegin();
		}

		ConstIterator cend() const
		{
			return container.cend();
		}

		ReverseIterator rbegin()
		{
			return container.rbegin();
		}
		ConstReverseIterator rbegin() const
		{
			return container.crbegin();
		}

		ReverseIterator rend()
		{
			return container.rend();
		}
		ConstReverseIterator rend() const
		{
			return container.crend();
		}

		auto& operator [] (size_t index)
		{
			return container[index];
		}
		auto& operator [] (size_t index) const
		{
			return container[index];
		}

		bool operator < (const _String& string) const
		{
			auto fnv32_hash = [](const _String& _string)
			{
				uint32 hash = 0x811C9DC5;
				auto length = _string.GetLength();

				for (size_t i = 0; i < length; ++i)
				{
					hash ^= static_cast<uint32>(
						_string[i]
					);

					hash *= 0x1000193llu;
				}

				return hash;
			};

			return fnv32_hash(*this) < fnv32_hash(string);
		}

		bool operator == (const _String& string) const
		{
			return container == string.container;
		}
		bool operator != (const _String& string) const
		{
			return !operator==(string);
		}
	};

	typedef _String<char> String;
	typedef _String<wchar_t> WString;
}

template<typename CHAR>
template<typename ... CHUNK_TYPES>
inline void AL::Collections::__String_Chunks_Helper<CHAR>::Concat(String& buffer, const String& chunk, CHUNK_TYPES ... chunks)
{
	buffer.Append(
		chunk
	);
	
	Concat(
		buffer,
		Forward<CHUNK_TYPES>(chunks) ...
	);
}

template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<char>::Format(const _String& format, TArgs ... args)
{
	auto bufferSize = std::snprintf(
		nullptr,
		0,
		format.GetCString(),
		std::forward<TArgs>(args) ...
	);

	_String string(
		'\0',
		bufferSize
	);

	std::snprintf(
		&string[0],
		bufferSize + 1,
		format.GetCString(),
		std::forward<TArgs>(args) ...
	);

	return string;
}
template<>
template<size_t SIZE, typename ... TArgs>
inline auto AL::Collections::_String<char>::Format(const char(&format)[SIZE], TArgs ... args)
{
	auto bufferSize = std::snprintf(
		nullptr,
		0,
		&format[0],
		std::forward<TArgs>(args) ...
	);

	_String string(
		'\0',
		bufferSize
	);

	std::snprintf(
		&string[0],
		bufferSize + 1,
		&format[0],
		std::forward<TArgs>(args) ...
	);

	return string;
}

template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<wchar_t>::Format(const _String& format, TArgs ... args)
{
	auto bufferSize = std::swprintf(
		nullptr,
		0,
		format.GetCString(),
		std::forward<TArgs>(args) ...
	);

	_String string(
		L'\0',
		bufferSize
	);

	std::swprintf(
		&string[0],
		bufferSize + 1,
		format.GetCString(),
		std::forward<TArgs>(args) ...
	);

	return string;
}
template<>
template<size_t SIZE, typename ... TArgs>
inline auto AL::Collections::_String<wchar_t>::Format(const wchar_t(&format)[SIZE], TArgs ... args)
{
	auto bufferSize = std::swprintf(
		nullptr,
		0,
		&format[0],
		std::forward<TArgs>(args) ...
	);

	_String string(
		L'\0',
		bufferSize
	);

	std::swprintf(
		&string[0],
		bufferSize + 1,
		&format[0],
		std::forward<TArgs>(args) ...
	);

	return string;
}

template<typename C>
struct std::hash<AL::Collections::_String<C>>
{
	size_t operator ()(const AL::Collections::_String<C>& string) const noexcept
	{
		static constexpr bool IS_UINT32 = sizeof(size_t) == sizeof(AL::uint32);

		typedef typename AL::Conditional<IS_UINT32, AL::Algorithms::FNV32, AL::Algorithms::FNV64>::Type FNV;

		return FNV::Calculate(
			string.GetCString(),
			string.GetLength()
		);
	}
};
