#include "world.h"

#include <cstdlib>
#include <random>

void World::create(FPCamera& camera) {
    std::cout << "~~~~~~~~~~~~~~~~~~\n";
    std::cout << "CHUNK_SIZE = " << CHUNK_SIZE << '\n' << '\n';
    std::cout << "CHUNKS_X   = " << CHUNKS_X << '\n';
    std::cout << "CHUNKS_Y   = " << CHUNKS_Y << '\n';
    std::cout << "CHUNKS_Z   = " << CHUNKS_Z << '\n';
    std::cout << "~~~~~~~~~~~~~~~~~~\n\n";

    float t = Window::get().time();
    std::cout << "CREATE CHUNKS BEGIN\n";

    // TODO: !!! not cache friendly, consider changing indexing order for chunks
    // it could make some sense for rendering too, because fully covered chunks do not need to be drawn

    for (int c_z = 0; c_z < CHUNKS_Z; c_z++)
    for (int c_x = 0; c_x < CHUNKS_X; c_x++)
    for (int c_y = 0; c_y < CHUNKS_Y; c_y++) {
        int i = c_z * CHUNKS_XY + c_y * CHUNKS_X + c_x;
        chunks[i].empty = true;
        chunkGenerator.createChunk(c_x, c_y, c_z, chunks);
    }

    std::cout << "CREATE CHUNKS END\n";
    std::cout << "TOOK: " << Window::get().time() - t << '\n' << '\n';

    t = Window::get().time();
    std::cout << "GENERATE OFFSETS BEGIN\n";

    generateOffsets();
    uploadOffsets();

    std::cout << "GENERATE OFFSETS END\n";
    std::cout << "TOOK: " << Window::get().time() - t << '\n' << '\n';

    t = Window::get().time();
    std::cout << "GENERATE MESHES BEGIN\n";

    generateMeshes();

    std::cout << "GENERATE MESHES END\n";
    std::cout << "TOOK: " << Window::get().time() - t << '\n' << '\n';

    m_worldCreated = true;

    vertices.clear();
    indices.clear();

    std::cout << "WORLD CREATE DONE\n";

    // set camera position above surface
    glm::vec3& position = camera.getPosition();

    position.x = CHUNK_SIZE * CHUNKS_X / 2.0f;
    position.y = CHUNK_SIZE * CHUNKS_Y + 2.0f;
    position.z = CHUNK_SIZE * CHUNKS_Z / 2.0f;

    int c_x = CHUNKS_X / 2;
    int c_z = CHUNKS_Z / 2;

    for (int c_y = 0; c_y < CHUNKS_Y; c_y++) {
        int i = c_z * CHUNKS_XY + c_y * CHUNKS_X + c_x;
        if (chunks[i].empty) {
            position.y = CHUNK_SIZE * c_y + 2.0f;
            break;
        }
    }

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        0,
        offsetSSBO
    );
}

void World::draw() {
    for (int i = 0; i < chunkCount; i++) {
        drawChunk(i);
    }
}

// TODO: handle chunk borders
void World::replaceBlock(const glm::vec3& position, Block block) {
    int blockX = int(position.x);
    int blockY = int(position.y);
    int blockZ = int(position.z);

    if (
        blockX < 0 || blockY < 0 || blockZ < 0 ||
        blockX >= CHUNKS_X * CHUNK_SIZE ||
        blockY >= CHUNKS_Y * CHUNK_SIZE ||
        blockZ >= CHUNKS_Z * CHUNK_SIZE
    ) return;

    int chunkX = blockX / CHUNK_SIZE;
    int chunkY = blockY / CHUNK_SIZE;
    int chunkZ = blockZ / CHUNK_SIZE;

    blockX -= chunkX * CHUNK_SIZE;
    blockY -= chunkY * CHUNK_SIZE;
    blockZ -= chunkZ * CHUNK_SIZE;

    int blockIndex = blockZ * CHUNK_SIZE_SQ + blockY * CHUNK_SIZE + blockX;
    int chunkIndex = chunkZ * CHUNKS_XY + chunkY * CHUNKS_X + chunkX;

    block = chunks[chunkIndex].data[blockIndex] = block;

    generateChunkMesh(chunkX, chunkY, chunkZ);
}

