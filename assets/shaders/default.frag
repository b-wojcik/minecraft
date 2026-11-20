#version 440 core

out vec4 FragColor;

in vec2 uv;
flat in float layer;

uniform sampler2DArray txt;

void main() {
    FragColor = texture(txt, vec3(uv, layer));
}