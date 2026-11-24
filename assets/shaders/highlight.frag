#version 440 core

out vec4 FragColor;

in vec3 position;

void main() {
    vec3 pos = position + vec3(0.5);

    float frameMin = 0.015;
    float frameMax = 1.0 - frameMin;

    // TODO: use a mathematical formula, not if statements
    if (
        pos.x > frameMin &&
        pos.x < frameMax &&
        pos.y > frameMin && 
        pos.y < frameMax 
    ) discard;

    if (
        pos.x > frameMin &&
        pos.x < frameMax &&
        pos.z > frameMin && 
        pos.z < frameMax 
    ) discard;

    if (
        pos.y > frameMin &&
        pos.y < frameMax &&
        pos.z > frameMin && 
        pos.z < frameMax 
    ) discard;

    FragColor = vec4(0.5, 0, 0, 0.95);
}