#pragma once

#include <glad/gl.h>
#include <vector>
#include <memory>

#include "types/chunk.h"
#include "types/vertex.h"
#include "chunk_generator.h"
#include "face_generator.h"

#include "../../engine/framework.h"

class World {
public:
    void create(FPCamera& camera);
    void draw();

    glm::vec3 raycastForBlock(
        const glm::vec3& position, 
        const glm::vec3& direction, 
        float range,
        bool& success
    );

    World();
private:
    ChunkGenerator chunkGenerator;

    const int chunkCount = CHUNKS_X * CHUNKS_Y * CHUNKS_Z;
	std::unique_ptr<Chunk[]> chunks;

    std::unique_ptr<bool[]> containsGeometry;

    void drawChunk(int index);

    std::unique_ptr<uint32[]> indicesCount;
    std::unique_ptr<GLuint[]> VAOs;
    std::unique_ptr<GLuint[]> VBOs;
    std::unique_ptr<GLuint[]> EBOs;

	std::vector<Vertex> vertices;
	std::vector<uint32> indices;

    int currentChunk = 0;
    int current_x = 0;
    int current_y = 0;
    int current_z = 0;

    void generateMeshes();

    void generateChunkMesh(int c_x, int c_y, int c_z);
    void addVoxel(Block block, int x, int y, int z, uint32& indexOffset);
    bool isAir(int x, int y, int z);

    bool worldCreated = false;
    void uploadMesh();

    const int offsetCount = CHUNKS_X * CHUNKS_Y * CHUNKS_Z;
    std::unique_ptr<glm::vec4[]> offsets;
    
    void generateOffsets();

    // shader storage buffer object
    GLuint offsetSSBO;
    GLuint indexVBO;

    void uploadOffsets();
};