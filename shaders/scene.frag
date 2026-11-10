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

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 16

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;

uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform bool isRoad;
uniform bool isEmissive;

// Procedural 2D Noise & FBM for Organic Puddles
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Hermite curve

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float fbm(vec2 p) {
    float v = 0.0;
    v += 0.65 * noise(p);
    v += 0.35 * noise(p * 2.15);
    return v;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength);

void main() {
    if (isEmissive) {
        vec3 col = vertexColor;
        if (hasTexture) {
            col = texture(diffuseTexture, TexCoords).rgb;
        }
        FragColor = vec4(col, 1.0);
        return;
    }

    vec3 baseColor = vertexColor;
    if (hasTexture) {
        baseColor = texture(diffuseTexture, TexCoords).rgb;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float puddleMask = 0.0;
    float shininess = 24.0;
    float specularStrength = 0.3;

    if (isRoad) {
        // Procedural world-space noise: completely non-repeating across the 60m street!
        // At frequency 0.16, puddles naturally form in 2 to 4 meter irregular organic patches
        float n = fbm(FragPos.xz * 0.16 + vec2(4.2, 1.7));

        // Smooth transition: dry pavement -> damp dark asphalt -> mirror puddle pool
        puddleMask = smoothstep(0.46, 0.58, n);

        // Wet asphalt absorbs more light: darken surface inside puddles
        baseColor = mix(baseColor, baseColor * 0.35, puddleMask);

        // Deep water pool: razor-sharp specular shine reflecting streetlamps
        shininess = mix(24.0, 128.0, puddleMask);
        specularStrength = mix(0.25, 3.5, puddleMask);
    }

    // 1. Directional Moonlight (subtle dark ambient fill)
    vec3 result = CalcDirLight(dirLight, norm, viewDir, baseColor, shininess, specularStrength);

    // 2. Point lights (8+ colored lights)
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, baseColor, shininess, specularStrength);
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 ambient  = light.ambient  * baseColor;
    vec3 diffuse  = light.diffuse  * diff * baseColor;
    vec3 specular = light.specular * spec * (specularStrength * 0.35);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength) {
    vec3 lightVec = light.position - fragPos;
    float distance = length(lightVec);
    // Early cutoff: Skip expensive lighting calculations if fragment is beyond reach
    if (distance > 24.0) {
        return vec3(0.0);
    }
    vec3 lightDir = lightVec / distance;
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * baseColor;
    vec3 diffuse  = light.diffuse  * diff * baseColor;
    vec3 specular = light.specular * spec * specularStrength;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
