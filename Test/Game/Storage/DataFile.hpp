#pragma once
#include "AL/Common.hpp"

#include "AL/OS/Console.hpp"

#include "AL/Game/Storage/DataFile.hpp"

static void AL_Game_Storage_DataFile()
{
	using namespace AL;
	using namespace AL::Game::Storage;

	DataFile<int, int> file(
		"test"
	);

	file.OpenNew();

	file[0] = 1;

	file.Close();
}
