#ifndef SHOP_BUILDER_H
#define SHOP_BUILDER_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Scene.h"

class ShopBuilder {
public:
    static void buildRamenShop(std::vector<SceneNode>& rootNodes,
                               std::vector<PointLight>& pointLights);

    static void buildBarFacade(std::vector<SceneNode>& rootNodes,
                               std::vector<PointLight>& pointLights);

    static void buildHotel(std::vector<SceneNode>& rootNodes,
                           std::vector<PointLight>& pointLights);

    static void buildDetailedVendingMachine(std::vector<SceneNode>& rootNodes,
                                           const glm::vec3& position,
                                           float rotationYDeg,
                                           const glm::vec3& bodyColor,
                                           bool addSpotlight = false,
                                           std::vector<PointLight>* pointLights = nullptr);
};

#endif
