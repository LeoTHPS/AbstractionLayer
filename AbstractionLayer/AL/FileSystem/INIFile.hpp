#pragma once
#include "AL/Common.hpp"

#include "TextFile.hpp"

#include "AL/Collections/Map.hpp"
#include "AL/Collections/List.hpp"
#include "AL/Collections/Array.hpp"

namespace AL::FileSystem
{
	enum class INIFileFieldAttributes
	{
		Struct      = 0x1,
		String      = 0x2,
		Integer     = 0x4,
		Decimal     = 0x8,
		Unsigned    = 0x10
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(INIFileFieldAttributes);

	class INIFile;

	class INIFileField
	{
		friend INIFile;

		static constexpr char REGEX_PATTERN[] = "^([^ ]*) = (.*)$";

		static constexpr char REGEX_PATTERN_DATA_TYPE_STRUCT[] = "^\"(([0-9A-Fa-f]{2})+)\"$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_STRING[] = "^\"([^\"]*)\"$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_INTEGER[] = "^(-?\\d+)$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_DECIMAL[] = "^(-?\\d+(\\.\\d+)?)$";
		static constexpr char REGEX_PATTERN_DATA_TYPE_UNSIGNED[] = "^(-{0}\\d+(\\.\\d+)?)$";

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
			if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_STRUCT, this->value))
			{
				attributes.Mask = INIFileFieldAttributes::Struct;
				attributes.Mask |= INIFileFieldAttributes::String;
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_STRING, this->value))
			{
				attributes.Mask = INIFileFieldAttributes::String;
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_INTEGER, this->value))
			{
				attributes.Mask = INIFileFieldAttributes::Integer;
				attributes.Mask |= INIFileFieldAttributes::Decimal;

				attributes.Set(
					INIFileFieldAttributes::Unsigned,
					Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
			else if (Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_DECIMAL, this->value))
			{
				attributes.Mask = INIFileFieldAttributes::Decimal;

				attributes.Set(
					INIFileFieldAttributes::Unsigned,
					Regex::IsMatch(REGEX_PATTERN_DATA_TYPE_UNSIGNED, this->value)
				);
			}
		}

		bool IsStruct() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::Struct
			);
		}

		bool IsString() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::String
			);
		}

		bool IsInteger() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::Integer
			);
		}

		bool IsDecimal() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::Decimal
			);
		}

		bool IsUnsigned() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::Unsigned
			);
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
			HexConverter::Decode(buffer, GetString());

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
				GetValue().GetLength() - 1
			);
		}

		template<typename T>
		auto GetInteger() const
		{
			AL_ASSERT(
				IsInteger(),
				"INIFileField type is not integer"
			);

			return Integer<T>::FromString(
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

			if constexpr (Is_Float<T>::Value)
			{
				return Float<T>::FromString(
					GetValue()
				);
			}
			else if constexpr (Is_Double<T>::Value)
			{
				return Double<T>::FromString(
					GetValue()
				);
			}

			throw Exceptions::NotImplementedException();
		}

		template<typename T>
		void SetStruct(const T& value)
		{
			SetStructAsBuffer(
				&value,
				sizeof(T)
			);
		}

		void SetStructAsBuffer(const void* lpBuffer, size_t size)
		{
			this->value = String::Format(
				"\"%s\"",
				HexConverter::Encode(lpBuffer, size).GetCString()
			);

			attributes.Mask = INIFileFieldAttributes::Struct;
			attributes.Mask |= INIFileFieldAttributes::String;
		}

		void SetString(const String& value)
		{
			this->value = String::Format(
				"\"%s\"",
				value.GetCString()
			);

			attributes.Mask = INIFileFieldAttributes::String;
		}

		template<typename T>
		void SetInteger(T value)
		{
			this->value = Integer<T>::ToString(
				value
			);

			attributes.Mask = INIFileFieldAttributes::Integer;
			attributes.Mask |= INIFileFieldAttributes::Decimal;
			attributes.Set(INIFileFieldAttributes::Unsigned, Is_Unsigned<T>::Value);
		}

		template<typename T>
		void SetDecimal(T value)
		{
			if constexpr (Is_Float<T>::Value)
			{
				this->value = Float<T>::ToString(
					value
				);
			}
			else if constexpr (Is_Double<T>::Value)
			{
				this->value = Double<T>::ToString(
					value
				);
			}
			else
			{

				throw Exceptions::NotImplementedException();
			}

			attributes.Mask = INIFileFieldAttributes::Decimal;
		}
	};

	class INIFileSection
	{
		friend INIFile;

		static constexpr char REGEX_PATTERN[] = "^\\[([^\\]]*)\\]$";

		String name;

		Collections::List<INIFileField> fields;

	public:
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

		void Clear()
		{
			fields.Clear();
		}

		auto begin()
		{
			return fields.begin();
		}
		auto cbegin() const
		{
			return fields.cbegin();
		}

		auto end()
		{
			return fields.end();
		}
		auto cend() const
		{
			return fields.cend();
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

			return *fields.GetBack();
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

			return *fields.GetBack();
		}
		auto& operator [] (const String& fieldName) const
		{
			for (auto& field : fields)
			{
				if (field.GetName().Compare(fieldName))
				{

					return field;
				}
			}

			AL_ASSERT(
				false,
				"INIFileField not found"
			);
		}
	};

	class INIFile
	{
		TextFile file;

		Collections::List<INIFileSection> sections;

	public:
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

		// @throw AL::Exceptions::Exception
		bool Exists() const
		{
			return file.Exists();
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Read()
		{
			if (!Exists())
			{

				return false;
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
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error reading TextFile"
				);
			}

			file.Close();

			return true;
		}

		// @throw AL::Exceptions::Exception
		void Write()
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
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error writing TextFile"
				);
			}

			file.Close();
		}
		
		void Clear()
		{
			sections.Clear();
		}

		// @throw AL::Exceptions::Exception
		void Delete()
		{
			file.Delete();
		}

		auto begin()
		{
			return sections.begin();
		}
		auto cbegin() const
		{
			return sections.cbegin();
		}

		auto end()
		{
			return sections.end();
		}
		auto cend() const
		{
			return sections.cend();
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

			return *sections.GetBack();
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

			return *sections.GetBack();
		}
		auto& operator [] (const String& sectionName) const
		{
			for (auto& section : sections)
			{
				if (section.GetName().Compare(sectionName))
				{

					return section;
				}
			}

			AL_ASSERT(
				false,
				"INIFileSection not found"
			);
		}
	};
}