// TODO: use the range variable
glm::vec3 World::raycastForBlock(
    const glm::vec3& position,
    const glm::vec3& direction,
    float range,
    bool& success,
    glm::vec3& normal
) {
    // thanks to http://www.cse.yorku.ca/~amana/research/grid.pdf

    int stepX = 0;
    if (direction.x > 0.0f) stepX = 1;
    else if (direction.x < 0.0f) stepX = -1;

    int stepY = 0;
    if (direction.y > 0.0f) stepY = 1;
    else if (direction.y < 0.0f) stepY = -1;

    int stepZ = 0;
    if (direction.z > 0.0f) stepZ = 1;
    else if (direction.z < 0.0f) stepZ = -1;

    int blockX = int(position.x);
    int blockY = int(position.y);
    int blockZ = int(position.z);

    const float INF = 32.0f;

    // find the t at which the ray crosses the first block boundary for each axis
    // x(t) = t * direction.x and we solve x(t) = "proper edge which depends on direction" 
    float maxX = INF;
    if (stepX != 0.0f) {
        maxX = stepX > 0.0f ? blockX + 1 - position.x : blockX - position.x;
        maxX /= direction.x;
    }

    float maxY = INF;
    if (stepY != 0.0f) {
        maxY = stepY > 0.0f ? blockY + 1 - position.y : blockY - position.y;
        maxY /= direction.y;
    }

    float maxZ = INF;
    if (stepZ != 0.0f) {
        maxZ = stepZ > 0.0f ? blockZ + 1 - position.z : blockZ - position.z;
        maxZ /= direction.z;
    }

    // find the t needed to cross a boundary on each axis
    // x(t) = t * direction.x => t = x(t) / direction.x and we want x(t) = 1, ...
    // note that direction is normalized
    float deltaX = stepX != 0.0f ? 1.0f / std::abs(direction.x) : INF;
    float deltaY = stepY != 0.0f ? 1.0f / std::abs(direction.y) : INF;
    float deltaZ = stepZ != 0.0f ? 1.0f / std::abs(direction.z) : INF;

    int chunkX = blockX / CHUNK_SIZE;
    int chunkY = blockY / CHUNK_SIZE;
    int chunkZ = blockZ / CHUNK_SIZE;

    // this step ensures that blockX = position.x % CHUNK_SIZE, ...
    blockX -= chunkX * CHUNK_SIZE;
    blockY -= chunkY * CHUNK_SIZE;
    blockZ -= chunkZ * CHUNK_SIZE;

    // for identifying the current block
    int blockIndex = blockZ * CHUNK_SIZE_SQ + blockY * CHUNK_SIZE + blockX;

    int currentChunk = chunkZ * CHUNKS_XY + chunkY * CHUNKS_X + chunkX;
    bool chunkChanged;

    // skip the rest if outside the world
    if (
        currentChunk < 0 ||
        currentChunk >= chunkCount ||
        blockIndex < 0 ||
        blockIndex >= CHUNK_SIZE_SQ * CHUNK_SIZE
    ) {
        success = false;
        return glm::vec3(0.0f);
    }

    Block block = chunks[currentChunk].data[blockIndex];
    normal = glm::vec3(0.0f);

    // for being inside of a block
    if (block != BLOCK_AIR) {
        success = true;
        
        return glm::vec3(
            int(position.x),
            int(position.y),
            int(position.z)
        );
    }

    // for find out the normal vector
    glm::vec3 stepTaken = glm::vec3(0.0f);

    success = false;
    do {
        chunkChanged = true;

        stepTaken.x = 0.0f;
        stepTaken.y = 0.0f;
        stepTaken.z = 0.0f;

        if (maxX < maxY) {
            if (maxX < maxZ) {
                blockX += stepX;
                stepTaken.x = stepX;

                // check if the chunk should change
                if (blockX == CHUNK_SIZE) {
                    chunkX++;
                    blockX = 0;
                }
                else if (blockX == -1) {
                    chunkX--;
                    blockX = CHUNK_SIZE - 1;
                }
                else chunkChanged = false;

                // out of bounds
                if (chunkX < 0 || chunkX >= CHUNKS_X) {
                    return glm::vec3(0.0f);
                }

                maxX += deltaX;
            }
            else {
                blockZ += stepZ;
                stepTaken.z = stepZ;

                // check if the chunk should change
                if (blockZ == CHUNK_SIZE) {
                    chunkZ++;
                    blockZ = 0;
                }
                else if (blockZ == -1) {
                    chunkZ--;
                    blockZ = CHUNK_SIZE - 1;
                }
                else chunkChanged = false;

                // out of bounds
                if (chunkZ < 0 || chunkZ >= CHUNKS_Z) {
                    return glm::vec3(0.0f);
                }

                maxZ += deltaZ;
            }
        }
        else {
            if (maxY < maxZ) {
                blockY += stepY;
                stepTaken.y = stepY;

                // check if the chunk should change
                if (blockY == CHUNK_SIZE) {
                    chunkY++;
                    blockY = 0;
                }
                else if (blockY == -1) {
                    chunkY--;
                    blockY = CHUNK_SIZE - 1;
                }
                else chunkChanged = false;

                // out of bounds
                if (chunkY < 0 || chunkY >= CHUNKS_Y) {
                    return glm::vec3(0.0f);
                }

                maxY += deltaY;
            }
            else {
                blockZ += stepZ;
                stepTaken.z = stepZ;

                // check if the chunk should change
                if (blockZ == CHUNK_SIZE) {
                    chunkZ++;
                    blockZ = 0;
                }
                else if (blockZ == -1) {
                    chunkZ--;
                    blockZ = CHUNK_SIZE - 1;
                }
                else chunkChanged = false;

                // out of bounds
                if (chunkZ < 0 || chunkZ >= CHUNKS_Z) {
                    return glm::vec3(0.0f);
                }

                maxZ += deltaZ;
            }
        }

        blockIndex = blockZ * CHUNK_SIZE_SQ + blockY * CHUNK_SIZE + blockX;

        // calculate a new chunk index only when there was a change 
        if (chunkChanged) {
            currentChunk = chunkZ * CHUNKS_XY + chunkY * CHUNKS_X + chunkX;
        }

        // identify the new block
        block = chunks[currentChunk].data[blockIndex];

        // calculate the normal vector
        if (block != BLOCK_AIR) {
            normal = -stepTaken;
        }
    } while (block == BLOCK_AIR);

    success = true;

    // the intersection was found
    return glm::vec3(
        chunkX * CHUNK_SIZE + blockX,
        chunkY * CHUNK_SIZE + blockY,
        chunkZ * CHUNK_SIZE + blockZ
    );
}

