#ifndef ALLEY_BUILDER_H
#define ALLEY_BUILDER_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Light.h"

class AlleyBuilder {
public:
    static void buildAlleys(std::vector<SceneNode>& rootNodes,
                            std::vector<PointLight>& pointLights);

    static void buildDeadEnds(std::vector<SceneNode>& rootNodes,
                              std::vector<PointLight>& pointLights);
};

#endif
