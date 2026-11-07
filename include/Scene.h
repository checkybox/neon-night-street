#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Light.h"

struct SceneNode {
    std::shared_ptr<Mesh> mesh;
    glm::mat4 localTransform;
    bool isEmissive;
    std::vector<SceneNode> children;

    SceneNode();
    SceneNode(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), bool isEmissive = false);

    void addChild(const SceneNode& child);
    void draw(const Shader& shader, const glm::mat4& parentTransform = glm::mat4(1.0f)) const;
};

class Scene {
public:
    Scene();
    ~Scene();

    void init();
    void render(const Shader& shader);
    void setupLights(const Shader& shader);

private:
    std::vector<SceneNode> rootNodes;
    DirLight dirLight;
    std::vector<PointLight> pointLights;

    std::shared_ptr<Mesh> createBox(float width, float height, float depth, const glm::vec3& color);
    std::shared_ptr<Mesh> createPlane(float width, float depth, const glm::vec3& color);
};

#endif
