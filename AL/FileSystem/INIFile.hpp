#pragma once
#include "AL/Common.hpp"

#include "TextFile.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

namespace AL::FileSystem
{
	enum class INIFileFieldAttributes
	{
		Bool        = 0x1,
		Struct      = 0x2,
		String      = 0x4,
		Integer     = 0x8,
		Decimal     = 0x10,
		Unsigned    = 0x20,
		Hexadecimal = 0x40
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(INIFileFieldAttributes);

	class INIFile;

	class INIFileField
	{
		friend INIFile;

		static constexpr char REGEX_PATTERN[] = "^([^ ]+) *= *(.*)$";

		static constexpr char REGEX_PATTERN_DATA_TYPE_BOOL[] = "^([Tt][Rr][Uu][Ee]|[Ff][Aa][Ll][Ss][Ee])$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_STRUCT[] = "^\"(([0-9A-Fa-f]{2})+)\"$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_STRING[] = "^\"([^\"]*)\"$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_INTEGER[] = "^(-?\\d+)$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_DECIMAL[] = "^(-?\\d+(\\.\\d+)?)$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_UNSIGNED[] = "^(-{0}\\d+(\\.\\d+)?)$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_HEXADECIMAL[] = "^0x([0-9A-Fa-f]+)$";

		String name;
		String value;

		BitMask<INIFileFieldAttributes> attributes;

	public:
		INIFileField()
		{
		}

