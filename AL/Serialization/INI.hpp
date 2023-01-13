#pragma once
#include "AL/Common.hpp"

#include "Text.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

namespace AL::Serialization
{
	enum class INIFieldAttributes
	{
		Bool        = 0x1,
		Struct      = 0x2,
		String      = 0x4,
		Integer     = 0x8,
		Decimal     = 0x10,
		Unsigned    = 0x20,
		Hexadecimal = 0x40
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(INIFieldAttributes);

	class INI;

	class INIField
	{
		friend INI;

		static constexpr WString::Char REGEX_PATTERN[]                       = L"^([^ ]+) *= *(.*)$";

		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_BOOL[]        = L"^([Tt][Rr][Uu][Ee]|[Ff][Aa][Ll][Ss][Ee])$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_STRUCT[]      = L"^\"(([0-9A-Fa-f]{2})+)\"$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_STRING[]      = L"^\"([^\"]*)\"$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_INTEGER[]     = L"^(-?\\d+)$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_DECIMAL[]     = L"^(-?\\d+(\\.\\d+)?)$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_UNSIGNED[]    = L"^(-{0}\\d+(\\.\\d+)?)$";
		static constexpr WString::Char REGEX_PATTERN_DATA_TYPE_HEXADECIMAL[] = L"^0x([0-9A-Fa-f]+)$";

		WString name;
		WString value;

		BitMask<INIFieldAttributes> attributes;

	public:
		INIField()
		{
		}

