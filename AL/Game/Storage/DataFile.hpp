#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

#include "AL/Algorithms/FNV.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Dictionary.hpp"

#include "AL/FileSystem/File.hpp"

#include "AL/OS/System.hpp"

namespace AL::Game::Storage
{
	template<typename T_SECTION_ID, typename T_SECTION_DATA>
	class DataFile
	{
		typedef Algorithms::FNV64 FNV64;

#pragma pack(push, 1)
		struct FileHeader
		{
			static constexpr size_t     MAGIC_SIZE        = 4;
			static constexpr const char MAGIC[MAGIC_SIZE] = { 'A', 'L', 'D', 'F' };

			char   Magic[MAGIC_SIZE];

			uint64 Size;
			uint64 Timestamp;
			uint32 SectionCount;
		};

		struct FileIndexEntry
		{
			T_SECTION_ID Id;
			uint64       Offset;
		};

		struct FileSectionHeader
		{
			T_SECTION_ID Id;
			FNV64        Hash;
		};

		struct FileSection
		{
			FileSectionHeader Header;
			T_SECTION_DATA    Data;
		};
#pragma pack(pop)

		Bool                                                  isOpen = False;

		FileSystem::File                                      file;
		Collections::Dictionary<T_SECTION_ID, T_SECTION_DATA> sections;

		DataFile(const DataFile&) = delete;

	public:
		typedef T_SECTION_ID   SectionId;
		typedef T_SECTION_DATA SectionData;

		DataFile(DataFile&& dataFile)
			: isOpen(
				dataFile.isOpen
			),
			file(
				Move(dataFile.file)
			),
			sections(
				Move(dataFile.sections)
			)
		{
			dataFile.isOpen = False;
		}

		explicit DataFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}
		explicit DataFile(const Path& path)
			: file(
				path
			)
		{
		}

		explicit DataFile(String&& path)
			: file(
				Move(path)
			)
		{
		}
		explicit DataFile(const String& path)
			: file(
				path
			)
		{
		}

		virtual ~DataFile()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool Contains(const SectionId& id) const
		{
			if (!IsOpen())
			{

				return False;
			}

			if (!sections.Contains(id))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return False if not found
		Bool Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"DataFile already open"
			);

			if (!file.Open(FileSystem::FileOpenModes::Binary | FileSystem::FileOpenModes::Read))
			{

				return False;
			}

			FileHeader header;
			size_t     bytesRead;

			if ((bytesRead = file.Read(&header, sizeof(FileHeader))) != sizeof(FileHeader))
			{
				file.Close();

				throw Exception(
					"Unexpected end of file"
				);
			}

			if ((header.Magic[0] != FileHeader::MAGIC[0]) ||
				(header.Magic[1] != FileHeader::MAGIC[1]) ||
				(header.Magic[2] != FileHeader::MAGIC[2]) ||
				(header.Magic[3] != FileHeader::MAGIC[3]))
			{
				file.Close();

				throw Exception(
					"Invalid signature"
				);
			}

			header.Size = FromEndian(
				header.Size
			);
			header.Timestamp = FromEndian(
				header.Timestamp
			);
			header.SectionCount = FromEndian(
				header.SectionCount
			);

			uint64 fileSize;

			try
			{
				fileSize = file.GetSize();
			}
			catch (Exception&)
			{
				file.Close();

				throw;
			}

			if (fileSize != header.Size)
			{
				file.Close();

				throw Exception(
					"Invalid file size"
				);
			}

			Collections::Array<uint8> buffer(
				fileSize
			);

			if ((bytesRead = file.Read(&buffer[sizeof(FileHeader)], fileSize - sizeof(FileHeader))) != (fileSize - sizeof(FileHeader)))
			{
				file.Close();

				throw Exception(
					"Unexpected end of file"
				);
			}

			{
				auto lpFileIndex = reinterpret_cast<FileIndexEntry*>(
					&buffer[sizeof(FileHeader)]
				);

				for (uint32 i = 0; i < header.SectionCount; ++i, ++lpFileIndex)
				{
					if constexpr (Is_Enum_Or_Integer<SectionId>::Value || Is_Decimal<SectionId>::Value)
					{
						lpFileIndex->Id = FromEndian(
							lpFileIndex->Id
						);
					}

					lpFileIndex->Offset = FromEndian(
						lpFileIndex->Offset
					);

					auto lpFileSection = reinterpret_cast<FileSection*>(
						&buffer[lpFileIndex->Offset]
					);

					if constexpr (Is_Enum_Or_Integer<SectionId>::Value || Is_Decimal<SectionId>::Value)
					{
						lpFileSection->Header.Id = FromEndian(
							lpFileSection->Header.Id
						);
					}

					lpFileSection->Header.Hash = FromEndian(
						lpFileSection->Header.Hash
					);

					if (lpFileSection->Header.Hash != FNV64::Calculate(&lpFileSection->Data, sizeof(SectionData)))
					{
						file.Close();

						sections.Clear();

						throw Exception(
							"Invalid section hash [Index: %lu, Offset: %llu]",
							i,
							lpFileIndex->Offset
						);
					}

					sections.Add(
						Move(lpFileSection->Header.Id),
						Move(lpFileSection->Data)
					);
				}
			}

