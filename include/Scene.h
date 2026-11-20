#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Light.h"
#include "Texture.h"
#include "Model.h"

struct SceneNode {
    std::shared_ptr<Mesh> mesh;
    glm::mat4 localTransform;
    bool isEmissive;
    bool isRoad;
    bool isFlickering;
    std::shared_ptr<Texture> texture;
    std::vector<SceneNode> children;

    SceneNode();
    SceneNode(std::shared_ptr<Mesh> mesh, 
              const glm::mat4& transform = glm::mat4(1.0f), 
              bool isEmissive = false, 
              bool isRoad = false, 
              std::shared_ptr<Texture> texture = nullptr,
              bool isFlickering = false);

    void addChild(const SceneNode& child);
    void draw(const Shader& shader, const glm::mat4& parentTransform = glm::mat4(1.0f)) const;
};

class Scene {
public:
    Scene();
    ~Scene();

    void init();
    void render(const Shader& shader, const glm::vec3& camPos, const glm::mat4& view, const glm::mat4& projection, float time = 0.0f, bool fogEnabled = true);
    void setupLights(const Shader& shader);
    float getNeonFlicker() const { return currentFlicker; }

private:
    std::vector<SceneNode> rootNodes;
    std::vector<SceneNode> emissiveNodes;
    std::vector<SceneNode> skyscraperNodes;
    std::vector<SceneNode> decalNodes;
    std::unique_ptr<Shader> emissiveShader;
    std::unique_ptr<Shader> skyscraperShader;
    std::unique_ptr<Shader> decalShader;
    DirLight dirLight;
    std::vector<PointLight> pointLights;

    int barLightIndex;
    glm::vec3 baseBarDiffuse;
    glm::vec3 baseBarSpecular;
    glm::vec3 baseBarAmbient;
    float currentFlicker;

    std::vector<glm::vec3> basePointDiffuses;
    std::vector<glm::vec3> basePointSpeculars;
    std::vector<glm::vec3> basePointAmbients;

    std::shared_ptr<Texture> asphaltTex;
    std::shared_ptr<Texture> moonTex;

    void partitionNodes(const std::vector<SceneNode>& nodes, const glm::mat4& parentTransform,
                        std::vector<SceneNode>& opaque, std::vector<SceneNode>& emissive);

    std::shared_ptr<Mesh> createBox(float width, float height, float depth, const glm::vec3& color);
    std::shared_ptr<Mesh> createPlane(float width, float depth, const glm::vec3& color, float tileX = 1.0f, float tileY = 1.0f);
    std::shared_ptr<Mesh> createCylinder(float radius, float height, int segments, const glm::vec3& color, bool capEnds = true);
    std::shared_ptr<Mesh> createCornerCurb(float centerX, float centerZ, float radius, float startAngleRad, float endAngleRad, float innerX, float innerZ, float height, int segments, const glm::vec3& color);
};

#endif
