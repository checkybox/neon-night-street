#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 vertexColor;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform bool isFlickering;
uniform float neonFlicker;
uniform float emissiveBoost;
uniform bool fogEnabled;
uniform float powerFactor;

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
    float boost = emissiveBoost > 0.05 ? emissiveBoost : 1.0;
    float pFactor = (powerFactor >= 0.0) ? powerFactor : 1.0;
    float flicker = isFlickering ? (neonFlicker * pFactor) : pFactor;

    float maxC = max(vertexColor.r, max(vertexColor.g, vertexColor.b));
    float minC = min(vertexColor.r, min(vertexColor.g, vertexColor.b));
    float sat = maxC > 0.001 ? (maxC - minC) / maxC : 0.0;

    float flash = 0.0;
    if (spotLight.enabled) {
        vec3 toLight = spotLight.position - FragPos;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;
        float theta = dot(lightDir, normalize(-spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
        float atten = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        vec3 norm = (length(Normal) > 0.1) ? normalize(Normal) : lightDir;
        float diff = max(dot(norm, lightDir), 0.28);
        flash = diff * intensity * atten * 3.2;
    }

    vec3 baseMat = hasTexture ? texture(diffuseTexture, TexCoords).rgb : vertexColor;
    vec3 physicalReflection = baseMat * (0.05 + flash);

    vec3 emissiveGlow = vec3(0.0);
    if (maxC < 0.20) {
        emissiveGlow = vertexColor * 0.15;
    } else if (maxC < 0.58 && sat < 0.20) {
        emissiveGlow = vertexColor * 0.20;
    } else {
        emissiveGlow = baseMat * (1.35 * boost) * flicker;
    }

    vec3 col = emissiveGlow + physicalReflection;
    if (fogEnabled && FragPos.y < 70.0) {
        vec3 fogColor = vec3(0.02, 0.025, 0.045);
        float dist = length(viewPos - FragPos);
        float fogFactor = clamp(1.0 - exp(-dist * 0.008), 0.0, 0.70);
        col = mix(col, fogColor, fogFactor);
    }
    FragColor = vec4(ACESFilm(col), 1.0);
}