void World::drawChunk(int index) {
    if (!containsGeometry[index]) return;

    glBindVertexArray(VAOs[index]);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        indicesCount[index],
        GL_UNSIGNED_INT,
        0,
        1
    );
}

void World::generateMeshes() {
    uint32 indexOffset;

    // calling clear() will not change capacity
    vertices.reserve(144 * CHUNK_SIZE_SQ);
    indices.reserve(24 * CHUNK_SIZE_SQ);

    for (int c_z = 0; c_z < CHUNKS_Z; c_z++)
    for (int c_y = 0; c_y < CHUNKS_Y; c_y++)
    for (int c_x = 0; c_x < CHUNKS_X; c_x++) {
        generateChunkMesh(c_x, c_y, c_z);
    }
}

void World::generateChunkMesh(int c_x, int c_y, int c_z) {
    uint32 indexOffset = 0;

    m_currentChunk = c_z * CHUNKS_XY + c_y * CHUNKS_X + c_x;

    if (chunks[m_currentChunk].empty) return;

    m_current_x = c_x;
    m_current_y = c_y;
    m_current_z = c_z;

    vertices.clear();
    indices.clear();

    for (int z = 0; z < CHUNK_SIZE; z++)
    for (int y = 0; y < CHUNK_SIZE; y++)
    for (int x = 0; x < CHUNK_SIZE; x++) {
        Block block = chunks[m_currentChunk].data[
            z * CHUNK_SIZE_SQ + y * CHUNK_SIZE + x
        ];
        addVoxel(block, x, y, z, indexOffset);
    }

    bool shouldUpload = containsGeometry[m_currentChunk] = vertices.size() > 0;
    indicesCount[m_currentChunk] = indices.size();

    if (shouldUpload) {
        uploadMesh();
    }
}

void World::addVoxel(Block block, int x, int y, int z, uint32& indexOffset) {
    if (block == BLOCK_AIR) return;

    if (isAir(x, y, z + 1)) {
        addFrontFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }

    if (isAir(x, y, z - 1)) {
        addBackFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }

    if (isAir(x - 1, y, z)) {
        addLeftFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }

    if (isAir(x + 1, y, z)) {
        addRightFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }

    if (isAir(x, y + 1, z)) {
        addTopFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }

    if (isAir(x, y - 1, z)) {
        addBottomFace(vertices, indices, x, y, z, indexOffset, block);
        indexOffset += 4;
    }
}

