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

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    bool enabled;
};

#define NR_POINT_LIGHTS 32

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int activePointLights;
uniform SpotLight spotLight;
uniform vec3 viewPos;

uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform bool isRoad;
uniform bool isEmissive;
uniform bool isFlickering;
uniform float neonFlicker;
uniform bool fogEnabled;

// Procedural noise and fractal brownian motion for puddles
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

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
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength);

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    if (isEmissive) {
        float flicker = isFlickering ? (neonFlicker > 0.0 ? neonFlicker : 1.0) : 1.0;
        vec3 col = vertexColor * 1.25 * flicker;
        if (hasTexture) {
            col = texture(diffuseTexture, TexCoords).rgb * 1.30 * flicker;
        }
        if (fogEnabled && FragPos.y < 70.0) {
            float dist = length(viewPos - FragPos);
            float fogFactor = clamp(1.0 - exp(-dist * 0.008), 0.0, 0.70);
            col = mix(col, vec3(0.02, 0.025, 0.045), fogFactor);
        }
        FragColor = vec4(ACESFilm(col), 1.0);
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
    float specularStrength = 0.0;

    if (isRoad) {
        float n = fbm(FragPos.xz * 0.16 + vec2(4.2, 1.7));
        puddleMask = smoothstep(0.24, 0.42, n);
        baseColor = mix(baseColor, baseColor * 0.35, puddleMask);
        shininess = mix(24.0, 128.0, puddleMask);
        specularStrength = mix(0.25, 3.5, puddleMask);
    }

    vec3 result = CalcDirLight(dirLight, norm, viewDir, baseColor, shininess, specularStrength);

    int lightCount = (activePointLights > 0 && activePointLights <= NR_POINT_LIGHTS) ? activePointLights : NR_POINT_LIGHTS;
    for (int i = 0; i < lightCount; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, baseColor, shininess, specularStrength);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, baseColor, shininess, specularStrength);

    if (fogEnabled) {
        float dist = length(viewPos - FragPos);
        float fogFactor = clamp(1.0 - exp(-dist * 0.024), 0.0, 0.95);
        result = mix(result, vec3(0.02, 0.025, 0.045), fogFactor);
    }

    FragColor = vec4(ACESFilm(result), 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    float spec = 0.0;
    if (specularStrength > 0.1 && diff > 0.05) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        if (NdotH > 0.0) {
            spec = pow(NdotH, shininess);
        }
    }

    vec3 ambient  = light.ambient  * baseColor;
    vec3 diffuse  = light.diffuse  * (diff * baseColor);
    vec3 specular = light.specular * (spec * specularStrength * 0.35);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength) {
    vec3 lightVec = light.position - fragPos;
    float distSq = dot(lightVec, lightVec);
    if (distSq > 256.0) {
        return vec3(0.0);
    }
    float NdotL = dot(normal, lightVec);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }
    float invDist = inversesqrt(distSq);
    float distance = distSq * invDist;
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distSq);
    float fade = clamp(1.0 - distSq * (1.0 / 256.0), 0.0, 1.0);
    attenuation *= fade;

    float diff = NdotL * invDist;
    vec3 diffuse = light.diffuse * (diff * baseColor);

    vec3 specular = vec3(0.0);
    if (specularStrength > 0.1 && diff > 0.05) {
        vec3 lightDir = lightVec * invDist;
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        if (NdotH > 0.0) {
            specular = light.specular * (pow(NdotH, shininess) * specularStrength);
        }
    }

    return (diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, float shininess, float specularStrength) {
    if (!light.enabled) {
        return vec3(0.0);
    }
    vec3 lightVec = light.position - fragPos;
    float distSq = dot(lightVec, lightVec);
    if (distSq > 400.0) {
        return vec3(0.0);
    }
    float invDist = inversesqrt(distSq);
    float distance = distSq * invDist;
    vec3 lightDir = lightVec * invDist;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    if (intensity <= 0.0) {
        return vec3(0.0);
    }

    float diff = max(dot(normal, lightDir), 0.0);
    if (diff <= 0.01) {
        return vec3(0.0);
    }

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distSq);
    float spec = 0.0;
    if (specularStrength > 0.1 && diff > 0.05) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        if (NdotH > 0.0) {
            spec = pow(NdotH, shininess);
        }
    }

    vec3 effectiveColor = max(baseColor, vec3(0.18));
    vec3 diffuse  = light.diffuse  * (max(diff, 0.30) * effectiveColor * intensity);
    vec3 specular = light.specular * (spec * specularStrength * intensity);

    return (diffuse + specular) * attenuation;
}
