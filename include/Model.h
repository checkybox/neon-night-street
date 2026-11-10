#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"

class Model {
public:
    static std::shared_ptr<Mesh> loadOBJ(const std::string& filepath, const glm::vec3& defaultColor = glm::vec3(0.8f));
};

#endif
