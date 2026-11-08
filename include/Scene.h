#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Light.h"
#include "Texture.h"

struct SceneNode {
    std::shared_ptr<Mesh> mesh;
    glm::mat4 localTransform;
    bool isEmissive;
    bool isRoad;
    std::shared_ptr<Texture> texture;
    std::vector<SceneNode> children;

    SceneNode();
    SceneNode(std::shared_ptr<Mesh> mesh, 
              const glm::mat4& transform = glm::mat4(1.0f), 
              bool isEmissive = false, 
              bool isRoad = false, 
              std::shared_ptr<Texture> texture = nullptr);

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

    std::shared_ptr<Texture> asphaltTex;
    std::shared_ptr<Texture> ramenTex;
    std::shared_ptr<Texture> barTex;
    std::shared_ptr<Texture> hotelTex;

    std::shared_ptr<Mesh> createBox(float width, float height, float depth, const glm::vec3& color);
    std::shared_ptr<Mesh> createPlane(float width, float depth, const glm::vec3& color, float tileX = 1.0f, float tileY = 1.0f);
};

#endif
