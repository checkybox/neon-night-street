#include "Rain.h"
#include <glad/glad.h>
#include <cstdlib>
#include <cmath>

Rain::Rain(int maxDrops)
    : maxCount(maxDrops), intensityLevel(1), VAO(0), VBO(0) {
    drops.resize(maxCount);
    vertexBufferData.resize(maxCount * 14);

    for (int i = 0; i < maxCount; ++i) {
        drops[i].pos = glm::vec3(
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 34.0f,
            (static_cast<float>(rand()) / RAND_MAX) * 16.0f,
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 34.0f
        );
        drops[i].speed  = 19.0f + (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
        drops[i].length = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.25f;
        drops[i].alpha  = 0.25f + (static_cast<float>(rand()) / RAND_MAX) * 0.25f;
        drops[i].driftX = -1.2f + ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 1.6f;
        drops[i].driftZ = 0.8f + ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 1.4f;
        drops[i].swayPhase = (static_cast<float>(rand()) / RAND_MAX) * 6.28318f;
        drops[i].swayFreq = 1.8f + (static_cast<float>(rand()) / RAND_MAX) * 2.2f;
    }

    shader = std::unique_ptr<Shader>(new Shader("shaders/rain.vert", "shaders/rain.frag"));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(float), nullptr, GL_STREAM_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

    // Color & Alpha attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

Rain::~Rain() {
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

void Rain::setIntensity(int level) {
    intensityLevel = (level < 0) ? 0 : ((level > 3) ? 3 : level);
}

void Rain::update(float deltaTime, const glm::vec3& centerPos) {
    if (intensityLevel == 0) return;
    int activeCount = (intensityLevel == 1) ? 2000 : ((intensityLevel == 2) ? 4000 : 6000);

    size_t idx = 0;
    for (int i = 0; i < activeCount; ++i) {
        auto& d = drops[i];
        float sway = std::sin(d.pos.y * d.swayFreq + d.swayPhase);
        float vx = d.driftX + sway * 0.45f;
        float vz = d.driftZ + std::cos(d.pos.y * d.swayFreq + d.swayPhase) * 0.35f;

        d.pos.x += vx * deltaTime;
        d.pos.y -= d.speed * deltaTime;
        d.pos.z += vz * deltaTime;

        if (d.pos.y < 0.05f) {
            d.pos.y += 15.5f;
        }

        float dx = d.pos.x - centerPos.x;
        if (dx > 17.0f)  d.pos.x -= 34.0f;
        else if (dx < -17.0f) d.pos.x += 34.0f;

        float dz = d.pos.z - centerPos.z;
        if (dz > 17.0f)  d.pos.z -= 34.0f;
        else if (dz < -17.0f) d.pos.z += 34.0f;

        float streakScale = d.length / d.speed;
        glm::vec3 head = d.pos - glm::vec3(vx * streakScale, d.length, vz * streakScale);

        // Top vertex (faded tail)
        vertexBufferData[idx++] = d.pos.x;
        vertexBufferData[idx++] = d.pos.y;
        vertexBufferData[idx++] = d.pos.z;
        vertexBufferData[idx++] = 0.75f;
        vertexBufferData[idx++] = 0.88f;
        vertexBufferData[idx++] = 0.98f;
        vertexBufferData[idx++] = d.alpha * 0.20f;

        // Bottom vertex (leading streak drop)
        vertexBufferData[idx++] = head.x;
        vertexBufferData[idx++] = head.y;
        vertexBufferData[idx++] = head.z;
        vertexBufferData[idx++] = 0.85f;
        vertexBufferData[idx++] = 0.95f;
        vertexBufferData[idx++] = 1.00f;
        vertexBufferData[idx++] = d.alpha;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, idx * sizeof(float), vertexBufferData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Rain::render(const glm::mat4& view, const glm::mat4& projection, float lightMultiplier,
                  bool flashlightOn, const glm::vec3& flashPos, const glm::vec3& flashDir, float flashBrightness) {
    if (intensityLevel == 0) return;
    int activeCount = (intensityLevel == 1) ? 2000 : ((intensityLevel == 2) ? 4000 : 6000);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setFloat("lightMultiplier", lightMultiplier);

    shader->setBool("spotLight.enabled", flashlightOn);
    if (flashlightOn) {
        shader->setVec3("spotLight.position", flashPos);
        shader->setVec3("spotLight.direction", flashDir);
        shader->setFloat("spotLight.cutOff", std::cos(glm::radians(12.5f)));
        shader->setFloat("spotLight.outerCutOff", std::cos(glm::radians(17.5f)));
        shader->setFloat("spotLight.intensity", flashBrightness);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, activeCount * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
