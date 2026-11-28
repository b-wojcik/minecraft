#version 440 core

out vec4 FragColor;

in vec3 position;

uniform float uTime;

void main() {
    vec3 pos = position + vec3(0.5);

    float frameMin = 0.04;
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

    float wave = sin(uTime * 2.0) * 0.5 + 0.5;

    float timeStep = smoothstep(0.0, 1.0, wave);

    vec3 color = mix(
        vec3(0.066, 0.126, 0.228),
        vec3(0.132, 0.216, 0.318),
        timeStep
    );

    float alpha = 0.85 + 0.10 * sin(uTime * 1.25);

    FragColor = vec4(color, alpha);
}