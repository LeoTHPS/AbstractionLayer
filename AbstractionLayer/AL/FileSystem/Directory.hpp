#pragma once
#include "AL/Common.hpp"

#include "Path.hpp"
#include "File.hpp"

namespace AL::FileSystem
{
	class Directory
	{
		Path path;

	public:
		Directory(Path&& path)
			: path(
				Move(path)
			)
		{
		}

		Directory(const Path& path)
			: Directory(
				Path(path)
			)
		{
		}

		auto& GetPath() const
		{
			return path;
		}


	};
}
