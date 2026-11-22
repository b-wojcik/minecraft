#include "chunk_generator.h"

#include <algorithm>

void ChunkGenerator::initialize(int seed) {
    ChunkGenerator::seed = seed;
    setNoiseParameters();
}

void ChunkGenerator::createChunk(int c_x, int c_y, int c_z, std::unique_ptr<Chunk[]>& chunks) {
    if (firstChunk) {
        current_x = c_x;
        current_z = c_z;
    }

    const int MAX_HEIGHT = int(CHUNKS_Y * CHUNK_SIZE - 1);
    const int HEIGHT_INVALID = 0x3f3f3f3f;

    if (firstChunk || c_x != current_x || c_z != current_z) {
        current_x = c_x;
        current_z = c_z;

        for (int i = 0; i < CHUNK_SIZE_SQ; i++) {
            heights[i] = HEIGHT_INVALID;
        }

        for (int z = 0; z < CHUNK_SIZE; z++) {
            int indexOffset = z * CHUNK_SIZE;
            int worldZ = c_z * CHUNK_SIZE + z;

            for (int x = 0; x < CHUNK_SIZE; x++) {
                const int heightIndex = indexOffset + x;

                int worldX = c_x * CHUNK_SIZE + x;

                float xPos = float(worldX);
                float zPos = float(worldZ);

                // normalize noise from [-1 , 1] to [0 , 1]
                float broad = broadNoise.GetNoise(xPos, zPos) * 0.5f + 0.5f;
                float detail = detailNoise.GetNoise(xPos, zPos) * 0.5f + 0.5f;
                float micro = microNoise.GetNoise(xPos, zPos) * 0.5f + 0.5f;

                // determine which regions become mountains
                float mountainWeight = mountainNoise.GetNoise(xPos, zPos) * 0.5f + 0.5f;
                const float mountainThreshold = 0.37f;

                float mountainMask = std::clamp(
                    (mountainWeight - mountainThreshold) / (0.75f - mountainThreshold),
                    0.0f,
                    1.0f
                );

                // apply an easing function
                mountainMask =
                    mountainMask * mountainMask *
                    (3.0f - 2.0f * mountainMask);

                // TODO: expose those magic numbers as parameters in the future
                // maybe chunks size dependence for height constants?

                // mountain height
                float mountainHeight = 0.0f;

                if (mountainMask > 0.0f) {
                    float mountain = mountainShapeNoise.GetNoise(xPos, zPos) * 0.5f + 0.5f;

                    mountainHeight =
                        70.0f +
                        std::pow(mountain, 1.05f) * 125.0f +
                        (detail - 0.5f) * 14.0f +
                        (micro - 0.5f) * 4.0f;
                }

                // plains height
                float plainsHeight =
                    64.0f +
                    (broad - 0.5f) * 18.0f +
                    (detail - 0.5f) * 5.0f +
                    (micro - 0.5f) * 2.0f;

                // blend plains and mountains
                float heightValue =
                    plainsHeight * (1.0f - mountainMask) +
                    mountainHeight * mountainMask;

                // final height
                int height = int(heightValue);
                height = std::clamp(height, 1, MAX_HEIGHT);
                heights[heightIndex] = height;
            }
        }
    }

    firstChunk = false;

    const int chunkIndex = c_z * CHUNKS_X * CHUNKS_Y + c_y * CHUNKS_X + c_x;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        int indexOffset = z * CHUNK_SIZE;

        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int x = 0; x < CHUNK_SIZE; x++) {
                Block& block = chunks[chunkIndex].data[
                    z * CHUNK_SIZE_SQ + y * CHUNK_SIZE + x
                ];

                const int heightIndex = indexOffset + x;
                int height = heights[heightIndex];

                int worldY = c_y * CHUNK_SIZE + y;

                // determine the block by height
                if (worldY < height) {
                    block = worldY < 72 && worldY > 60 ? BLOCK_DIRT : BLOCK_STONE;
                    chunks[chunkIndex].empty = false;
                }
                else if (worldY == height - 1) {
                    block = height <= 120 ? BLOCK_DIRT : BLOCK_SNOW;
                    chunks[chunkIndex].empty = false;
                }
                else if (worldY == height) {
                    block = height <= 120 ? BLOCK_GRASS : BLOCK_SNOW;
                    chunks[chunkIndex].empty = false;
                }
                else {
                    block = BLOCK_AIR;
                }
            }
    }
}

void ChunkGenerator::setNoiseParameters() {
    // https://auburn.github.io/FastNoiseLite/

    broadNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    broadNoise.SetSeed(seed);
    broadNoise.SetFrequency(0.0035f);
    broadNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    broadNoise.SetFractalOctaves(4);

    detailNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    detailNoise.SetSeed(seed);
    detailNoise.SetFrequency(0.014f);
    detailNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    detailNoise.SetFractalOctaves(3);

    microNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    microNoise.SetSeed(seed);
    microNoise.SetFrequency(0.04f);
    microNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    microNoise.SetFractalOctaves(2);

    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainNoise.SetSeed(seed);
    mountainNoise.SetFrequency(0.005f);

    mountainShapeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainShapeNoise.SetSeed(seed);
    mountainShapeNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    mountainShapeNoise.SetFrequency(0.0045f);
    mountainShapeNoise.SetFractalOctaves(4);
}