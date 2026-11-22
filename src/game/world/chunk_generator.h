#pragma once

#include "../../third_party/FastNoiseLite.h"

#include "types/chunk.h"
#include "world_constants.h"

#include <array>
#include <memory>

class ChunkGenerator {
public:
    // initializes the chunk generator members
    void initialize(int seed);

    // fills the chunk with blocks
    void createChunk(int c_x, int c_y, int c_z, std::unique_ptr<Chunk[]>& chunks);

    ChunkGenerator() = default;
private:
    std::array<int, CHUNK_SIZE_SQ> heights;
    int current_x = 0;
    int current_z = 0;
    bool firstChunk = true;

    int seed;
    FastNoiseLite broadNoise;
    FastNoiseLite detailNoise;
    FastNoiseLite microNoise;

    FastNoiseLite mountainNoise;
    FastNoiseLite mountainShapeNoise;

    void setNoiseParameters();
};