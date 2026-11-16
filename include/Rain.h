#ifndef RAIN_H
#define RAIN_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

class Rain {
public:
    explicit Rain(int dropCount = 2000);
    ~Rain();

    void update(float deltaTime, const glm::vec3& centerPos);
    void render(const glm::mat4& view, const glm::mat4& projection);

private:
    struct Drop {
        glm::vec3 pos;
        float speed;
        float length;
        float alpha;
    };

    int count;
    std::vector<Drop> drops;
    std::vector<float> vertexBufferData;
    unsigned int VAO, VBO;
    std::unique_ptr<Shader> shader;
};

#endif
