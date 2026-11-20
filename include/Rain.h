#ifndef RAIN_H
#define RAIN_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

class Rain {
public:
    explicit Rain(int maxDrops = 6000);
    ~Rain();

    void setIntensity(int level);
    int getIntensity() const { return intensityLevel; }

    void update(float deltaTime, const glm::vec3& centerPos);
    void render(const glm::mat4& view, const glm::mat4& projection, float lightMultiplier = 1.0f,
                bool flashlightOn = false, const glm::vec3& flashPos = glm::vec3(0.0f),
                const glm::vec3& flashDir = glm::vec3(0.0f, 0.0f, -1.0f), float flashBrightness = 1.0f);

private:
    struct Drop {
        glm::vec3 pos;
        float speed;
        float length;
        float alpha;
        float driftX;
        float driftZ;
        float swayPhase;
        float swayFreq;
    };

    int maxCount;
    int intensityLevel;
    std::vector<Drop> drops;
    std::vector<float> vertexBufferData;
    unsigned int VAO, VBO;
    std::unique_ptr<Shader> shader;
};

#endif
