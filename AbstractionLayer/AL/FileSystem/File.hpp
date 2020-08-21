#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"

namespace AL::FileSystem
{
	class File
	{
		Path path;

	public:
		File(Path&& path)
			: path(
				Move(path)
			)
		{
		}

		File(const Path& path)
			: File(
				Path(path)
			)
		{
		}

		bool IsOpen() const;

		// @throw AL::Exceptions::Exception
		auto GetSize() const;

		auto& GetPath() const
		{
			return path;
		}

		// @throw AL::Exceptions::Exception
		void Create();

		// @throw AL::Exceptions::Exception
		void Delete();

		// @throw AL::Exceptions::Exception
		// @return false if not found
		bool Open();
		
		void Close();

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		size_t Read(void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		// @return number of bytes written
		size_t Write(const void* lpBuffer, size_t size);
	};
}
