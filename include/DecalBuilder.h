#ifndef DECALBUILDER_H
#define DECALBUILDER_H

#include <vector>
#include <memory>
#include "Scene.h"

class DecalBuilder {
public:
    static void buildDecals(std::vector<SceneNode>& rootNodes,
                            std::vector<SceneNode>& decalNodes,
                            std::vector<PointLight>& pointLights);
};

#endif
