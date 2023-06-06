#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

#include "AL/Algorithms/FNV.hpp"

#include "AL/FileSystem/File.hpp"

#include "AL/Collections/ByteBuffer.hpp"
#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

namespace AL::Game::FileSystem
{
	template<typename T_KEY, typename T_VALUE>
	class DataFile
	{
		static_assert(
			Is_POD<T_KEY>::Value,
			"T_KEY must be POD"
		);

		static_assert(
			Is_POD<T_VALUE>::Value,
			"T_VALUE must be POD"
		);

		typedef T_KEY                                 _Key;
		typedef T_VALUE                               _Value;

		typedef Algorithms::FNV64                     _FNV64;
		typedef typename _FNV64::Hash                 _FNV64_Hash;

		typedef Collections::ByteBuffer<Endians::Big> _Buffer;

		static constexpr size_t FILE_BUFFER_CHUNK_SIZE                  = 1024;

		static constexpr size_t HEADER_SIGNATURE_SIZE                   = 4;
		static constexpr uint8  HEADER_SIGNATURE[HEADER_SIGNATURE_SIZE] =
		{
//          A     L     D     F
			0x41, 0x4C, 0x44, 0x46
		};

#pragma pack(push, 1)
		struct Header
		{
			uint8       Signature[HEADER_SIGNATURE_SIZE];

			uint32      TimeCreate;
			uint32      TimeUpdate;

			_FNV64_Hash ContentHash;
			uint64      ContentSize;

			uint32      SectionCount;
		};

		struct SectionHeader
		{
			_FNV64_Hash Hash;
		};

		struct Section
		{
			SectionHeader Header;

			T_KEY         Key;
			T_VALUE       Value;
		};
#pragma pack(pop)

		AL::FileSystem::File                  file;
		Collections::Dictionary<_Key, _Value> sections;

	public:
		typedef _Key   Key;
		typedef _Value Value;

		DataFile(DataFile&& dataFile);
		DataFile(const DataFile& dataFile);

		explicit DataFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}
		explicit DataFile(const Path& path)
			: DataFile(
				Path(path)
			)
		{
		}

		explicit DataFile(String&& path)
			: DataFile(
				Path(
					Move(path)
				)
			)
		{
		}
		explicit DataFile(const String& path)
			: DataFile(
				String(path)
			)
		{
		}

		virtual ~DataFile()
		{
		}

		Bool Contains(const Key& key) const
		{
			if (!sections.Contains(key))
			{

				return False;
			}

			return True;
		}

		Void Clear()
		{
			sections.Clear();
		}

		Void Add(Key&& key, Value&& value)
		{
			sections.Add(
				Move(key),
				Move(value)
			);
		}
		Void Add(const Key& key, Value&& value)
		{
			sections.Add(
				key,
				Move(value)
			);
		}
		Void Add(const Key& key, const Value& value)
		{
			sections.Add(
				key,
				value
			);
		}

		// TODO: implement
		// @throw AL::Exception
		// @return AL::False if not found
		Bool Read();

		// TODO: implement
		// @throw AL::Exception
		Void Write();

		Value&       operator [] (Key&& key)
		{
			return sections[Move(key)];
		}
		Value&       operator [] (const Key& key)
		{
			return sections[key];
		}
		const Value& operator [] (const Key& key) const
		{
			return sections[key];
		}

		DataFile& operator = (DataFile&& dataFile)
		{
			file = Move(
				dataFile.file
			);

			sections = Move(
				dataFile.sections
			);

			return *this;
		}
		DataFile& operator = (const DataFile& dataFile)
		{
			file     = dataFile.file;
			sections = dataFile.sections;

			return *this;
		}

		Bool operator == (const DataFile& dataFile) const;
		Bool operator != (const DataFile& dataFile) const
		{
			if (operator==(dataFile))
			{

				return False;
			}

			return True;
		}
	};
}
