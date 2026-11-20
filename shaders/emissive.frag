#version 330 core
in vec3 FragPos;
in vec3 vertexColor;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform bool isFlickering;
uniform float neonFlicker;
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
    float flicker = isFlickering ? (neonFlicker > 0.0 ? neonFlicker : 1.0) : 1.0;
    vec3 col = vertexColor * 1.25 * flicker;
    if (hasTexture) {
        col = texture(diffuseTexture, TexCoords).rgb * 1.30 * flicker;
    }
    if (fogEnabled && FragPos.y < 70.0) {
        vec3 fogColor = vec3(0.02, 0.025, 0.045);
        float dist = length(viewPos - FragPos);
        float fogFactor = clamp(1.0 - exp(-dist * 0.008), 0.0, 0.70);
        col = mix(col, fogColor, fogFactor);
    }
    FragColor = vec4(ACESFilm(col), 1.0);
}
