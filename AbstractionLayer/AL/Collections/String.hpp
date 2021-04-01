#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"

#include "AL/Algorithms/FNV.hpp"

namespace AL::Collections
{
	template<typename CHAR>
	struct __String_Types;

	template<typename CHAR>
	struct __String_Utility;

	template<typename CHAR>
	struct __String_Constants;
	template<>
	struct __String_Constants<char>
	{
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
	};
	template<>
	struct __String_Constants<wchar_t>
	{
		static constexpr wchar_t END = L'\000';

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
	};

	template<typename CHAR>
	class _String
	{
		typedef __String_Types<CHAR>     Types;
		typedef __String_Utility<CHAR>   Utility;
		typedef __String_Constants<CHAR> Constants;

		size_t length;
		typename Types::Container container;

		explicit _String(size_t capacity)
			: length(
				0
			),
			container(
				capacity + 1
			)
		{
		}

	public:
		typedef typename Types::Char                     Char;

		typedef typename Types::Container                Container;

		typedef typename Container::Iterator             Iterator;
		typedef typename Container::ConstIterator        ConstIterator;

		typedef typename Container::ReverseIterator      ReverseIterator;
		typedef typename Container::ConstReverseIterator ConstReverseIterator;

		static constexpr Char                            END = Constants::END;

		static auto GetLength(const Char* lpString)
		{
			size_t length = 0;

			for (; lpString[length] != Constants::END; ++length)
			{
			}

			return length;
		}
		static auto GetLength(const _String& string)
		{
			return string.GetLength();
		}

		template<typename ... CHUNK_TYPES>
		static auto Combine(CHUNK_TYPES ... chunks)
		{
			return Utility::Combine(
				Forward<CHUNK_TYPES>(chunks) ...
			);
		}
		
		template<typename DELIMITER, typename ... CHUNK_TYPES>
		static auto Join(DELIMITER delimiter, CHUNK_TYPES ... chunks)
		{
			return Utility::Join(
				Forward<DELIMITER>(delimiter),
				Forward<CHUNK_TYPES>(chunks) ...
			);
		}

		template<typename ... TArgs>
		static auto Format(const Char* lpFormat, TArgs ... args);
		template<typename ... TArgs>
		static auto Format(const _String& format, TArgs ... args);

		_String()
			: length(
				0
			),
			container(
				{ Constants::END }
			)
		{
		}

		_String(_String&& string)
			: length(
				string.length
			),
			container(
				Move(string.container)
			)
		{
		}

		_String(const _String& string)
			: length(
				string.length
			),
			container(
				string.container
			)
		{
		}

		explicit _String(Container&& container)
			: length(
				GetLength(
					&container[0]
				)
			),
			container(
				Move(container)
			)
		{
			this->container.SetSize(
				length + 1
			);
		}
		
		explicit _String(const Container& container)
			: _String(
				Container(container)
			)
		{
		}

		_String(const Char* lpString)
			: _String(
				lpString,
				GetLength(
					lpString
				)
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
			: length(
				length
			),
			container(
				length + 1
			)
		{
			memcpy(
				&container[0],
				lpString,
				length * sizeof(Char)
			);

			container.SetSize(
				length + 1
			);

			container[length] = END;
		}

		_String(Char c, size_t count)
			: length(
				count
			),
			container(
				count + 1
			)
		{
			container.Fill(
				c
			);

			container[count] = END;
		}

		virtual ~_String()
		{
		}

		size_t GetSize() const
		{
			return GetLength() + 1;
		}

		size_t GetLength() const
		{
			return length;
		}

		size_t GetCapacity() const
		{
			return container.GetCapacity() - 1;
		}

		auto GetCString() const
		{
			return &container[0];
		}

		_String ToLower() const
		{
			_String string = *this;

			for (auto& c : string)
			{
				c = Utility::ToLower(
					c
				);
			}

			return string;
		}

		_String ToUpper() const
		{
			_String string = *this;

			for (auto& c : string)
			{
				c = Utility::ToUpper(
					c
				);
			}

			return string;
		}

		void Swap(_String& string)
		{
			AL::Swap(
				length,
				string.length
			);

			container.Swap(
				string.container
			);
		}

		void Clear()
		{
			Erase(
				begin(),
				end()
			);
		}

		void Erase(size_t index)
		{
			Erase(
				index,
				1
			);
		}
		void Erase(size_t index, size_t count)
		{
			length -= count;

			container.Erase(
				index,
				count
			);
		}

		void Erase(Iterator it)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstIterator it)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}

