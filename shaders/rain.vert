#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 dropColor;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    dropColor = aColor;
    FragPos = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