bool World::isAir(int x, int y, int z) {
    // to find the chunk in which you should land
    int nx = m_current_x;
    int ny = m_current_y;
    int nz = m_current_z;

    // find x
    if (x < 0) {
        nx--;
        x += CHUNK_SIZE;
    }
    else if (x >= CHUNK_SIZE) {
        nx++;
        x -= CHUNK_SIZE;
    }

    if (nx < 0 || nx >= CHUNKS_X) return true;

    // find y
    if (y < 0) {
        ny--;
        y += CHUNK_SIZE;
    }
    else if (y >= CHUNK_SIZE) {
        ny++;
        y -= CHUNK_SIZE;
    }

    if (ny < 0 || ny >= CHUNKS_Y) return true;

    // find z
    if (z < 0) {
        nz--;
        z += CHUNK_SIZE;
    }
    else if (z >= CHUNK_SIZE) {
        nz++;
        z -= CHUNK_SIZE;
    }

    if (nz < 0 || nz >= CHUNKS_Z) return true;

    // assumes that BLOCK_AIR = 0
    return !chunks[nz * CHUNKS_XY + ny * CHUNKS_X + nx].data[
        z * CHUNK_SIZE_SQ + y * CHUNK_SIZE + x
    ];
}

void World::uploadMesh() {
    if (m_worldCreated) {
        glDeleteVertexArrays(1, &VAOs[m_currentChunk]);
        glDeleteBuffers(1, &VBOs[m_currentChunk]);
        glDeleteBuffers(1, &EBOs[m_currentChunk]);
    }

    // generate buffers
    glGenVertexArrays(1, &VAOs[m_currentChunk]);
    glBindVertexArray(VAOs[m_currentChunk]);

    glGenBuffers(1, &VBOs[m_currentChunk]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[m_currentChunk]);

    // upload vertex data
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // vertex value
    glVertexAttribIPointer(
        0,
        1,
        GL_UNSIGNED_INT,
        sizeof(Vertex),
        (void*)(0)
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, indexVBO);

    // vertex chunk index
    glVertexAttribIPointer(
        1,
        1,
        GL_UNSIGNED_INT,
        sizeof(uint32),
        reinterpret_cast<void*>(
            static_cast<uintptr_t>(m_currentChunk * sizeof(uint32))
            )
    );
    glEnableVertexAttribArray(1);

    // one index per chunk for each vertex
    glVertexAttribDivisor(1, 1);

    glGenBuffers(1, &EBOs[m_currentChunk]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[m_currentChunk]);

    // upload indices
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );
}

void World::generateOffsets() {
    for (int z = 0; z < CHUNKS_Z; z++)
    for (int y = 0; y < CHUNKS_Y; y++)
    for (int x = 0; x < CHUNKS_X; x++) {
        int index = z * CHUNKS_XY + y * CHUNKS_X + x;

        offsets[index] = glm::vec4(
            float(x * CHUNK_SIZE),
            float(y * CHUNK_SIZE),
            float(z * CHUNK_SIZE),
            0.0f
        );
    }
}

void World::uploadOffsets() {
    uint32 indexData[CHUNKS_XY * CHUNKS_Z];

    for (int i = 0; i < offsetCount; i++) {
        indexData[i] = i;
    }

    // create index VBO
    glGenBuffers(1, &indexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, indexVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        offsetCount * sizeof(uint32),
        &indexData[0],
        GL_STATIC_DRAW
    );

    // create SSBO
    glGenBuffers(1, &offsetSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetSSBO);

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        offsetCount * sizeof(glm::vec4),
        offsets.get(),
        GL_STATIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        0,
        offsetSSBO
    );
}

World::World() :
    chunks(std::make_unique<Chunk[]>(chunkCount)),
    containsGeometry(std::make_unique<bool[]>(chunkCount)),
    offsets(std::make_unique<glm::vec4[]>(CHUNKS_XY * CHUNKS_Z)),
    indicesCount(std::make_unique<uint32[]>(CHUNKS_XY * CHUNKS_Z)),
    VAOs(std::make_unique<GLuint[]>(CHUNKS_XY * CHUNKS_Z)),
    VBOs(std::make_unique<GLuint[]>(CHUNKS_XY * CHUNKS_Z)),
    EBOs(std::make_unique<GLuint[]>(CHUNKS_XY * CHUNKS_Z)) {

    srand(time(NULL));
    int seed = rand();
    chunkGenerator.initialize(seed);

    for (int i = 0; i < chunkCount; i++) {
        containsGeometry[i] = false;
    }
}