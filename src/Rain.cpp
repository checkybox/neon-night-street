#include "Rain.h"
#include <glad/glad.h>
#include <cstdlib>

Rain::Rain(int dropCount)
    : count(dropCount), VAO(0), VBO(0) {
    drops.resize(count);
    vertexBufferData.resize(count * 14);

    for (int i = 0; i < count; ++i) {
        drops[i].pos = glm::vec3(
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 34.0f,
            (static_cast<float>(rand()) / RAND_MAX) * 16.0f,
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 34.0f
        );
        drops[i].speed  = 20.0f + (static_cast<float>(rand()) / RAND_MAX) * 8.0f;
        drops[i].length = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.20f;
        drops[i].alpha  = 0.28f + (static_cast<float>(rand()) / RAND_MAX) * 0.22f;
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

void Rain::update(float deltaTime, const glm::vec3& centerPos) {
    size_t idx = 0;
    for (int i = 0; i < count; ++i) {
        auto& d = drops[i];
        d.pos.y -= d.speed * deltaTime;

        if (d.pos.y < 0.05f) {
            d.pos.y += 15.5f;
        }

        float dx = d.pos.x - centerPos.x;
        if (dx > 17.0f)  d.pos.x -= 34.0f;
        else if (dx < -17.0f) d.pos.x += 34.0f;

        float dz = d.pos.z - centerPos.z;
        if (dz > 17.0f)  d.pos.z -= 34.0f;
        else if (dz < -17.0f) d.pos.z += 34.0f;

        // Top vertex (faded tail)
        vertexBufferData[idx++] = d.pos.x;
        vertexBufferData[idx++] = d.pos.y;
        vertexBufferData[idx++] = d.pos.z;
        vertexBufferData[idx++] = 0.75f;
        vertexBufferData[idx++] = 0.88f;
        vertexBufferData[idx++] = 0.98f;
        vertexBufferData[idx++] = d.alpha * 0.20f;

        // Bottom vertex (leading streak drop)
        vertexBufferData[idx++] = d.pos.x;
        vertexBufferData[idx++] = d.pos.y - d.length;
        vertexBufferData[idx++] = d.pos.z;
        vertexBufferData[idx++] = 0.85f;
        vertexBufferData[idx++] = 0.95f;
        vertexBufferData[idx++] = 1.00f;
        vertexBufferData[idx++] = d.alpha;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(float), vertexBufferData.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Rain::render(const glm::mat4& view, const glm::mat4& projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, count * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
