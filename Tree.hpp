#pragma once
#include <glm/glm.hpp>
#include "Model3D.hpp"

class Tree {
public:
    Tree(glm::vec3 pos, gps::Model3D* model);

    glm::vec3 position;
    gps::Model3D* model;
};
