#ifndef STREET_PROPS_H
#define STREET_PROPS_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Scene.h"

class StreetProps {
public:
    static void buildConstructionBarriers(std::vector<SceneNode>& rootNodes, std::vector<PointLight>& pointLights);
    static void buildAlleyClutter(std::vector<SceneNode>& rootNodes);
    static void buildBuildingEntrances(std::vector<SceneNode>& rootNodes, std::vector<PointLight>& pointLights);
    static void buildUpperWindows(std::vector<SceneNode>& rootNodes);
};

#endif