		void Erase(Iterator first, Iterator last)
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstIterator first, ConstIterator last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ReverseIterator first, ReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstReverseIterator first, ConstReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}

		void ShrinkToFit()
		{
			container.SetCapacity(
				GetLength() + 1
			);

			container[GetLength()] = END;
		}

		void SetCapacity(size_t value)
		{
			if (value > GetCapacity())
			{
				container.SetCapacity(
					value + 1
				);
			}
			else if (value < GetCapacity())
			{
				container.SetCapacity(
					value + 1
				);

				if (value > GetLength())
				{
					length = value;

					container[value] = END;
				}
			}
		}

		auto SubString(size_t index) const
		{
			return SubString(
				index,
				GetLength() - index
			);
		}
		auto SubString(size_t index, size_t length) const
		{
			_String string(
				length
			);

			memcpy(
				&string[0],
				&GetCString()[index],
				length * sizeof(Char)
			);

			string.length = length;

			string[length] = END;

			return string;
		}

		auto SubString(Iterator it) const
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			return SubString(
				index
			);
		}
		auto SubString(ConstIterator it) const
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			return SubString(
				index
			);
		}
		auto SubString(ReverseIterator it) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			return SubString(
				index
			);
		}
		auto SubString(ConstReverseIterator it) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			return SubString(
				index
			);
		}
		
		auto SubString(Iterator first, Iterator last) const
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			return SubString(
				index,
				length
			);
		}
		auto SubString(ConstIterator first, ConstIterator last) const
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			return SubString(
				index,
				length
			);
		}
		auto SubString(ReverseIterator first, ReverseIterator last) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			return SubString(
				index,
				length
			);
		}
		auto SubString(ConstReverseIterator first, ConstReverseIterator last) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			return SubString(
				index,
				length
			);
		}

		void Append(Char value)
		{
			Insert(
				end(),
				value
			);
		}
		void Append(const Char* lpValue)
		{
			Insert(
				end(),
				lpValue
			);
		}
		void Append(const _String& value)
		{
			Insert(
				end(),
				value
			);
		}

		void Insert(size_t index, Char value)
		{
			Insert(
				index,
				&value,
				1
			);
		}
		void Insert(size_t index, const Char* lpValue)
		{
			Insert(
				index,
				lpValue,
				GetLength(lpValue)
			);
		}
		void Insert(size_t index, const _String& value)
		{
			Insert(
				index,
				value.GetCString(),
				value.GetLength()
			);
		}
		void Insert(Iterator it, Char value)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(Iterator it, const Char* lpValue)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Insert(
				index,
				lpValue
			);
		}
		void Insert(Iterator it, const _String& value)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ConstIterator it, Char value)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ConstIterator it, const Char* lpValue)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Insert(
				index,
				lpValue
			);
		}
		void Insert(ConstIterator it, const _String& value)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ReverseIterator it, Char value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ReverseIterator it, const Char* lpValue)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				lpValue
			);
		}
		void Insert(ReverseIterator it, const _String& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ConstReverseIterator it, Char value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}
		void Insert(ConstReverseIterator it, const Char* lpValue)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				lpValue
			);
		}
		void Insert(ConstReverseIterator it, const _String& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				value
			);
		}

		auto Split(Char delimiter, bool ignoreCase = false) const
		{
			Char buffer[] =
			{
				delimiter,
				END
			};

			return Split(
				buffer,
				1,
				ignoreCase
			);
		}
		auto Split(const Char* lpDelimiter, bool ignoreCase = false) const
		{
			return Split(
				lpDelimiter,
				GetLength(lpDelimiter),
				ignoreCase
			);
		}
		auto Split(const _String& delimiter, bool ignoreCase = false) const
		{
			return Split(
				delimiter.GetCString(),
				delimiter.GetLength(),
				ignoreCase
			);
		}

		void Remove(Char value)
		{
			size_t index;

			if (Find(index, value))
			{
				Erase(
					index
				);
			}
		}
		void Remove(const Char* lpValue)
		{
			size_t index;

			if (Find(index, lpValue))
			{
				Erase(
					index,
					GetLength(lpValue)
				);
			}
		}
		void Remove(const _String& value)
		{
			size_t index;

			if (Find(index, value))
			{
				Erase(
					index,
					value.GetLength()
				);
			}
		}
		
		void RemoveAll(Char value)
		{
			size_t index;

			while (Find(index, value))
			{
				Erase(
					index
				);
			}
		}
		void RemoveAll(const Char* lpValue)
		{
			size_t index;

			auto valueLength = GetLength(
				lpValue
			);

			while (Find(index, lpValue))
			{
				Erase(
					index,
					valueLength
				);
			}
		}
		void RemoveAll(const _String& value)
		{
			size_t index;

			while (Find(index, value))
			{
				Erase(
					index,
					value.GetLength()
				);
			}
		}
		
		void RemoveLast(Char value)
		{
			size_t index;

			if (FindLast(index, value))
			{
				Erase(
					index
				);
			}
		}
		void RemoveLast(const Char* lpValue)
		{
			size_t index;

			if (FindLast(index, lpValue))
			{
				Erase(
					index,
					GetLength(lpValue)
				);
			}
		}
		void RemoveLast(const _String& value)
		{
			size_t index;

			if (FindLast(index, value))
			{
				Erase(
					index,
					value.GetLength()
				);
			}
		}

		bool Compare(const Char* lpValue, bool ignoreCase = false) const
		{
			for (size_t i = 0; i < GetLength(); ++i)
			{
				if (ignoreCase)
				{
					if (Utility::ToLower(container[i]) != Utility::ToLower(lpValue[i]))
					{

						return false;
					}
				}
				else if (container[i] != lpValue[i])
				{

					return false;
				}
			}

			if (lpValue[GetLength()] != END)
			{

				return false;
			}

			return true;
		}
		bool Compare(const _String& value, bool ignoreCase = false) const
		{
			if (GetLength() != value.GetLength())
			{

				return false;
			}

			if (!ignoreCase && !memcmp(value.GetCString(), GetCString(), GetLength() * sizeof(Char)))
			{

				return false;
			}
			else if (ignoreCase)
			{
				for (size_t i = 0; i < GetLength(); ++i)
				{
					if (Utility::ToLower(container[i]) != Utility::ToLower(value[i]))
					{

						return false;
					}
				}
			}

			return true;
		}

		bool Contains(Char value, bool ignoreCase = false) const
		{
			for (size_t i = 0; i < GetLength(); ++i)
			{
				if (container[i] == value)
				{

					return true;
				}
				else if (ignoreCase && (Utility::ToLower(container[i]) == Utility::ToLower(value)))
				{

					return true;
				}
			}

			return false;
		}
		bool Contains(const Char* lpValue, bool ignoreCase = false) const
		{
			return Contains(
				lpValue,
				GetLength(lpValue),
				ignoreCase
			);
		}
		bool Contains(const _String& value, bool ignoreCase = false) const
		{
			return Contains(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}

		bool EndsWith(Char value, bool ignoreCase = false) const
		{
			if (container[GetLength() - 1] == value)
			{

				return true;
			}
			else if (ignoreCase && (Utility::ToLower(container[GetLength() - 1]) == Utility::ToLower(value)))
			{

				return true;
			}

			return false;
		}
		bool EndsWith(const Char* lpValue, bool ignoreCase = false) const
		{
			return EndsWith(
				lpValue,
				GetLength(lpValue),
				ignoreCase
			);
		}
		bool EndsWith(const _String& value, bool ignoreCase = false) const
		{
			return EndsWith(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		
		bool StartsWith(Char value, bool ignoreCase = false) const
		{
			if (container[0] == value)
			{

				return true;
			}
			else if (ignoreCase && (Utility::ToLower(container) == Utility::ToLower(value)))
			{

				return true;
			}

			return false;
		}
		bool StartsWith(const Char* lpValue, bool ignoreCase = false) const
		{
			return StartsWith(
				lpValue,
				GetLength(lpValue),
				ignoreCase
			);
		}
		bool StartsWith(const _String& value, bool ignoreCase = false) const
		{
			return StartsWith(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}

		bool Find(size_t& index, Char value, bool ignoreCase = false) const
		{
			Char buffer[] =
			{
				value,
				END
			};

			return Find(
				index,
				buffer,
				ignoreCase
			);
		}
		bool Find(size_t& index, const Char* lpValue, bool ignoreCase = false) const
		{
			return Find(
				index,
				lpValue,
				GetLength(lpValue),
				ignoreCase
			);
		}
		bool Find(size_t& index, const _String& value, bool ignoreCase = false) const
		{
			return Find(
				index,
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		bool Find(Iterator& iterator, Char value, bool ignoreCase = false)
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool Find(Iterator& iterator, const Char* lpValue, bool ignoreCase = false)
		{
			size_t index;

			if (!Find(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool Find(Iterator& iterator, const _String& value, bool ignoreCase = false)
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool Find(ConstIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool Find(ConstIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool Find(ConstIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool Find(ReverseIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool Find(ReverseIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool Find(ReverseIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool Find(ConstReverseIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool Find(ConstReverseIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool Find(ConstReverseIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!Find(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}

		bool FindLast(size_t& index, Char value, bool ignoreCase = false) const
		{
			Char buffer[] =
			{
				value,
				END
			};

			return FindLast(
				index,
				buffer,
				ignoreCase
			);
		}
		bool FindLast(size_t& index, const Char* lpValue, bool ignoreCase = false) const
		{
			return FindLast(
				index,
				lpValue,
				GetLength(lpValue),
				ignoreCase
			);
		}
		bool FindLast(size_t& index, const _String& value, bool ignoreCase = false) const
		{
			return FindLast(
				index,
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		bool FindLast(Iterator& iterator, Char value, bool ignoreCase = false)
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool FindLast(Iterator& iterator, const Char* lpValue, bool ignoreCase = false)
		{
			size_t index;

			if (!FindLast(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool FindLast(Iterator& iterator, const _String& value, bool ignoreCase = false)
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = begin() += index;

			return true;
		}
		bool FindLast(ConstIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool FindLast(ConstIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool FindLast(ConstIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = cbegin() += index;

			return true;
		}
		bool FindLast(ReverseIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool FindLast(ReverseIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool FindLast(ReverseIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool FindLast(ConstReverseIterator& iterator, Char value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool FindLast(ConstReverseIterator& iterator, const Char* lpValue, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, lpValue, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}
		bool FindLast(ConstReverseIterator& iterator, const _String& value, bool ignoreCase = false) const
		{
			size_t index;

			if (!FindLast(index, value, ignoreCase))
			{

				return false;
			}

			iterator = rbegin() += index;

			return true;
		}

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
			return container.begin() += GetLength();
		}
		ConstIterator end() const
		{
			return container.begin() += GetLength();
		}

		ConstIterator cbegin() const
		{
			return container.cbegin();
		}

		ConstIterator cend() const
		{
			return container.cbegin() += GetLength();
		}

		ReverseIterator rbegin()
		{
			return container.rend() += GetLength();
		}
		ConstReverseIterator rbegin() const
		{
			return container.rend() += GetLength();
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

		auto& operator = (_String&& string)
		{
			length = string.length;
			string.length = 0;

			container = Move(
				string.container
			);

			return *this;
		}
		auto& operator = (const _String& string)
		{
			length = string.length;
			container = string.container;

			return *this;
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

		bool operator == (const Char* lpString) const
		{
			return Compare(
				lpString
			);
		}
		bool operator != (const Char* lpString) const
		{
			return !operator==(
				lpString
			);
		}

		bool operator == (const _String& string) const
		{
			return Compare(
				string
			);
		}
		bool operator != (const _String& string) const
		{
			return !operator==(
				string
			);
		}

	private:
		auto Split(const Char* lpDelimiter, size_t delimiterLength, bool ignoreCase) const
		{
			Array<_String> chunks;

			for (size_t i = 0, j; i < GetLength(); i = (j + 1))
			{
				if (!FindAt(j, i, lpDelimiter, delimiterLength, ignoreCase))
				{

					j = GetLength();
				}

				auto chunk = SubString(
					i,
					j - i
				);

				chunks.PushBack(
					Move(chunk)
				);
			}

			return chunks;
		}

		void Insert(size_t index, const Char* lpValue, size_t length)
		{
			Container container(
				(GetLength() + length) + 1
			);

			if (index == 0)
			{
				memcpy(
					&container[0],
					lpValue,
					length * sizeof(Char)
				);

				memcpy(
					&container[length],
					GetCString(),
					GetLength() * sizeof(Char)
				);
			}
			else if (index == GetLength())
			{
				memcpy(
					&container[0],
					GetCString(),
					GetLength() * sizeof(Char)
				);

				memcpy(
					&container[GetLength()],
					lpValue,
					length * sizeof(Char)
				);
			}
			else
			{
				memcpy(
					&container[0],
					GetCString(),
					index * sizeof(Char)
				);

				memcpy(
					&container[index],
					lpValue,
					length * sizeof(Char)
				);

				memcpy(
					&container[index + length],
					&GetCString()[index],
					GetLength() - index * sizeof(Char)
				);
			}

			container.SetSize(
				(GetLength() + length) + 1
			);

			container[GetLength() + length] = END;

			this->length += length;

			this->container = Move(
				container
			);
		}

		bool Contains(const Char* lpValue, size_t length, bool ignoreCase) const
		{
			size_t index;

			return Find(
				index,
				lpValue,
				length,
				ignoreCase
			);
		}

		bool EndsWith(const Char* lpValue, size_t length, bool ignoreCase) const
		{
			if (GetLength() >= length)
			{
				auto compare_value_to_container = [](const Char* _lpContainer, const Char* _lpValue, size_t _length, bool _ignoreCase)
				{
					if (!_ignoreCase && !memcmp(_lpContainer, _lpValue, _length * sizeof(Char)))
					{

						return false;
					}
					else if (_ignoreCase)
					{
						for (size_t i = 0; i < _length; ++i)
						{
							if (Utility::ToLower(_lpContainer[i]) != Utility::ToLower(_lpValue[i]))
							{

								return false;
							}
						}
					}

					return true;
				};

				return compare_value_to_container(
					&container[GetLength() - length],
					lpValue,
					length,
					ignoreCase
				);
			}

			return false;
		}

		bool StartsWith(const Char* lpValue, size_t length, bool ignoreCase) const
		{
			if (GetLength() >= length)
			{
				if (!ignoreCase && memcmp(&container[0], lpValue, length * sizeof(Char)))
				{

					return true;
				}
				else if (ignoreCase)
				{
					for (size_t i = 0; i < length; ++i)
					{
						if (Utility::ToLower(container[i]) != Utility::ToLower(lpValue[i]))
						{

							return false;
						}
					}

					return true;
				}
			}

			return false;
		}

		bool Find(size_t& index, const Char* lpValue, size_t length, bool ignoreCase) const
		{
			if (!FindAt(index, 0, lpValue, length, ignoreCase))
			{

				return false;
			}

			return true;
		}

		bool FindAt(size_t& index, size_t offset, const Char* lpValue, size_t length, bool ignoreCase) const
		{
			auto compare_value_to_container = [](const Char* _lpContainer, const Char* _lpValue, size_t _length, bool _ignoreCase)
			{
				if (!_ignoreCase && !memcmp(_lpContainer, _lpValue, _length * sizeof(Char)))
				{

					return false;
				}
				else if (_ignoreCase)
				{
					for (size_t i = 0; i < _length; ++i)
					{
						if (Utility::ToLower(_lpContainer[i]) != Utility::ToLower(_lpValue[i]))
						{

							return false;
						}
					}
				}

				return true;
			};

			for (size_t i = offset; i < GetLength(); ++i)
			{
				if (container[i] != lpValue[0])
				{

					continue;
				}

				if (!compare_value_to_container(&container[i + 1], &lpValue[1], length - 1, ignoreCase))
				{

					continue;
				}

				index = i;

				return true;
			}

			return false;
		}

		bool FindLast(size_t& index, const Char* lpValue, size_t length, bool ignoreCase) const
		{
			auto compare_value_to_container = [](const Char* _lpContainer, const Char* _lpValue, size_t _length, bool _ignoreCase)
			{
				if (!_ignoreCase && !memcmp(_lpContainer, _lpValue, _length * sizeof(Char)))
				{

					return false;
				}
				else if (_ignoreCase)
				{
					for (size_t i = 0; i < _length; ++i)
					{
						if (Utility::ToLower(_lpContainer[i]) != Utility::ToLower(_lpValue[i]))
						{

							return false;
						}
					}
				}

				return true;
			};

			for (size_t i = (GetLength() - length), j = length; j <= GetLength(); --i, ++j)
			{
				if (container[i] != lpValue[0])
				{

					continue;
				}

				if (!compare_value_to_container(&container[i + 1], &lpValue[1], length - 1, ignoreCase))
				{

					continue;
				}

				index = i;

				return true;
			}

			return false;
		}
	};

	template<>
	struct __String_Types<char>
	{
		typedef char        Char;
		typedef Array<Char> Container;
	};
	template<>
	struct __String_Types<wchar_t>
	{
		typedef wchar_t     Char;
		typedef Array<Char> Container;
	};

	template<>
	struct __String_Utility<char>
	{
		template<typename ... CHUNK_TYPES>
		static _String<char> Combine(CHUNK_TYPES ... chunks)
		{
			_String<char> buffer;

			Concat<CHUNK_TYPES ...>(
				buffer,
				Forward<CHUNK_TYPES>(chunks) ...
			);

			return buffer;
		}

		template<typename DELIMITER, typename ... CHUNK_TYPES>
		static _String<char> Join(DELIMITER delimiter, CHUNK_TYPES ... chunks)
		{
			_String<char> buffer;

			Concat2<DELIMITER, CHUNK_TYPES ...>(
				buffer,
				Forward<DELIMITER>(delimiter),
				Forward<CHUNK_TYPES>(chunks) ...
			);

			return buffer;
		}

		static constexpr bool IsLower(char c)
		{
			return ((c >= 'a') && (c <= 'z')) && (__String_Constants<char>::LOWERCASE_TABLE[c - 'a'] == c);
		}

		static constexpr bool IsUpper(char c)
		{
			return ((c >= 'A') && (c <= 'Z')) && (__String_Constants<char>::UPPERCASE_TABLE[c - 'A'] == c);
		}

		static constexpr char ToLower(char c)
		{
			return IsUpper(c) ? __String_Constants<char>::LOWERCASE_TABLE[c - 'A'] : c;
		}

		static constexpr char ToUpper(char c)
		{
			return IsLower(c) ? __String_Constants<char>::UPPERCASE_TABLE[c - 'a'] : c;
		}

	private:
		static void Concat(_String<char>& buffer)
		{
		}
		template<typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static void Concat(_String<char>& buffer, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
		{
			buffer.Append(
				chunk
			);

			Concat<CHUNK_TYPES ...>(
				buffer,
				Forward<CHUNK_TYPES>(chunks) ...
			);
		}

		template<typename DELIMITER>
		static void Concat2(_String<char>& buffer, DELIMITER delimiter)
		{
		}
		template<typename DELIMITER, typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static void Concat2(_String<char>& buffer, DELIMITER delimiter, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
		{
			buffer.Append(
				chunk
			);

			if constexpr (sizeof ...(CHUNK_TYPES) != 0)
			{
				buffer.Append(
					delimiter
				);

				Concat2<DELIMITER, CHUNK_TYPES ...>(
					buffer,
					Forward<DELIMITER>(delimiter),
					Forward<CHUNK_TYPES>(chunks) ...
				);
			}
		}
	};
	template<>
	struct __String_Utility<wchar_t>
	{
		template<typename ... CHUNK_TYPES>
		static _String<wchar_t> Combine(CHUNK_TYPES ... chunks)
		{
			_String<wchar_t> buffer;

			Concat<CHUNK_TYPES ...>(
				buffer,
				Forward<CHUNK_TYPES>(chunks) ...
			);

			return buffer;
		}

		template<typename DELIMITER, typename ... CHUNK_TYPES>
		static _String<wchar_t> Join(DELIMITER delimiter, CHUNK_TYPES ... chunks)
		{
			_String<wchar_t> buffer;

			Concat2<DELIMITER, CHUNK_TYPES ...>(
				buffer,
				Forward<DELIMITER>(delimiter),
				Forward<CHUNK_TYPES>(chunks) ...
			);

			return buffer;
		}
		
		static constexpr bool IsLower(wchar_t c)
		{
			return ((c >= L'a') && (c <= L'z')) && (__String_Constants<wchar_t>::LOWERCASE_TABLE[c - L'a'] == c);
		}

		static constexpr bool IsUpper(wchar_t c)
		{
			return ((c >= L'A') && (c <= L'Z')) && (__String_Constants<wchar_t>::UPPERCASE_TABLE[c - L'A'] == c);
		}

		static constexpr wchar_t ToLower(wchar_t c)
		{
			return IsUpper(c) ? __String_Constants<wchar_t>::LOWERCASE_TABLE[c - L'A'] : c;
		}

		static constexpr wchar_t ToUpper(wchar_t c)
		{
			return IsLower(c) ? __String_Constants<wchar_t>::UPPERCASE_TABLE[c - L'a'] : c;
		}

	private:
		static void Concat(_String<wchar_t>& buffer)
		{
		}
		template<typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static void Concat(_String<wchar_t>& buffer, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
		{
			buffer.Append(
				chunk
			);

			Concat<CHUNK_TYPES ...>(
				buffer,
				Forward<CHUNK_TYPES>(chunks) ...
			);
		}

		template<typename DELIMITER>
		static void Concat2(_String<wchar_t>& buffer, DELIMITER delimiter)
		{
		}
		template<typename DELIMITER, typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static void Concat2(_String<wchar_t>& buffer, DELIMITER delimiter, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
		{
			buffer.Append(
				chunk
			);

			if constexpr (sizeof ...(CHUNK_TYPES) != 0)
			{
				buffer.Append(
					delimiter
				);

				Concat2<DELIMITER, CHUNK_TYPES ...>(
					buffer,
					Forward<DELIMITER>(delimiter),
					Forward<CHUNK_TYPES>(chunks) ...
				);
			}
		}
	};

	typedef _String<char>    String;
	typedef _String<wchar_t> WString;
}

template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<char>::Format(const Char* lpFormat, TArgs ... args)
{
	auto bufferSize = std::snprintf(
		nullptr,
		0,
		lpFormat,
		std::forward<TArgs>(args) ...
	);

	_String string(
		bufferSize
	);

	string.length = std::snprintf(
		&string[0],
		bufferSize + 1,
		lpFormat,
		std::forward<TArgs>(args) ...
	);

	return string;
}
template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<wchar_t>::Format(const Char* lpFormat, TArgs ... args)
{
	auto bufferSize = std::swprintf(
		nullptr,
		0,
		lpFormat,
		std::forward<TArgs>(args) ...
	);

	_String string(
		bufferSize
	);

	string.length = std::swprintf(
		&string[0],
		bufferSize + 1,
		lpFormat,
		std::forward<TArgs>(args) ...
	);

	return string;
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
		bufferSize
	);

	string.length = std::snprintf(
		&string[0],
		bufferSize + 1,
		format.GetCString(),
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
		bufferSize
	);

	string.length = std::swprintf(
		&string[0],
		bufferSize + 1,
		format.GetCString(),
		std::forward<TArgs>(args) ...
	);

	return string;
}

template<typename CHAR>
struct std::hash<AL::Collections::_String<CHAR>>
{
	size_t operator () (const AL::Collections::_String<CHAR>& string) const noexcept
	{
		static constexpr bool IS_UINT32 = sizeof(size_t) == sizeof(AL::uint32);

		typedef typename AL::Conditional<IS_UINT32, AL::Algorithms::FNV32, AL::Algorithms::FNV64>::Type FNV;

		return FNV::Calculate(
			string.GetCString(),
			string.GetLength()
		);
	}
};
