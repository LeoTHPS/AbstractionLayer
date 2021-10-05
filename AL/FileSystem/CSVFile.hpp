#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"
#include "TextFile.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::FileSystem
{
	class CSVFile
	{
		TextFile file;

		CSVFile(const CSVFile&) = delete;

	public:
		CSVFile(CSVFile&& csvFile)
			: file(
				Move(csvFile.file)
			)
		{
		}

		explicit CSVFile(Path&& path)
			: file(
				Move(path)
			)
		{
		}
		explicit CSVFile(const Path& path)
			: file(
				path
			)
		{
		}

		explicit CSVFile(String&& path)
			: file(
				path
			)
		{
		}
		explicit CSVFile(const String& path)
			: file(
				path
			)
		{
		}

		virtual ~CSVFile()
		{
			if (IsOpen())
			{
				Close();
			}
		}

		Bool IsOpen() const
		{
			return file.IsOpen();
		}

		auto& GetPath() const
		{
			return file.GetPath();
		}

		auto GetLineEnding() const
		{
			return file.GetLineEnding();
		}

		void SetLineEnding(TextFileLineEndings value)
		{
			file.SetLineEnding(
				value
			);
		}

		// @throw AL::Exception
		// @return False if not found
		Bool Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"CSVFile already open"
			);

			if (!file.Open(FileOpenModes::Read | FileOpenModes::Write | FileOpenModes::Append))
			{

				return False;
			}

			return True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				file.Close();
			}
		}

		// @throw AL::Exception
		// @return False on EOL
		Bool ReadLine(Collections::Array<String>& line)
		{
			AL_ASSERT(
				IsOpen(),
				"CSVFile not open"
			);

			String _line;

			if (!file.ReadLine(_line))
			{

				return False;
			}

			line = _line.Split(
				','
			);

			for (auto& chunk : line)
			{
				if (chunk.StartsWith('"') && chunk.EndsWith('"'))
				{
					chunk = chunk.SubString(
						1,
						chunk.GetLength() - 2
					);
				}
			}

			return True;
		}

		// @throw AL::Exception
		template<size_t S>
		Void WriteLine(const String(&line)[S])
		{
			WriteLine(
				&line[0],
				S
			);
		}
		// @throw AL::Exception
		Void WriteLine(const String* lpLine, size_t count)
		{
			for (size_t i = 0; i < count; ++i, ++lpLine)
			{
				if (i > 0)
				{
					file.Write(
						','
					);
				}

				auto line = *lpLine;

				Bool containsSpace = line.Contains(
					' '
				);

				if (containsSpace)
				{
					file.Write(
						'"'
					);
				}

				file.Write(
					line
				);

				if (containsSpace)
				{
					file.Write(
						'"'
					);
				}
			}

			file.WriteLine(
				""
			);
		}
		// @throw AL::Exception
		Void WriteLine(const Collections::Array<String>& line)
		{
			WriteLine(
				&line[0],
				line.GetSize()
			);
		}

		CSVFile& operator = (CSVFile&& csvFile)
		{
			file = Move(
				csvFile.file
			);

			return *this;
		}

		Bool operator == (const CSVFile& csvFile) const;
		Bool operator != (const CSVFile& csvFile) const
		{
			if (operator==(csvFile))
			{

				return False;
			}

			return True;
		}
	};
}