		INIFileField(String&& name, String&& value)
			: name(
				Move(name)
			),
			value(
				Move(value)
			)
		{
			if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_BOOL, this->value))
			{
				attributes.Value = INIFileFieldAttributes::Bool;
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_STRUCT, this->value))
			{
				attributes.Value = INIFileFieldAttributes::Struct;
				attributes.Value |= INIFileFieldAttributes::String;
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_STRING, this->value))
			{
				attributes.Value = INIFileFieldAttributes::String;
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_INTEGER, this->value))
			{
				attributes.Value = INIFileFieldAttributes::Integer;
				attributes.Value |= INIFileFieldAttributes::Decimal;

				attributes.Set(
					INIFileFieldAttributes::Unsigned,
					Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_DECIMAL, this->value))
			{
				attributes.Value = INIFileFieldAttributes::Decimal;

				attributes.Set(
					INIFileFieldAttributes::Unsigned,
					Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_HEXADECIMAL, this->value))
			{
				attributes.Value = INIFileFieldAttributes::Integer;
				attributes.Value |= INIFileFieldAttributes::Decimal;
				attributes.Value |= INIFileFieldAttributes::Unsigned;
				attributes.Value |= INIFileFieldAttributes::Hexadecimal;
			}
		}

		Bool IsBool() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Bool))
			{

				return False;
			}

			return True;
		}

		Bool IsStruct() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Struct))
			{

				return False;
			}

			return True;
		}

		Bool IsString() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::String))
			{

				return False;
			}

			return True;
		}

		Bool IsInteger() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Integer))
			{

				return False;
			}

			return True;
		}

		Bool IsDecimal() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Decimal))
			{

				return False;
			}

			return True;
		}

		Bool IsUnsigned() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Unsigned))
			{

				return False;
			}

			return True;
		}

		Bool IsHexadecimal() const
		{
			if (!attributes.IsSet(INIFileFieldAttributes::Hexadecimal))
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

		BitMask<INIFileFieldAttributes> GetAttributes() const
		{
			return attributes;
		}

		auto GetBool() const
		{
			AL_ASSERT(
				IsBool(),
				"INIFileField type is not Bool"
			);

			return FromString<Bool>(
				GetValue()
			);
		}

		template<typename T>
		auto GetStruct() const
		{
			AL_ASSERT(
				IsStruct(),
				"INIFileField type is not struct"
			);

			return HexConverter::Decode<T>(
				GetString()
			);
		}

		auto GetStructAsBuffer() const
		{
			AL_ASSERT(
				IsStruct(),
				"INIFileField type is not struct"
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
				"INIFileField type is not string"
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
				"INIFileField type is not integer"
			);

			return FromString<T>(
				GetValue()
			);
		}

		template<typename T>
		auto GetDecimal() const
		{
			AL_ASSERT(
				IsDecimal(),
				"INIFileField type is not decimal"
			);

			return FromString<T>(
				GetValue()
			);
		}

		Void SetBool(Bool value)
		{
			this->value = ToString(
				value
			);

			attributes.Value = INIFileFieldAttributes::Bool;
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
			this->value = String::Format(
				"\"%s\"",
				HexConverter::Encode(lpBuffer, size).GetCString()
			);

			attributes.Value = INIFileFieldAttributes::Struct;
			attributes.Value |= INIFileFieldAttributes::String;
		}

		Void SetString(const String& value)
		{
			this->value = String::Format(
				"\"%s\"",
				value.GetCString()
			);

			attributes.Value = INIFileFieldAttributes::String;
		}

		template<typename T>
		Void SetInteger(T value, Bool hexadecimal = False)
		{
			if (hexadecimal)
			{
				this->value = String::Format(
					"0x%s",
					HexConverter::Encode(value).GetCString()
				);
			}
			else
			{
				this->value = ToString(
					value
				);
			}

			attributes.Value = INIFileFieldAttributes::Integer;
			attributes.Value |= INIFileFieldAttributes::Decimal;
			attributes.Set(INIFileFieldAttributes::Unsigned, Is_Unsigned<T>::Value);
			attributes.Set(INIFileFieldAttributes::Hexadecimal, hexadecimal);
		}

		template<typename T>
		Void SetDecimal(T value)
		{
			this->value = ToString(
				value
			);

			attributes.Value = INIFileFieldAttributes::Decimal;
		}
	};

	class INIFileSection
	{
		friend INIFile;

		typedef Collections::LinkedList<INIFileField> FieldContainer;

		static constexpr char REGEX_PATTERN[] = "^\\[([^\\]]*)\\]$";

		String         name;
		FieldContainer fields;

	public:
		typedef typename FieldContainer::Iterator             Iterator;
		typedef typename FieldContainer::ConstIterator        ConstIterator;

		typedef typename FieldContainer::ReverseIterator      ReverseIterator;
		typedef typename FieldContainer::ConstReverseIterator ConstReverseIterator;

		INIFileSection()
		{
		}

		explicit INIFileSection(String&& name)
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

		auto& operator [] (String&& fieldName)
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
				String("\"\"")
			);

			return *(--fields.end());
		}
		auto& operator [] (const String& fieldName)
		{
			for (auto& field : fields)
			{
				if (field.GetName().Compare(fieldName))
				{

					return field;
				}
			}

			fields.EmplaceBack(
				String(fieldName),
				String("\"\"")
			);

			return *(--fields.end());
		}
		auto& operator [] (const String& fieldName) const
		{
			const INIFileField* lpField = nullptr;

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
				"INIFileField not found"
			);

			return *lpField;
		}
	};

	class INIFile
	{
		typedef Collections::LinkedList<INIFileSection> SectionContainer;

		TextFile         file;
		SectionContainer sections;

	public:
		typedef typename SectionContainer::Iterator             Iterator;
		typedef typename SectionContainer::ConstIterator        ConstIterator;

		typedef typename SectionContainer::ReverseIterator      ReverseIterator;
		typedef typename SectionContainer::ConstReverseIterator ConstReverseIterator;

		INIFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}

		INIFile(const Path& path)
			: INIFile(
				Path(path)
			)
		{
		}

		auto& GetPath() const
		{
			return file.GetPath();
		}

		auto GetSectionCount() const
		{
			return sections.GetSize();
		}

		// @throw AL::Exception
		Bool Exists() const
		{
			if (!file.Exists())
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return False if not found
		Bool Read()
		{
			if (!Exists())
			{

				return False;
			}

			file.Open(
				FileOpenModes::Read
			);

			String line;

			Regex regexSection(
				INIFileSection::REGEX_PATTERN
			);

			Regex regexSectionField(
				INIFileField::REGEX_PATTERN
			);

			Regex::MatchCollection regexMatchCollection;

			try
			{
				while (file.ReadLine(line))
				{
					if (!regexSection.Match(regexMatchCollection, line))
					{

						continue;
					}

				read_section:
					INIFileSection section(
						Move(regexMatchCollection[1])
					);

					while (file.ReadLine(line))
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
							sections.PushBack(
								Move(section)
							);

							goto read_section;
						}
					}

					sections.PushBack(
						Move(section)
					);
				}
			}
			catch (Exception& exception)
			{
				file.Close();

				throw Exception(
					Move(exception),
					"Error reading TextFile"
				);
			}

			file.Close();

			return True;
		}

		// @throw AL::Exception
		Void Write()
		{
			file.Open(
				FileOpenModes::Read | FileOpenModes::Write | FileOpenModes::Truncate
			);

			try
			{
				for (auto& section : sections)
				{
					file.WriteLine(
						String::Format(
							"[%s]",
							section.GetName().GetCString()
						)
					);

					for (auto& field : section.fields)
					{
						file.WriteLine(
							String::Format(
								"%s = %s",
								field.GetName().GetCString(),
								field.GetValue().GetCString()
							)
						);
					}

					file.WriteLine(
						""
					);
				}
			}
			catch (Exception& exception)
			{
				file.Close();

				throw Exception(
					Move(exception),
					"Error writing TextFile"
				);
			}

			file.Close();
		}
		
		Void Clear()
		{
			sections.Clear();
		}

		// @throw AL::Exception
		Void Delete()
		{
			file.Delete();
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

		auto& operator [] (String&& sectionName)
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
		auto& operator [] (const String& sectionName)
		{
			for (auto& section : sections)
			{
				if (section.GetName().Compare(sectionName))
				{

					return section;
				}
			}

			sections.EmplaceBack(
				String(sectionName)
			);

			return *(--sections.end());
		}
		auto& operator [] (const String& sectionName) const
		{
			const INIFileSection* lpSection = nullptr;

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
				"INIFileSection not found"
			);

			return *lpSection;
		}
	};
}
