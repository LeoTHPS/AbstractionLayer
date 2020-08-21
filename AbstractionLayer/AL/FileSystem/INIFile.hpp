#pragma once
#include "AL/Common.hpp"

#include "TextFile.hpp"

#include "AL/Collections/Map.hpp"
#include "AL/Collections/List.hpp"

namespace AL::FileSystem
{
	enum class INIFileFieldAttributes
	{
		Struct      = 0x1,
		String      = 0x2,
		Integer     = 0x4,
		Decimal     = 0x8,
		Unsigned    = 0x10,
		Hexadecimal = 0x20
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(INIFileFieldAttributes);

	class INIFileField
	{
		BitMask<INIFileFieldAttributes> attributes;

	public:
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

		bool IsHexadecimal() const
		{
			return attributes.IsSet(
				INIFileFieldAttributes::Hexadecimal
			);
		}

		auto GetAttributes() const
		{
			return attributes;
		}


	};

	class INIFileSection
	{
	public:
		auto GetFieldCount() const;

		auto& operator [] (const String& field);
		auto& operator [] (const String& field) const;
	};

	class INIFile
	{
		TextFile file;

		Collections::List<INIFileSection> sections;
		Collections::Map<String, INIFileSection*> sectionsByName;

	public:
		INIFile(Path&& path)
			: file(
				Move(file)
			)
		{
		}

		INIFile(const Path& path)
			: INIFile(
				Path(path)
			)
		{
		}

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Open();

		void Close();

		auto& operator [] (const String& section);
		auto& operator [] (const String& section) const;
	};
}
