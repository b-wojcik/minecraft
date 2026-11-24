#version 440 core

layout (location = 0) in uint aValue;
layout (location = 1) in uint aChunkIndex;

layout (std430, binding = 0) readonly buffer ChunkOffsets {
    vec4 chunkOffset[];
};

uniform mat4 uProjView;

flat out float light;
flat out float layer;
flat out vec3 normal;

out vec2 uv;

void main() {
    // packed uint32
    // | 000000 | 00 | 00 | 00 | 00 | 000000 | 000000 | 000000 |
    // | layer  | uv | nz | ny | nx | z pos  | y pos  | x pos  |

    // sign of ny, value of ny (0/1)
    uint sOfY = bitfieldExtract(aValue, 21, 1);
    uint vOfY = bitfieldExtract(aValue, 20, 1);

    float sOfX = float(bitfieldExtract(aValue, 19, 1));
    float vOfX = float(bitfieldExtract(aValue, 18, 1));

    float sOfZ = float(bitfieldExtract(aValue, 23, 1));
    float vOfZ = float(bitfieldExtract(aValue, 22, 1));

    normal = vec3(
        vOfX * (sOfX * 2.0 - 1.0),
        float(vOfY) * (float(sOfY) * 2.0 - 1.0),
        vOfZ * (sOfZ * 2.0 - 1.0)
    );

    uint blockType = bitfieldExtract(aValue, 26, 6);
    layer = blockType * 3.0 + vOfY * (sOfY + 1);

    // directional light
    float dirLight = normal.x * 0.6;
    dirLight += abs(normal.y * 0.75) + normal.y * 0.25;
    dirLight += normal.z * 0.8;
    light = abs(dirLight);

    uv = vec2(
        bitfieldExtract(aValue, 24, 1),
        bitfieldExtract(aValue, 25, 1)
    );

    vec3 pos = vec3(
        bitfieldExtract(aValue, 0, 6),
        bitfieldExtract(aValue, 6, 6),
        bitfieldExtract(aValue, 12, 6)
    );

    gl_Position = uProjView *
    vec4(
        pos + chunkOffset[aChunkIndex].xyz, 1.0
    ); 
}