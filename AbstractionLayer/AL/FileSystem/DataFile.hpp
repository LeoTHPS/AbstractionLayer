#pragma once
#include "AL/Common.hpp"

#include "AL/Algorithms/FNV.hpp"

#include "AL/Collections/Map.hpp"
#include "AL/Collections/Array.hpp"

#include "AL/FileSystem/BinaryFile.hpp"

namespace AL::FileSystem
{
	typedef uint64 DataFileSectionID;
	typedef Collections::Array<uint8> DataFileSectionBuffer;

	class DataFile
	{
		typedef uint32 HeaderSignature;

		static constexpr HeaderSignature HEADER_SIGNATURE = 0x12345678;

#pragma pack(push, 1)
		struct Header
		{
			HeaderSignature Signature;
			uint64          SectionCount;
		};

		struct SectionHeader
		{
			DataFileSectionID       ID;
			Algorithms::FNV64::Hash BufferHash;
			uint64                  BufferSize;
		};
#pragma pack(pop)

		BinaryFile file;

		Collections::Map<DataFileSectionID, DataFileSectionBuffer> sections;

		DataFile(const DataFile&) = delete;

	public:
		DataFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}

		DataFile(const Path& path)
			: DataFile(
				Path(path)
			)
		{
		}

		DataFile(DataFile&& file)
			: file(
				Move(file.file)
			),
			sections(
				Move(sections)
			)
		{
		}

		virtual ~DataFile()
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
			if (!file.Open(FileOpenModes::Read | FileOpenModes::Binary))
			{

				return false;
			}

			Header header;

			try
			{
				ReadHeader(
					file,
					header
				);
			}
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error reading DataFile::Header"
				);
			}

			DataFileSectionID sectionId;
			DataFileSectionBuffer sectionBuffer;

			try
			{
				for (decltype(Header::SectionCount) i = 0; i < header.SectionCount; ++i)
				{
					ReadSection(
						file,
						sectionId,
						sectionBuffer
					);

					sections.Add(
						sectionId,
						Move(sectionBuffer)
					);
				}
			}
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error reading DataFile::SectionHeader"
				);
			}

			file.Close();

			return true;
		}

		// @throw AL::Exceptions::Exception
		void Write()
		{
			file.Open(
				FileOpenModes::Write | FileOpenModes::Binary | FileOpenModes::Truncate
			);

			Header header;
			header.Signature = HEADER_SIGNATURE;
			header.SectionCount = sections.GetSize();
			
			try
			{
				WriteHeader(
					file,
					header
				);
			}
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error writing DataFile::Header"
				);
			}

			try
			{
				for (auto& pair : sections)
				{
					WriteSection(
						file,
						pair.first,
						pair.second
					);
				}
			}
			catch (Exceptions::Exception& exception)
			{
				file.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error writing DataFile::SectionHeader"
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

		bool Find(DataFileSectionID sectionId, DataFileSectionBuffer*& lpBuffer)
		{
			auto it = sections.Find(
				sectionId
			);

			if (it == sections.end())
			{

				return false;
			}

			lpBuffer = &it->second;

			return true;
		}
		bool Find(DataFileSectionID sectionId, const DataFileSectionBuffer*& lpBuffer) const
		{
			auto it = sections.Find(
				sectionId
			);

			if (it == sections.end())
			{

				return false;
			}

			lpBuffer = &it->second;

			return true;
		}

		bool Contains(DataFileSectionID sectionId) const
		{
			return sections.Contains(
				sectionId
			);
		}

		// @return false if section already exists and overwrite == false
		bool Add(DataFileSectionID sectionId, DataFileSectionBuffer&& buffer, bool overwrite = false)
		{
			if (overwrite)
			{
				sections[sectionId] = Move(
					buffer
				);

				return true;
			}
			else if (!sections.Contains(sectionId))
			{
				sections.Add(
					sectionId,
					Move(buffer)
				);

				return true;
			}

			return false;
		}
		// @return false if section already exists and overwrite == false
		bool Add(DataFileSectionID sectionId, const DataFileSectionBuffer& buffer, bool overwrite = false)
		{
			DataFileSectionBuffer sectionBuffer(
				buffer
			);

			return Add(
				sectionId,
				Move(sectionBuffer),
				overwrite
			);
		}

		void Remove(DataFileSectionID sectionId)
		{
			sections.Remove(
				sectionId
			);
		}

		auto& operator = (DataFile&& file)
		{
			this->file = Move(
				file.file
			);

			sections = Move(
				file.sections
			);

			return *this;
		}

	private:
		// @throw AL::Exceptions::Exception
		static void ReadHeader(BinaryFile& file, Header& header)
		{
			if (!file.ReadUInt32(header.Signature))
			{

				throw Exceptions::Exception(
					"Error reading DataFile::Header::Signature"
				);
			}

			if (header.Signature != HEADER_SIGNATURE)
			{

				throw Exceptions::Exception(
					"Unexpected DataFile::Header::Signature value"
				);
			}

			if (!file.ReadUInt64(header.SectionCount))
			{

				throw Exceptions::Exception(
					"Error reading DataFile::Header::SectionCount"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		static void WriteHeader(BinaryFile& file, const Header& header)
		{
			file.WriteUInt32(header.Signature);
			file.WriteUInt64(header.SectionCount);
		}

		// @throw AL::Exceptions::Exception
		static void ReadSection(BinaryFile& file, DataFileSectionID& sectionId, DataFileSectionBuffer& sectionBuffer)
		{
			SectionHeader sectionHeader;

			if (!file.ReadUInt64(sectionHeader.ID))
			{

				throw Exceptions::Exception(
					"Error reading DataFile::SectionHeader::ID"
				);
			}

			sectionId = sectionHeader.ID;

			if (!file.ReadUInt64(sectionHeader.BufferSize))
			{

				throw Exceptions::Exception(
					"Error reading DataFile::SectionHeader::ID"
				);
			}

			if (!file.ReadUInt64(sectionHeader.BufferHash))
			{

				throw Exceptions::Exception(
					"Error reading DataFile::SectionHeader::BufferHash"
				);
			}

			sectionBuffer.SetSize(
				static_cast<size_t>(sectionHeader.BufferSize)
			);

			if (!file.Read(&sectionBuffer[0], sectionBuffer.GetCapacity()))
			{

				throw Exceptions::Exception(
					"Error reading DataFileSectionBuffer"
				);
			}

			if (sectionHeader.BufferHash != Algorithms::FNV64::Calculate(&sectionBuffer[0], sectionBuffer.GetSize()))
			{

				throw Exceptions::Exception(
					"Unexpected DataFile::SectionHeader::BufferHash value"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		static void WriteSection(BinaryFile& file, DataFileSectionID sectionId, const DataFileSectionBuffer& sectionBuffer)
		{
			SectionHeader sectionHeader;
			sectionHeader.ID = sectionId;
			sectionHeader.BufferSize = sectionBuffer.GetSize();
			sectionHeader.BufferHash = Algorithms::FNV64::Calculate(
				&sectionBuffer[0],
				sectionBuffer.GetSize()
			);

			file.WriteUInt64(sectionHeader.ID);
			file.WriteUInt64(sectionHeader.BufferHash);
			file.WriteUInt64(sectionHeader.BufferSize);
			file.Write(&sectionBuffer[0], sectionBuffer.GetSize());
		}
	};
}
