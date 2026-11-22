#include "world.h"

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
    // currently you do not check if a chunk even contains any geometry (the case mentioned above)!!!

    for (int c_z = 0; c_z < CHUNKS_Z; c_z++)
    for (int c_x = 0; c_x < CHUNKS_X; c_x++)
    for (int c_y = 0; c_y < CHUNKS_Y; c_y++) {
        int i = c_z * CHUNKS_X * CHUNKS_Y + c_y * CHUNKS_X + c_x;
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
        int i = c_z * CHUNKS_X * CHUNKS_Y + c_y * CHUNKS_X + c_x;
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
        indexOffset = 0;

        currentChunk = c_z * CHUNKS_X * CHUNKS_Y + c_y * CHUNKS_X + c_x;

        if (chunks[currentChunk].empty) continue;

        current_x = c_x;
        current_y = c_y;
        current_z = c_z;

        vertices.clear();
        indices.clear();

        for (int z = 0; z < CHUNK_SIZE; z++)
        for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Block block = chunks[currentChunk].data[
                z * CHUNK_SIZE_SQ + y * CHUNK_SIZE + x
            ];
            addVoxel(block, x, y, z, indexOffset);
        }

        bool shouldUpload = containsGeometry[currentChunk] = vertices.size() > 0;
        indicesCount[currentChunk] = indices.size();

        if (shouldUpload) {
            uploadMesh();
        }
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
    int nx = current_x;
    int ny = current_y;
    int nz = current_z;

    if (x < 0) {
        nx--;
        x += CHUNK_SIZE;
    }
    else if (x >= CHUNK_SIZE) {
        nx++;
        x -= CHUNK_SIZE;
    }

    if (y < 0) {
        ny--;
        y += CHUNK_SIZE;
    }
    else if (y >= CHUNK_SIZE) {
        ny++;
        y -= CHUNK_SIZE;
    }

    if (z < 0) {
        nz--;
        z += CHUNK_SIZE;
    }
    else if (z >= CHUNK_SIZE) {
        nz++;
        z -= CHUNK_SIZE;
    }

    if (
        nx < 0 || nx >= CHUNKS_X ||
        ny < 0 || ny >= CHUNKS_Y ||
        nz < 0 || nz >= CHUNKS_Z
        ) {
        return true;
    }

    const int chunkIndex = nz * CHUNKS_X * CHUNKS_Y + ny * CHUNKS_X + nx;
    return chunks[chunkIndex].data[
        z * CHUNK_SIZE_SQ + y * CHUNK_SIZE + x
    ] == BLOCK_AIR;
}

void World::uploadMesh() {
    // generate buffers
    glGenVertexArrays(1, &VAOs[currentChunk]);
    glBindVertexArray(VAOs[currentChunk]);

    glGenBuffers(1, &VBOs[currentChunk]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[currentChunk]);

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
            static_cast<uintptr_t>(currentChunk * sizeof(uint32))
            )
    );
    glEnableVertexAttribArray(1);

    // one index per chunk for each vertex
    glVertexAttribDivisor(1, 1);

    glGenBuffers(1, &EBOs[currentChunk]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[currentChunk]);

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
        int index = z * CHUNKS_X * CHUNKS_Y + y * CHUNKS_X + x;

        offsets[index] = glm::vec4(
            float(x * CHUNK_SIZE),
            float(y * CHUNK_SIZE),
            float(z * CHUNK_SIZE),
            0.0f
        );
    }
}

void World::uploadOffsets() {
    uint32 indexData[CHUNKS_X * CHUNKS_Y * CHUNKS_Z];

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
    offsets(std::make_unique<glm::vec4[]>(CHUNKS_X* CHUNKS_Y* CHUNKS_Z)),
    indicesCount(std::make_unique<uint32[]>(CHUNKS_X* CHUNKS_Y* CHUNKS_Z)),
    VAOs(std::make_unique<GLuint[]>(CHUNKS_X* CHUNKS_Y* CHUNKS_Z)),
    VBOs(std::make_unique<GLuint[]>(CHUNKS_X* CHUNKS_Y* CHUNKS_Z)),
    EBOs(std::make_unique<GLuint[]>(CHUNKS_X* CHUNKS_Y* CHUNKS_Z)) {

    srand(time(NULL));
    int seed = rand();
    chunkGenerator.initialize(seed);

    for (int i = 0; i < chunkCount; i++) {
        containsGeometry[i] = false;
    }
}