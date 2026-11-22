#pragma once

#include <vector>

#include "../../engine/data_types.h"
#include "world_constants.h"
#include "types/vertex.h"
#include "types/block.h"

inline uint32 packValue(
    uint32 x, uint32 y, uint32 z,
    uint32 normal,
    uint32 uv,
    uint32 blockType
    ) {
    return
        x << SHIFT_POS_X |
        y << SHIFT_POS_Y |
        z << SHIFT_POS_Z |
        normal << SHIFT_NORMAL |
        uv << SHIFT_UV_COORD |
        blockType << SHIFT_BLOCK_TYPE;
}

inline void addFrontFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices,
    int x, int y, int z,
    uint32 indexOffset,
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x, y, z + 1,
            0b110000u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y, z + 1,
            0b110000u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z + 1,
            0b110000u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y + 1, z + 1,
            0b110000u,
            0b10u,
            block
        )
    );

    indices.push_back(0 + indexOffset);
    indices.push_back(1 + indexOffset);
    indices.push_back(2 + indexOffset);

    indices.push_back(2 + indexOffset);
    indices.push_back(3 + indexOffset);
    indices.push_back(0 + indexOffset);
}

inline void addBackFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices,
    int x, int y, int z,
    uint32 indexOffset,
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x + 1, y, z,
            0b010000u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y, z,
            0b010000u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y + 1, z,
            0b010000u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z,
            0b010000u,
            0b10u,
            block
        )
    );

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indices.push_back(indexOffset + 0);
}

inline void addLeftFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices,
    int x, int y, int z,
    uint32 indexOffset,
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x, y, z,
            0b000001u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y, z + 1,
            0b000001u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y + 1, z + 1,
            0b000001u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y + 1, z,
            0b000001u,
            0b10u,
            block
        )
    );

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indices.push_back(indexOffset + 0);
}

inline void addRightFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices,
    int x, int y, int z,
    uint32 indexOffset,
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x + 1, y, z + 1,
            0b000011u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y, z,
            0b000011u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z,
            0b000011u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z + 1,
            0b000011u,
            0b10u,
            block
        )
    );

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indices.push_back(indexOffset + 0);
}

inline void addTopFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices,
    int x, int y, int z,
    uint32 indexOffset,
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x, y + 1, z + 1,
            0b001100u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z + 1,
            0b001100u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y + 1, z,
            0b001100u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y + 1, z,
            0b001100u,
            0b10u,
            block
        )
    );

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indices.push_back(indexOffset + 0);
}

inline void addBottomFace(
    std::vector<Vertex>& vertices, std::vector<uint32>& indices, 
    int x, int y, int z, 
    uint32 indexOffset, 
    Block block
    ) {
    vertices.emplace_back(
        packValue(
            x, y, z,
            0b000100u,
            0b00u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y, z,
            0b000100u,
            0b01u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x + 1, y, z + 1,
            0b000100u,
            0b11u,
            block
        )
    );

    vertices.emplace_back(
        packValue(
            x, y, z + 1,
            0b000100u,
            0b10u,
            block
        )
    );

    indices.push_back(indexOffset + 0);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);

    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indices.push_back(indexOffset + 0);
}