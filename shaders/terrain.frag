#version 410 core

in vec3 FragPos;
in vec3 Normal;
in float Height;

uniform float time;

out vec4 FragColor;

void main() {
    // Elevation-based color: low=dark green, high=grey rock
    vec3 low_color  = vec3(0.08, 0.18, 0.08);
    vec3 high_color = vec3(0.28, 0.25, 0.22);
    float t = clamp(Height / 40.0, 0.0, 1.0);
    vec3 base = mix(low_color, high_color, t);

    // Subtle grid lines — tactical map feel
    float grid = 200.0;
    float lx = abs(mod(FragPos.x, grid) - grid * 0.5);
    float lz = abs(mod(FragPos.z, grid) - grid * 0.5);
    float line = step(grid * 0.48, lx) + step(grid * 0.48, lz);
    base = mix(base, vec3(0.15, 0.35, 0.15), clamp(line, 0.0, 1.0) * 0.5);

    // Simple diffuse lighting
    vec3 light_dir = normalize(vec3(0.4, 1.0, 0.3));
    float diff     = max(dot(Normal, light_dir), 0.0);
    vec3 color     = base * (0.3 + 0.7 * diff);

    FragColor = vec4(color, 1.0);
}