			file.Close();

			isOpen = True;

			return True;
		}

		Void OpenNew()
		{
			AL_ASSERT(
				!IsOpen(),
				"DataFile already open"
			);

			isOpen = True;
		}

		// @throw AL::Exception
		Void Save()
		{
			AL_ASSERT(
				IsOpen(),
				"DataFile not open"
			);

			file.Open(
				FileSystem::FileOpenModes::Binary | FileSystem::FileOpenModes::Write | FileSystem::FileOpenModes::Truncate
			);

			auto sectionCount = sections.GetSize();

			Collections::Array<uint8> buffer(
				sizeof(FileHeader) + (sectionCount * sizeof(FileIndexEntry)) + (sectionCount * sizeof(FileSection))
			);

			{
				auto lpHeader = reinterpret_cast<FileHeader*>(
					&buffer[0]
				);

				lpHeader->Magic[0]     = FileHeader::MAGIC[0];
				lpHeader->Magic[1]     = FileHeader::MAGIC[1];
				lpHeader->Magic[2]     = FileHeader::MAGIC[2];
				lpHeader->Magic[3]     = FileHeader::MAGIC[3];
				lpHeader->Size         = ToEndian(
					buffer.GetCapacity()
				);
				lpHeader->Timestamp    = ToEndian(
					OS::System::GetTimestamp().ToSeconds()
				);
				lpHeader->SectionCount = ToEndian(
					static_cast<uint32>(sectionCount)
				);
			}

			{
				auto lpFileIndex = reinterpret_cast<FileIndexEntry*>(
					&buffer[sizeof(FileHeader)]
				);

				auto lpFileSection = reinterpret_cast<FileSection*>(
					&buffer[sizeof(FileHeader) + (sectionCount * sizeof(FileIndexEntry))]
				);

				for (auto it = sections.begin(); it != sections.end(); ++it, ++lpFileIndex, ++lpFileSection)
				{
					if constexpr (Is_Enum_Or_Integer<SectionId>::Value || Is_Decimal<SectionId>::Value)
					{
						lpFileIndex->Id = ToEndian(
							it->Key
						);
					}
					else
					{
						lpFileIndex->Id = it->Key;
					}

					lpFileIndex->Offset = ToEndian(
						static_cast<uint64>(
							reinterpret_cast<const uint8*>(lpFileSection) - &buffer[0]
						)
					);

					if constexpr (Is_Enum_Or_Integer<SectionId>::Value || Is_Decimal<SectionId>::Value)
					{
						lpFileSection->Header.Id = ToEndian(
							it->Key
						);
					}
					else
					{
						lpFileSection->Header.Id = it->Key;
					}

					lpFileSection->Header.Hash = ToEndian(
						FNV64::Calculate(
							&it->Value,
							sizeof(SectionData)
						)
					);

					lpFileSection->Data = it->Value;
				}
			}

			try
			{
				file.Write(
					&buffer[0],
					buffer.GetCapacity()
				);
			}
			catch (Exception&)
			{
				file.Close();

				throw;
			}

			file.Close();
		}

		Void Close()
		{
			if (IsOpen())
			{
				sections.Clear();

				isOpen = False;
			}
		}

		SectionData& operator [] (SectionId&& id)
		{
			return sections[Move(id)];
		}
		SectionData& operator [] (const SectionId& id)
		{
			return sections[id];
		}
		const SectionData& operator [] (const SectionId& id) const
		{
			return sections[id];
		}

		DataFile& operator = (DataFile&& dataFile)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = dataFile.isOpen;
			dataFile.isOpen = False;

			file = Move(
				dataFile
			);

			sections = Move(
				dataFile.sections
			);

			return *this;
		}

		Bool operator == (const DataFile& dataFile) const
		{
			if (sections != dataFile.sections)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const DataFile& dataFile) const
		{
			if (operator==(dataFile))
			{

				return False;
			}

			return True;
		}

	private:
		template<typename T>
		static constexpr T ToEndian(T value)
		{
			return BitConverter::ToBigEndian(
				value
			);
		}

		template<typename T>
		static constexpr T FromEndian(T value)
		{
			return BitConverter::FromBigEndian(
				value
			);
		}
	};
}
