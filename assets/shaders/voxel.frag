#version 440 core

out vec4 FragColor;

flat in float light;
flat in float layer;
flat in vec3 normal;
in vec2 uv;

uniform sampler2DArray uFaceTexture;

void main() {
    vec4 color = texture(uFaceTexture, vec3(uv, layer));
    color.rgb *= light;
    
    FragColor = color;
}