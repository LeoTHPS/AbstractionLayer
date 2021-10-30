#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"
#include "ArrayList.hpp"

#include "AL/Algorithms/FNV.hpp"

#include <cstdio> // snprintf/swprintf

namespace AL::Collections
{
	template<typename T_CHAR>
	struct __String_Types;
	template<>
	struct __String_Types<char>
	{
		typedef char                           Char;
		typedef ArrayList<Char>                Container;
		typedef typename Container::Collection Collection;
	};
	template<>
	struct __String_Types<wchar_t>
	{
		typedef wchar_t                        Char;
		typedef ArrayList<Char>                Container;
		typedef typename Container::Collection Collection;
	};

	template<typename T_CHAR>
	struct __String_Utility;

	template<typename T_CHAR>
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

	template<typename T_CHAR>
	class _String
		: public __String_Types<T_CHAR>::Collection
	{
		typedef __String_Types<T_CHAR>     Types;
		typedef __String_Utility<T_CHAR>   Utility;
		typedef __String_Constants<T_CHAR> Constants;

		typename Types::Container container;

	public:
		typedef typename Types::Char                      Char;
		typedef typename Types::Collection                Collection;

		typedef typename Collection::Iterator             Iterator;
		typedef typename Collection::ConstIterator        ConstIterator;

		typedef typename Collection::ReverseIterator      ReverseIterator;
		typedef typename Collection::ConstReverseIterator ConstReverseIterator;

		static constexpr Char                             END  = Constants::END;
		static constexpr size_t                           NPOS = Integer<size_t>::Maximum;

		static size_t GetLength(const Char* lpBuffer, size_t offset = 0)
		{
			size_t stringLength = 0;

			for (auto lpChar = &lpBuffer[offset]; *lpChar != END; ++lpChar)
			{
				++stringLength;
			}

			return stringLength;
		}
		static size_t GetLength(const _String& string, size_t offset = 0)
		{
			auto stringLength = string.GetLength();

			if (offset >= stringLength)
			{

				return 0;
			}

			return stringLength - offset;
		}

		template<typename ... T_CHUNKS>
		static auto Combine(T_CHUNKS ... chunks)
		{
			return Utility::Combine(
				Forward<T_CHUNKS>(chunks) ...
			);
		}

		template<typename T_DELIMITER, typename ... T_CHUNKS>
		static auto Join(T_DELIMITER delimiter, T_CHUNKS ... chunks)
		{
			return Utility::Join(
				Forward<T_DELIMITER>(delimiter),
				Forward<T_CHUNKS>(chunks) ...
			);
		}

		template<typename ... TArgs>
		static auto Format(const Char* format, TArgs ... args);
		template<typename ... TArgs>
		static auto Format(const _String& format, TArgs ... args)
		{
			return Format(
				format.GetCString(),
				Forward<TArgs>(args) ...
			);
		}

		_String()
			: container(
				&END,
				1
			)
		{
		}

		_String(_String&& string)
			: container(
				Move(string.container)
			)
		{
		}
		_String(const _String& string)
			: container(
				string.container
			)
		{
		}

		_String(const Char* lpBuffer)
			: _String(
				lpBuffer,
				GetLength(lpBuffer)
			)
		{
		}
		_String(const Char* lpBuffer, size_t length)
			: container(
				lpBuffer,
				length,
				1
			)
		{
			container.PushBack(
				END
			);
		}

		_String(Char c, size_t count)
			: container(
				count + 1
			)
		{
			container.Fill(
				c,
				count
			);

			container[count] = END;
		}

		virtual ~_String()
		{
		}

		virtual size_t GetSize() const override
		{
			return container.GetSize();
		}

		virtual size_t GetCapacity() const override
		{
			return container.GetCapacity();
		}

		size_t GetLength() const
		{
			return GetSize() - 1;
		}

		const Char* GetCString() const
		{
			return &container[0];
		}

		auto ToUpper() const
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

		auto ToLower() const
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

		Void Swap(_String& string)
		{
			container.Swap(
				string.container
			);
		}

		Void Clear()
		{
			container.SetCapacity(
				1
			);

			container.Fill(
				END
			);
		}

		Void ShrinkToFit()
		{
			container.SetCapacity(
				GetLength() + 1
			);

			container[GetLength()] = END;
		}

		Void SetCapacity(size_t value)
		{
			if (value < GetLength())
			{
				container.SetCapacity(
					value + 1
				);

				container[value] = END;
			}
			else
			{

				container.SetCapacity(
					value + 1
				);
			}
		}

		Void Assign(_String&& string)
		{
			container = Move(
				string.container
			);
		}
		Void Assign(const _String& string)
		{
			container = string.container;
		}
		Void Assign(const _String& string, size_t length)
		{
			Assign(
				string.GetCString(),
				length
			);
		}

		Void Assign(Char c, size_t count)
		{
			container.SetCapacity(
				count + 1
			);

			container.Fill(
				c
			);

			container[count] = END;
		}

		Void Assign(const Char* lpBuffer)
		{
			Assign(
				lpBuffer,
				GetLength(lpBuffer)
			);
		}
		Void Assign(const Char* lpBuffer, size_t length)
		{
			typedef typename Types::Container Container;

			container = Container(
				lpBuffer,
				length,
				1
			);

			container.PushBack(
				END
			);
		}

		Void Append(Char c)
		{
			Insert(
				end(),
				c
			);
		}
		Void Append(const _String& string)
		{
			Insert(
				end(),
				string
			);
		}
		Void Append(const Char* lpBuffer)
		{
			Insert(
				end(),
				lpBuffer
			);
		}
		Void Append(const Char* lpBuffer, size_t length)
		{
			Insert(
				end(),
				lpBuffer,
				length
			);
		}

		Void Insert(size_t index, Char c)
		{
			container.Insert(
				index,
				c
			);
		}
		Void Insert(size_t index, const _String& string)
		{
			container.Insert(
				index,
				string.GetCString(),
				string.GetLength()
			);
		}
		Void Insert(size_t index, const Char* lpBuffer)
		{
			container.Insert(
				index,
				lpBuffer,
				GetLength(lpBuffer)
			);
		}
		Void Insert(size_t index, const Char* lpBuffer, size_t length)
		{
			container.Insert(
				index,
				lpBuffer,
				length
			);
		}

		template<typename T_ITERATOR>
		Void Insert(T_ITERATOR it, Char c)
		{
			container.Insert(
				it,
				c
			);
		}
		template<typename T_ITERATOR>
		Void Insert(T_ITERATOR it, const _String& string)
		{
			container.Insert(
				it,
				string.GetCString(),
				string.GetLength()
			);
		}
		template<typename T_ITERATOR>
		Void Insert(T_ITERATOR it, const Char* lpBuffer)
		{
			container.Insert(
				it,
				lpBuffer,
				GetLength(lpBuffer)
			);
		}
		template<typename T_ITERATOR>
		Void Insert(T_ITERATOR it, const Char* lpBuffer, size_t length)
		{
			container.Insert(
				it,
				lpBuffer,
				length
			);
		}

		size_t IndexOf(Char c, Bool ignoreCase = False) const
		{
			auto index = IndexOf(
				&c,
				1,
				ignoreCase
			);

			return index;
		}
		size_t IndexOf(const _String& string, Bool ignoreCase = False) const
		{
			auto index = IndexOf(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return index;
		}
		size_t IndexOf(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto index = IndexOf(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return index;
		}
		size_t IndexOf(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			auto index = IndexOfAt(
				lpBuffer,
				length,
				0,
				ignoreCase
			);

			return index;
		}

		size_t IndexOfAt(Char c, size_t index, Bool ignoreCase = False) const
		{
			auto _index = IndexOfAt(
				&c,
				1,
				index,
				ignoreCase
			);

			return _index;
		}
		size_t IndexOfAt(const _String& string, size_t index, Bool ignoreCase = False) const
		{
			auto _index = IndexOfAt(
				string.GetCString(),
				string.GetLength(),
				index,
				ignoreCase
			);

			return _index;
		}
		size_t IndexOfAt(const Char* lpBuffer, size_t index, Bool ignoreCase = False) const
		{
			auto _index = IndexOfAt(
				lpBuffer,
				GetLength(lpBuffer),
				index,
				ignoreCase
			);

			return _index;
		}
		size_t IndexOfAt(const Char* lpBuffer, size_t length, size_t index, Bool ignoreCase = False) const
		{
			for (size_t i = index; i < GetLength(); ++i)
			{
				if (!CompareAt(lpBuffer, length, i, ignoreCase))
				{

					continue;
				}

				return i;
			}

			return NPOS;
		}

		size_t IndexOfLast(Char c, Bool ignoreCase = False) const
		{
			auto index = IndexOfLast(
				&c,
				1,
				ignoreCase
			);

			return index;
		}
		size_t IndexOfLast(const _String& string, Bool ignoreCase = False) const
		{
			auto index = IndexOfLast(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return index;
		}
		size_t IndexOfLast(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto index = IndexOfLast(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return index;
		}
		size_t IndexOfLast(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			if (length <= GetLength())
			{
				for (size_t i = 0, j = GetLength() - length; i <= (GetLength() - length); ++i, --j)
				{
					if (!CompareAt(lpBuffer, length, j, ignoreCase))
					{

						continue;
					}

					return j;
				}
			}

			return NPOS;
		}

		Iterator Find(Char c, Bool ignoreCase = False)
		{
			auto it = Find(
				&c,
				1,
				ignoreCase
			);

			return it;
		}
		Iterator Find(const _String& string, Bool ignoreCase = False)
		{
			auto it = Find(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return it;
		}
		Iterator Find(const Char* lpBuffer, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = Find(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return it;
		}
		Iterator Find(const Char* lpBuffer, size_t length, Bool ignoreCase = False)
		{
			auto it = FindAt(
				lpBuffer,
				length,
				0,
				ignoreCase
			);

			return it;
		}

		ConstIterator Find(Char c, Bool ignoreCase = False) const
		{
			auto it = Find(
				&c,
				1,
				ignoreCase
			);

			return it;
		}
		ConstIterator Find(const _String& string, Bool ignoreCase = False) const
		{
			auto it = Find(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return it;
		}
		ConstIterator Find(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = Find(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return it;
		}
		ConstIterator Find(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			auto it = FindAt(
				lpBuffer,
				length,
				0,
				ignoreCase
			);

			return it;
		}

		Iterator FindAt(Char c, size_t index, Bool ignoreCase = False)
		{
			auto it = FindAt(
				&c,
				1,
				index,
				ignoreCase
			);

			return it;
		}
		Iterator FindAt(const _String& string, size_t index, Bool ignoreCase = False)
		{
			auto it = FindAt(
				string.GetCString(),
				string.GetLength(),
				index,
				ignoreCase
			);

			return it;
		}
		Iterator FindAt(const Char* lpBuffer, size_t index, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = FindAt(
				lpBuffer,
				stringLength,
				index,
				ignoreCase
			);

			return it;
		}
		Iterator FindAt(const Char* lpBuffer, size_t length, size_t index, Bool ignoreCase = False)
		{
			for (size_t i = index; i < GetLength(); ++i)
			{
				if (!CompareAt(lpBuffer, length, i, ignoreCase))
				{

					continue;
				}

				return Iterator(
					&container[i]
				);
			}

			return end();
		}

		ConstIterator FindAt(Char c, size_t index, Bool ignoreCase = False) const
		{
			auto it = FindAt(
				&c,
				1,
				index,
				ignoreCase
			);

			return it;
		}
		ConstIterator FindAt(const _String& string, size_t index, Bool ignoreCase = False) const
		{
			auto it = FindAt(
				string.GetCString(),
				string.GetLength(),
				index,
				ignoreCase
			);

			return it;
		}
		ConstIterator FindAt(const Char* lpBuffer, size_t index, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = FindAt(
				lpBuffer,
				stringLength,
				index,
				ignoreCase
			);

			return it;
		}
		ConstIterator FindAt(const Char* lpBuffer, size_t length, size_t index, Bool ignoreCase = False) const
		{
			for (size_t i = index; i < GetLength(); ++i)
			{
				if (!CompareAt(lpBuffer, length, i, ignoreCase))
				{

					continue;
				}

				return ConstIterator(
					&container[i]
				);
			}

			return cend();
		}

		Iterator FindLast(Char c, Bool ignoreCase = False)
		{
			auto it = FindLast(
				&c,
				1,
				ignoreCase
			);

			return it;
		}
		Iterator FindLast(const _String& string, Bool ignoreCase = False)
		{
			auto it = FindLast(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return it;
		}
		Iterator FindLast(const Char* lpBuffer, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = FindLast(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return it;
		}
		Iterator FindLast(const Char* lpBuffer, size_t length, Bool ignoreCase = False)
		{
			if (length <= GetLength())
			{
				for (size_t i = 0, j = GetLength() - length; i <= (GetLength() - length); ++i, --j)
				{
					if (!CompareAt(lpBuffer, length, j, ignoreCase))
					{

						continue;
					}

					return Iterator(
						&container[j]
					);
				}
			}
			
			return end();
		}

		ConstIterator FindLast(Char c, Bool ignoreCase = False) const
		{
			auto it = FindLast(
				&c,
				1,
				ignoreCase
			);

			return it;
		}
		ConstIterator FindLast(const _String& string, Bool ignoreCase = False) const
		{
			auto it = FindLast(
				string.GetCString(),
				string.GetLength(),
				ignoreCase
			);

			return it;
		}
		ConstIterator FindLast(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			auto it = FindLast(
				lpBuffer,
				stringLength,
				ignoreCase
			);

			return it;
		}
		ConstIterator FindLast(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			if (length <= GetLength())
			{
				for (size_t i = 0, j = GetLength() - length; i <= (GetLength() - length); ++i, --j)
				{
					if (!CompareAt(lpBuffer, length, j, ignoreCase))
					{

						continue;
					}

					return ConstIterator(
						&container[j]
					);
				}
			}
			
			return cend();
		}

		Bool Contains(Char c, Bool ignoreCase = False) const
		{
			auto lpChar = &container[0];

			for (size_t i = 0; i < GetLength(); ++i, ++lpChar)
			{
				if (*lpChar == c)
				{

					return True;
				}
				else if (ignoreCase && (Utility::ToLower(*lpChar) == Utility::ToLower(c)))
				{

					return True;
				}
			}

			return False;
		}
		Bool Contains(const _String& string, Bool ignoreCase = False) const
		{
			if (!Contains(string.GetCString(), string.GetLength(), ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool Contains(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			if (!Contains(lpBuffer, stringLength, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool Contains(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			auto it = Find(
				lpBuffer,
				length,
				ignoreCase
			);

			if (it == cend())
			{

				return False;
			}

			return True;
		}

		Bool StartsWith(Char c, Bool ignoreCase = False) const
		{
			if (!StartsWith(&c, 1, ignoreCase))
			{
				
				return False;
			}

			return True;
		}
		Bool StartsWith(const _String& string, Bool ignoreCase = False) const
		{
			if (!StartsWith(string.GetCString(), string.GetLength(), ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool StartsWith(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);
			
			if (!StartsWith(lpBuffer, stringLength, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool StartsWith(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			if (!CompareAt(lpBuffer, length, 0, ignoreCase))
			{

				return False;
			}

			return True;
		}

		Bool EndsWith(Char c, Bool ignoreCase = False) const
		{
			if (!EndsWith(&c, 1, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool EndsWith(const _String& string, Bool ignoreCase = False) const
		{
			if (!StartsWith(string.GetCString(), string.GetLength(), ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool EndsWith(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			if (!EndsWith(lpBuffer, stringLength, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool EndsWith(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			if (length <= GetLength())
			{
				if (CompareAt(lpBuffer, length, GetLength() - length, ignoreCase))
				{

					return True;
				}
			}

			return False;
		}

		Bool Compare(Char c, Bool ignoreCase = False) const
		{
			if (!Compare(&c, 1, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool Compare(const _String& string, Bool ignoreCase = False) const
		{
			if (!Compare(string.GetCString(), string.GetLength(), ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool Compare(const Char* lpBuffer, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			if (!Compare(lpBuffer, stringLength, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool Compare(const Char* lpBuffer, size_t length, Bool ignoreCase = False) const
		{
			if (!CompareAt(lpBuffer, length, 0, ignoreCase))
			{

				return False;
			}

			return True;
		}

		Bool CompareAt(Char c, size_t index, Bool ignoreCase = False) const
		{
			if (!CompareAt(&c, 1, index, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool CompareAt(const _String& string, size_t index, Bool ignoreCase = False) const
		{
			if (!CompareAt(string.GetCString(), string.GetLength(), index, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool CompareAt(const Char* lpBuffer, size_t index, Bool ignoreCase = False) const
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			if (!CompareAt(lpBuffer, stringLength, index, ignoreCase))
			{

				return False;
			}

			return True;
		}
		Bool CompareAt(const Char* lpBuffer, size_t length, size_t index, Bool ignoreCase = False) const
		{
			if ((index + length) > GetLength())
			{

				return False;
			}

			auto lpContainer = &container[index];

			if (!ignoreCase)
			{
				if (!memcmp(lpContainer, lpBuffer, length * sizeof(Char)))
				{

					return False;
				}
			}
			else
			{
				for (size_t i = 0; i < length; ++i, ++lpBuffer, ++lpContainer)
				{
					if (Utility::ToLower(*lpContainer) != Utility::ToLower(*lpBuffer))
					{

						return False;
					}
				}
			}

			return True;
		}

		Void Remove(Char value, Bool ignoreCase = False)
		{
			Remove(
				&value,
				1,
				ignoreCase
			);
		}
		Void Remove(const _String& value, Bool ignoreCase = False)
		{
			Remove(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		Void Remove(const Char* lpBuffer, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			Remove(
				lpBuffer,
				stringLength,
				ignoreCase
			);
		}
		Void Remove(const Char* lpBuffer, size_t length, Bool ignoreCase = False)
		{
			auto i = IndexOf(
				lpBuffer,
				length,
				ignoreCase
			);

			if (i != NPOS)
			{

				Erase(
					i,
					length
				);
			}
		}

		Void RemoveAll(Char value, Bool ignoreCase = False)
		{
			RemoveAll(
				&value,
				1,
				ignoreCase
			);
		}
		Void RemoveAll(const _String& value, Bool ignoreCase = False)
		{
			RemoveAll(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		Void RemoveAll(const Char* lpBuffer, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			RemoveAll(
				lpBuffer,
				stringLength,
				ignoreCase
			);
		}
		Void RemoveAll(const Char* lpBuffer, size_t length, Bool ignoreCase = False)
		{
			auto i = IndexOf(
				lpBuffer,
				length,
				ignoreCase
			);

			while (i != NPOS)
			{
				Erase(
					i,
					length
				);

				i = IndexOf(
					lpBuffer,
					length,
					ignoreCase
				);
			}
		}

		Void RemoveLast(Char value, Bool ignoreCase = False)
		{
			RemoveLast(
				&value,
				1,
				ignoreCase
			);
		}
		Void RemoveLast(const _String& value, Bool ignoreCase = False)
		{
			RemoveLast(
				value.GetCString(),
				value.GetLength(),
				ignoreCase
			);
		}
		Void RemoveLast(const Char* lpBuffer, Bool ignoreCase = False)
		{
			auto stringLength = GetLength(
				lpBuffer
			);

			RemoveLast(
				lpBuffer,
				stringLength,
				ignoreCase
			);
		}
		Void RemoveLast(const Char* lpBuffer, size_t length, Bool ignoreCase = False)
		{
			auto i = IndexOfLast(
				lpBuffer,
				length,
				ignoreCase
			);

			if (i != NPOS)
			{
				Erase(
					i,
					length
				);
			}
		}

		Void Erase(size_t index)
		{
			Erase(
				index,
				1
			);
		}
		Void Erase(size_t index, size_t length)
		{
			auto _length = GetLength();

			if (index < _length)
			{
				if (length > (_length - index))
				{

					length = _length - index;
				}

				container.Erase(
					index,
					length
				);
			}
		}

		Void Erase(Iterator it)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Erase(
				index
			);
		}
		Void Erase(ConstIterator it)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Erase(
				index
			);
		}
		Void Erase(ReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}
		Void Erase(ConstReverseIterator it)
		{
			auto index = GetIteratorDifference(
				crbegin(),
				it
			);

			Erase(
				index
			);
		}

		Void Erase(Iterator first, Iterator last)
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				length
			);
		}
		Void Erase(ConstIterator first, ConstIterator last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				length
			);
		}
		Void Erase(ReverseIterator first, ReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				length
			);
		}
		Void Erase(ConstReverseIterator first, ConstReverseIterator last)
		{
			auto index = GetIteratorDifference(
				crbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				length
			);
		}

		Collections::Array<_String> Split(Char delimiter, Bool ignoreCase = False) const
		{
			auto strings = Split(
				&delimiter,
				1,
				ignoreCase
			);

			return strings;
		}
		Collections::Array<_String> Split(const _String& delimiter, Bool ignoreCase = False) const
		{
			auto strings = Split(
				delimiter.GetCString(),
				delimiter.GetLength(),
				ignoreCase
			);

			return strings;
		}
		Collections::Array<_String> Split(const Char* delimiter, Bool ignoreCase = False) const
		{
			auto delimiterLength = GetLength(
				delimiter
			);

			auto strings = Split(
				delimiter,
				delimiterLength,
				ignoreCase
			);

			return strings;
		}
		Collections::Array<_String> Split(const Char* delimiter, size_t length, Bool ignoreCase = False) const
		{
			// TODO: get comma count and remove extra contained
			Collections::ArrayList<_String> chunks;

			auto _length = GetLength();

			for (size_t i = 0; i < _length; )
			{
				auto end = IndexOfAt(
					delimiter,
					length,
					i,
					ignoreCase
				);

				if (end == NPOS)
				{

					end = _length;
				}

				auto chunk = SubString(
					i,
					end - (_length - i)
				);

				chunks.PushBack(
					Move(chunk)
				);

				i += chunk.GetLength();
			}

			Collections::Array<_String> _chunks(
				chunks.GetSize()
			);

			for (auto it1 = chunks.begin(), it2 = _chunks.begin(); it1 != chunks.end(); ++it1, ++it2)
			{
				*it2 = Move(
					*it1
				);
			}

			return _chunks;
		}

		_String SubString(size_t index) const
		{
			auto string = SubString(
				index,
				GetLength() - index
			);

			return string;
		}
		_String SubString(size_t index, size_t length) const
		{
			_String string;

			auto _length = GetLength();

			if (index < _length)
			{
				if (length >= (_length - index))
				{

					length = _length - index;
				}

				string.Assign(
					&container[index],
					length
				);
			}

			return string;
		}

		_String SubString(Iterator it)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			auto string = SubString(
				index
			);

			return string;
		}
		_String SubString(ConstIterator it) const
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			auto string = SubString(
				index
			);

			return string;
		}
		_String SubString(ReverseIterator it) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			auto string = SubString(
				index
			);

			return string;
		}
		_String SubString(ConstReverseIterator it) const
		{
			auto index = GetIteratorDifference(
				crbegin(),
				it
			);

			auto string = SubString(
				index
			);

			return string;
		}

		_String SubString(Iterator first, Iterator last)
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			auto string = SubString(
				index,
				length
			);
		}
		_String SubString(ConstIterator first, ConstIterator last) const
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			auto string = SubString(
				index,
				length
			);
		}
		_String SubString(ReverseIterator first, ReverseIterator last) const
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			auto string = SubString(
				index,
				length
			);
		}
		_String SubString(ConstReverseIterator first, ConstReverseIterator last) const
		{
			auto index = GetIteratorDifference(
				crbegin(),
				first
			);

			auto length = GetIteratorDifference(
				first,
				last
			);

			auto string = SubString(
				index,
				length
			);
		}

		virtual Iterator begin() override
		{
			return container.begin();
		}
		virtual ConstIterator begin() const override
		{
			return container.begin();
		}

		virtual Iterator end() override
		{
			return --container.end();
		}
		virtual ConstIterator end() const override
		{
			return --container.end();
		}

		virtual ConstIterator cbegin() const override
		{
			return container.cbegin();
		}

		virtual ConstIterator cend() const override
		{
			return --container.cend();
		}

		virtual ReverseIterator rbegin() override
		{
			return --container.rbegin();
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return --container.rbegin();
		}

		virtual ReverseIterator rend() override
		{
			return container.rend();
		}
		virtual ConstReverseIterator rend() const override
		{
			return container.rend();
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return --container.crbegin();
		}

		virtual ConstReverseIterator crend() const override
		{
			return container.crend();
		}

		Char& operator [] (size_t index)
		{
			AL_ASSERT(
				index < GetLength(),
				"index must be less than length"
			);

			return container[index];
		}
		const Char& operator [] (size_t index) const
		{
			AL_ASSERT(
				index < GetLength(),
				"index must be less than length"
			);

			return container[index];
		}

		_String& operator = (_String&& string)
		{
			Assign(
				Move(string)
			);

			return *this;
		}
		_String& operator = (const _String& string)
		{
			Assign(
				string
			);

			return *this;
		}

		_String& operator = (const Char* lpBuffer)
		{
			Assign(
				lpBuffer
			);

			return *this;
		}

		Bool operator == (const Char* lpBuffer) const
		{
			auto bufferLength = GetLength(
				lpBuffer
			);

			if (bufferLength != GetLength())
			{

				return False;
			}

			if (!memcmp(GetCString(), lpBuffer, bufferLength * sizeof(Char)))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Char* lpBuffer) const
		{
			if (operator==(lpBuffer))
			{

				return False;
			}

			return True;
		}

		Bool operator == (const _String& string) const
		{
			auto stringLength = string.GetLength();

			if (GetLength() != stringLength)
			{

				return False;
			}

			if (!memcmp(GetCString(), string.GetCString(), stringLength * sizeof(Char)))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const _String& string) const
		{
			if (operator==(string))
			{

				return False;
			}

			return True;
		}
	};

	typedef _String<char>    String;
	typedef _String<wchar_t> WString;

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

		static constexpr Bool IsLower(char c)
		{
			return ((c >= 'a') && (c <= 'z')) && (__String_Constants<char>::LOWERCASE_TABLE[c - 'a'] == c);
		}

		static constexpr Bool IsUpper(char c)
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
		static Void Concat(_String<char>& buffer)
		{
		}
		template<typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static Void Concat(_String<char>& buffer, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
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
		static Void Concat2(_String<char>& buffer, DELIMITER delimiter)
		{
		}
		template<typename DELIMITER, typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static Void Concat2(_String<char>& buffer, DELIMITER delimiter, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
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
		
		static constexpr Bool IsLower(wchar_t c)
		{
			return ((c >= L'a') && (c <= L'z')) && (__String_Constants<wchar_t>::LOWERCASE_TABLE[c - L'a'] == c);
		}

		static constexpr Bool IsUpper(wchar_t c)
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
		static Void Concat(_String<wchar_t>& buffer)
		{
		}
		template<typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static Void Concat(_String<wchar_t>& buffer, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
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
		static Void Concat2(_String<wchar_t>& buffer, DELIMITER delimiter)
		{
		}
		template<typename DELIMITER, typename CHUNK_TYPE, typename ... CHUNK_TYPES>
		static Void Concat2(_String<wchar_t>& buffer, DELIMITER delimiter, CHUNK_TYPE chunk, CHUNK_TYPES ... chunks)
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
}

template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<char>::Format(const Char* format, TArgs ... args)
{
	auto bufferLength = ::std::snprintf(
		nullptr,
		0,
		format,
		Forward<TArgs>(args) ...
	);

	auto lpBuffer = new char[bufferLength + 1];

	::std::snprintf(
		lpBuffer,
		bufferLength + 1,
		format,
		Forward<TArgs>(args) ...
	);

	_String string(
		lpBuffer,
		bufferLength
	);

	delete[] lpBuffer;

	return string;
}
template<>
template<typename ... TArgs>
inline auto AL::Collections::_String<wchar_t>::Format(const Char* format, TArgs ... args)
{
	auto bufferLength = ::std::swprintf(
		nullptr,
		0,
		format,
		Forward<TArgs>(args) ...
	);

	auto lpBuffer = new wchar_t[bufferLength + 1];

	::std::swprintf(
		lpBuffer,
		bufferLength + 1,
		format,
		Forward<TArgs>(args) ...
	);

	_String string(
		lpBuffer,
		bufferLength
	);

	delete[] lpBuffer;

	return string;
}