		INIField(String&& name, String&& value)
			: INIField(
				name.ToWString(),
				value.ToWString()
			)
		{
		}
		INIField(WString&& name, WString&& value)
			: name(
				Move(name)
			),
			value(
				Move(value)
			)
		{
			if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_BOOL, this->value))
			{
				attributes.Value = INIFieldAttributes::Bool;
			}
			else if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_STRUCT, this->value))
			{
				attributes.Value = INIFieldAttributes::Struct;
				attributes.Value |= INIFieldAttributes::String;
			}
			else if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_STRING, this->value))
			{
				attributes.Value = INIFieldAttributes::String;
			}
			else if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_INTEGER, this->value))
			{
				attributes.Value = INIFieldAttributes::Integer;
				attributes.Value |= INIFieldAttributes::Decimal;

				attributes.Set(
					INIFieldAttributes::Unsigned,
					RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
			else if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_DECIMAL, this->value))
			{
				attributes.Value = INIFieldAttributes::Decimal;

				attributes.Set(
					INIFieldAttributes::Unsigned,
					RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
			else if (RegexW::IsMatch(REGEX_PATTERN_DATA_TYPE_HEXADECIMAL, this->value))
			{
				attributes.Value = INIFieldAttributes::Integer;
				attributes.Value |= INIFieldAttributes::Decimal;
				attributes.Value |= INIFieldAttributes::Unsigned;
				attributes.Value |= INIFieldAttributes::Hexadecimal;
			}
		}

		Bool IsBool() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Bool))
			{

				return False;
			}

			return True;
		}

		Bool IsStruct() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Struct))
			{

				return False;
			}

			return True;
		}

		Bool IsString() const
		{
			if (!attributes.IsSet(INIFieldAttributes::String))
			{

				return False;
			}

			return True;
		}

		Bool IsInteger() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Integer))
			{

				return False;
			}

			return True;
		}

		Bool IsDecimal() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Decimal))
			{

				return False;
			}

			return True;
		}

		Bool IsUnsigned() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Unsigned))
			{

				return False;
			}

			return True;
		}

		Bool IsHexadecimal() const
		{
			if (!attributes.IsSet(INIFieldAttributes::Hexadecimal))
			{

				return False;
			}

			return True;
		}

		auto& GetName() const
		{
			return name;
		}

		auto& GetValue() const
		{
			return value;
		}

		BitMask<INIFieldAttributes> GetAttributes() const
		{
			return attributes;
		}

		auto GetBool() const
		{
			AL_ASSERT(
				IsBool(),
				"INIField type is not Bool"
			);

			return FromWString<Bool>(
				GetValue()
			);
		}

		template<typename T>
		auto GetStruct() const
		{
			AL_ASSERT(
				IsStruct(),
				"INIField type is not struct"
			);

			return HexConverter::Decode<T>(
				GetString()
			);
		}

		auto GetStructAsBuffer() const
		{
			AL_ASSERT(
				IsStruct(),
				"INIField type is not struct"
			);

			BaseConverterBuffer buffer;

			HexConverter::Decode(
				buffer,
				GetString()
			);

			return buffer;
		}

		String GetString() const
		{
			AL_ASSERT(
				IsString(),
				"INIField type is not string"
			);

			auto wstring = GetStringW();

			return wstring.ToString();
		}
		WString GetStringW() const
		{
			AL_ASSERT(
				IsString(),
				"INIField type is not string"
			);

			return GetValue().SubString(
				1,
				GetValue().GetLength() - 2
			);
		}

		template<typename T>
		auto GetInteger() const
		{
			AL_ASSERT(
				IsInteger(),
				"INIField type is not integer"
			);

			return FromWString<T>(
				GetValue()
			);
		}

		template<typename T>
		auto GetDecimal() const
		{
			AL_ASSERT(
				IsDecimal(),
				"INIField type is not decimal"
			);

			return FromWString<T>(
				GetValue()
			);
		}

		Void SetBool(Bool value)
		{
			this->value = ToWString(
				value
			);

			attributes.Value = INIFieldAttributes::Bool;
		}

		template<typename T>
		Void SetStruct(const T& value)
		{
			SetStructAsBuffer(
				&value,
				sizeof(T)
			);
		}

		Void SetStructAsBuffer(const Void* lpBuffer, size_t size)
		{
			this->value = WString::Format(
				L"\"%S\"",
				HexConverter::Encode(lpBuffer, size).GetCString()
			);

			attributes.Value = INIFieldAttributes::Struct;
			attributes.Value |= INIFieldAttributes::String;
		}

		Void SetString(const String& value)
		{
			this->value = WString::Format(
				L"\"%S\"",
				value.GetCString()
			);

			attributes.Value = INIFieldAttributes::String;
		}
		Void SetString(const WString& value)
		{
			this->value = WString::Format(
				L"\"%s\"",
				value.GetCString()
			);

			attributes.Value = INIFieldAttributes::String;
		}

		template<typename T>
		Void SetInteger(T value, Bool hexadecimal = False)
		{
			if (hexadecimal)
			{
				this->value = WString::Format(
					L"0x%S",
					HexConverter::Encode(value).GetCString()
				);
			}
			else
			{
				this->value = ToWString(
					value
				);
			}

			attributes.Value = INIFieldAttributes::Integer;
			attributes.Value |= INIFieldAttributes::Decimal;
			attributes.Set(INIFieldAttributes::Unsigned, Is_Unsigned<T>::Value);
			attributes.Set(INIFieldAttributes::Hexadecimal, hexadecimal);
		}

		template<typename T>
		Void SetDecimal(T value)
		{
			this->value = ToWString(
				value
			);

			attributes.Value = INIFieldAttributes::Decimal;
		}
	};

	class INISection
	{
		friend INI;

		typedef Collections::LinkedList<INIField> FieldContainer;

		static constexpr WString::Char REGEX_PATTERN[] = L"^\\[([^\\]]*)\\]$";

		WString        name;
		FieldContainer fields;

	public:
		typedef typename FieldContainer::Iterator             Iterator;
		typedef typename FieldContainer::ConstIterator        ConstIterator;

		typedef typename FieldContainer::ReverseIterator      ReverseIterator;
		typedef typename FieldContainer::ConstReverseIterator ConstReverseIterator;

		INISection()
		{
		}

		explicit INISection(String&& name)
			: INISection(
				name.ToWString()
			)
		{
		}
		explicit INISection(WString&& name)
			: name(
				Move(name)
			)
		{
		}

		auto& GetName() const
		{
			return name;
		}

		auto GetFieldCount() const
		{
			return fields.GetSize();
		}

		Void Clear()
		{
			fields.Clear();
		}

		Iterator      begin()
		{
			return fields.begin();
		}
		ConstIterator begin() const
		{
			return fields.begin();
		}

		Iterator      end()
		{
			return fields.end();
		}
		ConstIterator end() const
		{
			return fields.end();
		}

		ConstIterator cbegin() const
		{
			return fields.cbegin();
		}

		ConstIterator cend() const
		{
			return fields.cend();
		}

		ReverseIterator      rbegin()
		{
			return fields.rbegin();
		}
		ConstReverseIterator rbegin() const
		{
			return fields.rbegin();
		}

		ReverseIterator      rend()
		{
			return fields.rend();
		}
		ConstReverseIterator rend() const
		{
			return fields.rend();
		}

		ConstReverseIterator crbegin() const
		{
			return fields.crbegin();
		}

		ConstReverseIterator crend() const
		{
			return fields.crend();
		}

		INIField& operator [] (String&& fieldName)
		{
			return operator[](
				fieldName.ToWString()
			);
		}
		INIField& operator [] (WString&& fieldName)
		{
			for (auto& field : fields)
			{
				if (field.GetName().Compare(fieldName))
				{

					return field;
				}
			}

			fields.EmplaceBack(
				Move(fieldName),
				WString(L"\"\"")
			);

			return *(--fields.end());
		}

		INIField& operator [] (const String& fieldName)
		{
			return operator[](
				fieldName.ToWString()
			);
		}
		INIField& operator [] (const WString& fieldName)
		{
			for (auto& field : fields)
			{
				if (field.GetName().Compare(fieldName))
				{

					return field;
				}
			}

			fields.EmplaceBack(
				WString(fieldName),
				WString(L"\"\"")
			);

			return *(--fields.end());
		}

		const INIField& operator [] (const String& fieldName) const
		{
			return operator[](
				fieldName.ToWString()
			);
		}
		const INIField& operator [] (const WString& fieldName) const
		{
			const INIField* lpField = nullptr;

			for (auto& field : fields)
			{
				if (field.GetName().Compare(fieldName))
				{
					lpField = &field;
					
					break;
				}
			}

			AL_ASSERT(
				lpField != nullptr,
				"INIField not found"
			);

			return *lpField;
		}
	};

	class INI
	{
		typedef Collections::LinkedList<INISection> SectionContainer;

		SectionContainer sections;

	public:
		typedef typename SectionContainer::Iterator             Iterator;
		typedef typename SectionContainer::ConstIterator        ConstIterator;

		typedef typename SectionContainer::ReverseIterator      ReverseIterator;
		typedef typename SectionContainer::ConstReverseIterator ConstReverseIterator;

		// @throw AL::Exception
		static Void FromString(INI& ini, const String& string)
		{
			auto wstring = string.ToWString();

			FromString(
				ini,
				wstring
			);
		}
		// @throw AL::Exception
		static Void FromString(INI& ini, const WString& wstring)
		{
			Text text(
				wstring
			);

			WString line;

			RegexW regexSection(
				INISection::REGEX_PATTERN
			);

			RegexW regexSectionField(
				INIField::REGEX_PATTERN
			);

			RegexW::MatchCollection regexMatchCollection;

			ini.sections.Clear();

			while (text.ReadLine(line))
			{
				if (!regexSection.Match(regexMatchCollection, line))
				{

					continue;
				}

			read_section:
				INISection section(
					Move(regexMatchCollection[1])
				);

				while (text.ReadLine(line))
				{
					if (regexSectionField.Match(regexMatchCollection, line))
					{
						section.fields.EmplaceBack(
							Move(regexMatchCollection[1]),
							Move(regexMatchCollection[2])
						);
					}
					else if (regexSection.Match(regexMatchCollection, line))
					{
						ini.sections.PushBack(
							Move(section)
						);

						goto read_section;
					}
				}

				ini.sections.PushBack(
					Move(section)
				);
			}
		}

		INI()
		{
		}

		INI(INI&& ini)
			: sections(
				Move(ini.sections)
			)
		{
		}
		INI(const INI& ini)
			: sections(
				ini.sections
			)
		{
		}

		virtual ~INI()
		{
		}

		auto GetSectionCount() const
		{
			return sections.GetSize();
		}

		String  ToString() const
		{
			auto wstring = ToWString();

			return wstring.ToString();
		}
		WString ToWString() const
		{
			Text text;

			for (auto& section : sections)
			{
				text.WriteLine(
					WString::Format(
						L"[%s]",
						section.GetName().GetCString()
					)
				);

				for (auto& field : section.fields)
				{
					text.WriteLine(
						WString::Format(
							L"%s = %s",
							field.GetName().GetCString(),
							field.GetValue().GetCString()
						)
					);
				}

				text.WriteLine();
			}

			return text.ToWString();
		}

		Void Clear()
		{
			sections.Clear();
		}

		Iterator      begin()
		{
			return sections.begin();
		}
		ConstIterator begin() const
		{
			return sections.begin();
		}

		Iterator      end()
		{
			return sections.end();
		}
		ConstIterator end() const
		{
			return sections.end();
		}

		ConstIterator cbegin() const
		{
			return sections.cbegin();
		}

		ConstIterator cend() const
		{
			return sections.cend();
		}

		ReverseIterator      rbegin()
		{
			return sections.rbegin();
		}
		ConstReverseIterator rbegin() const
		{
			return sections.rbegin();
		}

		ReverseIterator      rend()
		{
			return sections.rend();
		}
		ConstReverseIterator rend() const
		{
			return sections.rend();
		}

		ConstReverseIterator crbegin() const
		{
			return sections.crbegin();
		}

		ConstReverseIterator crend() const
		{
			return sections.crend();
		}

		INISection& operator [] (String&& sectionName)
		{
			return operator[](
				sectionName.ToWString()
			);
		}
		INISection& operator [] (WString&& sectionName)
		{
			for (auto& section : sections)
			{
				if (section.GetName().Compare(sectionName))
				{

					return section;
				}
			}

			sections.EmplaceBack(
				Move(sectionName)
			);

			return *(--sections.end());
		}

		INISection& operator [] (const String& sectionName)
		{
			return operator[](
				sectionName.ToWString()
			);
		}
		INISection& operator [] (const WString& sectionName)
		{
			for (auto& section : sections)
			{
				if (section.GetName().Compare(sectionName))
				{

					return section;
				}
			}

			sections.EmplaceBack(
				WString(sectionName)
			);

			return *(--sections.end());
		}

		const INISection& operator [] (const String& sectionName) const
		{
			return operator[](
				sectionName.ToWString()
			);
		}
		const INISection& operator [] (const WString& sectionName) const
		{
			const INISection* lpSection = nullptr;

			for (auto& section : sections)
			{
				if (section.GetName().Compare(sectionName))
				{
					lpSection = &section;
					
					break;
				}
			}

			AL_ASSERT(
				lpSection != nullptr,
				"INISection not found"
			);

			return *lpSection;
		}
	};
}
