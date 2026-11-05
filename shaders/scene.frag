#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 vertexColor;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.2, 1.0, 0.3));
    float diff = max(dot(normalize(Normal), lightDir), 0.25);
    vec3 result = vertexColor * diff;
    FragColor = vec4(result, 1.0);
}
