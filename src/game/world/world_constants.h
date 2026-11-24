#pragma once

#include "../../engine/data_types.h"
#include "types/block.h"

// chunk constants
constexpr uint32 CHUNK_SIZE = 32;
constexpr uint32 CHUNK_SIZE_SQ = CHUNK_SIZE * CHUNK_SIZE;

constexpr uint32 CHUNKS_X = 8;
constexpr uint32 CHUNKS_Y = 8;
constexpr uint32 CHUNKS_Z = 8;
constexpr uint32 CHUNKS_XY = CHUNKS_X * CHUNKS_Y;

// vertex data packing
constexpr uint32 SHIFT_POS_X = 0;
constexpr uint32 SHIFT_POS_Y = 6;
constexpr uint32 SHIFT_POS_Z = 12;
constexpr uint32 SHIFT_NORMAL = 18;
constexpr uint32 SHIFT_UV_COORD = 24;
constexpr uint32 SHIFT_BLOCK_TYPE = 26;

constexpr uint32 MASK_POS_X = 0x3Fu;
constexpr uint32 MASK_POS_Y = 0x3Fu << SHIFT_POS_Y;
constexpr uint32 MASK_POS_Z = 0x3Fu << SHIFT_POS_Z;
constexpr uint32 MASK_NORMAL = 0x3Fu << SHIFT_NORMAL;
constexpr uint32 MASK_UV_COORD = 0x3u << SHIFT_UV_COORD;
constexpr uint32 MASK_BLOCK_TYPE = 0x3Fu << SHIFT_BLOCK_TYPE;

constexpr Block BLOCK_AIR = 0;
constexpr Block BLOCK_GRASS = 1;
constexpr Block BLOCK_DIRT = 2;
constexpr Block BLOCK_STONE = 3;
constexpr Block BLOCK_SNOW = 4;