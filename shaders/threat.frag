#version 410 core

in vec3 LocalPos;

uniform float time;

out vec4 FragColor;

void main() {
    // Pulsing red ring — 1.5 Hz
    float pulse = 0.5 + 0.5 * sin(time * 9.42);
    vec3  color = vec3(1.0, 0.1, 0.1);
    FragColor   = vec4(color, 0.4 + 0.6 * pulse);
}
