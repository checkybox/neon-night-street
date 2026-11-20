#version 330 core
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform bool isEmissive;
uniform float neonFlicker;
uniform vec3 viewPos;
uniform bool fogEnabled;

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec4 texColor = texture(diffuseTexture, TexCoords);
    if (texColor.a < 0.15) discard;

    float flicker = isEmissive ? ((neonFlicker > 0.0) ? neonFlicker : 1.0) : 1.0;
    vec3 col = texColor.rgb * (isEmissive ? 1.35 : 0.92) * flicker;

    if (fogEnabled) {
        float dist = length(viewPos - FragPos);
        vec3 fogColor = vec3(0.02, 0.025, 0.045);
        float fogFactor = clamp(1.0 - exp(-dist * 0.015), 0.0, 0.85);
        col = mix(col, fogColor, fogFactor);
    }

    FragColor = vec4(ACESFilm(col), texColor.a);
}
