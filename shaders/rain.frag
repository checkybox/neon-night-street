#version 330 core
in vec4 dropColor;
in vec3 FragPos;
out vec4 FragColor;

struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float intensity;
};

uniform SpotLight spotLight;
uniform float lightMultiplier;

void main() {
    float mult = (lightMultiplier > 0.0) ? lightMultiplier : 1.0;
    vec3 col = dropColor.rgb * (0.15 + 0.85 * mult);
    float alpha = dropColor.a * (0.12 + 0.88 * mult);

    if (spotLight.enabled) {
        vec3 lightDir = normalize(FragPos - spotLight.position);
        float theta = dot(lightDir, normalize(spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float spot = clamp((theta - spotLight.outerCutOff) / max(0.001, epsilon), 0.0, 1.0);
        float dist = length(FragPos - spotLight.position);
        float atten = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        
        float flashBoost = spot * atten * spotLight.intensity;
        col += vec3(1.2, 1.3, 1.5) * (flashBoost * 2.2);
        alpha = min(1.0, alpha + flashBoost * 0.65);
    }

    FragColor = vec4(col, alpha);
}
