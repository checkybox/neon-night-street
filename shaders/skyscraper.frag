#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 vertexColor;
in vec2 TexCoords;

out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform bool isEmissive;
uniform bool fogEnabled;
uniform float powerFactor;

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 fogColor = vec3(0.02, 0.025, 0.045);
    float dist = length(viewPos - FragPos);

    if (isEmissive) {
        float pFactor = (powerFactor >= 0.0) ? powerFactor : 1.0;
        float winPower = 1.0;
        if (!hasTexture) {
            float rnd = fract(sin(dot(floor(FragPos.xyz * 0.35), vec3(12.9898, 78.233, 45.164))) * 43758.5453);
            float thresh = 0.12 + rnd * 0.78;
            if (pFactor < thresh) {
                winPower = 0.04;
            } else if (pFactor < 0.98 && (pFactor - thresh) < 0.08 && rnd > 0.45) {
                winPower = 0.30;
            } else {
                winPower = 0.04 + 0.96 * min(1.0, (pFactor - thresh) / 0.15);
            }
        }
        vec3 col = vertexColor * (1.25 * winPower);
        if (hasTexture) {
            col = texture(diffuseTexture, TexCoords).rgb * 1.30;
        }
        if (fogEnabled && FragPos.y < 70.0) {
            float fogFactor = clamp(1.0 - exp(-dist * 0.008), 0.0, 0.70);
            col = mix(col, fogColor, fogFactor);
        }
        FragColor = vec4(ACESFilm(col), 1.0);
        return;
    }

    vec3 baseColor = vertexColor;
    if (hasTexture) {
        baseColor = texture(diffuseTexture, TexCoords).rgb;
    }

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = dirLight.ambient * baseColor;
    vec3 diffuse = dirLight.diffuse * (diff * baseColor);
    vec3 result  = ambient + diffuse;

    if (fogEnabled) {
        float fogFactor = clamp(1.0 - exp(-dist * 0.015), 0.0, 0.95);
        result = mix(result, fogColor, fogFactor);
    }

    FragColor = vec4(ACESFilm(result), 1.0);
}
