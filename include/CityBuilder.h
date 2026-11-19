#ifndef CITY_BUILDER_H
#define CITY_BUILDER_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Scene.h"

class CityBuilder {
public:
    static void buildSkyscrapers(std::vector<SceneNode>& rootNodes, std::shared_ptr<Texture> moonTex = nullptr);
    static void buildSkyElements(std::vector<SceneNode>& rootNodes, std::shared_ptr<Texture> moonTex = nullptr);
    static void buildExtendedStreet(std::vector<SceneNode>& rootNodes,
                                   std::vector<PointLight>& pointLights,
                                   std::shared_ptr<Texture> asphaltTex);

    static std::shared_ptr<Mesh> createBox(float width, float height, float depth, const glm::vec3& color);
    static std::shared_ptr<Mesh> createPlane(float width, float depth, const glm::vec3& color, float tileX = 1.0f, float tileY = 1.0f);

    static void appendBox(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                          float width, float height, float depth, const glm::vec3& color,
                          const glm::mat4& tr = glm::mat4(1.0f));
    static void appendPlane(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                           float width, float depth, const glm::vec3& color,
                           const glm::mat4& tr = glm::mat4(1.0f), float tileX = 1.0f, float tileY = 1.0f);
};

#endif
