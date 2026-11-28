#version 440 core

out vec4 FragColor;

flat in float light;
flat in float layer;
flat in vec3 normal;
in vec2 uv;

uniform sampler2DArray uTexture;

void main() {
    vec4 color = texture(uTexture, vec3(uv, layer));
    color.rgb *= light;
    FragColor = color;
}