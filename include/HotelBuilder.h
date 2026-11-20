#ifndef HOTEL_BUILDER_H
#define HOTEL_BUILDER_H

#include "Scene.h"
#include <vector>

class HotelBuilder {
public:
    static void buildHotel(std::vector<SceneNode>& rootNodes,
                           std::vector<PointLight>& pointLights);
};

#endif
