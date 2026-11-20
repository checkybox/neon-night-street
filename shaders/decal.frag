#version 330 core
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform bool isEmissive;
uniform float neonFlicker;
uniform float powerFactor;
uniform vec3 viewPos;
uniform bool fogEnabled;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    bool enabled;
};
uniform SpotLight spotLight;

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

    float pFactor = (powerFactor >= 0.0) ? powerFactor : 1.0;
    float ambient = 0.05 + 0.87 * pFactor;

    float flash = 0.0;
    if (spotLight.enabled) {
        vec3 toLight = spotLight.position - FragPos;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;
        float theta = dot(lightDir, normalize(-spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
        float atten = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        flash = intensity * atten * 3.5;
    }

    float totalLight = min(1.35, ambient + flash);
    float flicker = isEmissive ? (neonFlicker * pFactor) : 1.0;
    vec3 col = texColor.rgb * (isEmissive ? (2.50 * flicker) : totalLight);

    if (fogEnabled) {
        float dist = length(viewPos - FragPos);
        vec3 fogColor = vec3(0.02, 0.025, 0.045);
        float fogFactor = clamp(1.0 - exp(-dist * 0.015), 0.0, 0.85);
        col = mix(col, fogColor, fogFactor);
    }

    FragColor = vec4(ACESFilm(col), texColor.a);
}
