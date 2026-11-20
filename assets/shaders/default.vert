#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aLayer;

uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uOffset;

out vec2 uv;
flat out float layer;

void main() {
    uv = aUV;
    layer = aLayer;

    gl_Position = uProjection * uView * vec4(aPos + uOffset, 1.0); 
}