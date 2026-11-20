#pragma once

#include <array>
#include "block.h"
#include "../world_constants.hpp"

struct Chunk {
	std::array<Block, CHUNK_SIZE_SQ * CHUNK_SIZE> data;
	int x;
	int y;
	int z;
	bool empty;
};