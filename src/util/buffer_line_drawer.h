//
// Created by sneeuwpop on 20-09-19.
//

#pragma once

#include <glm/vec3.hpp>
#include "../graphics/mesh.h"
#include "nice_error.h"
#include "../graphics/vertex_buffer.h"

class BufferLineDrawer
{
private:
    SharedMesh mesh = SharedMesh(new Mesh(-1, 0, false));
    int vertexCount = 0;
    bool uploaded = false;

public:
    BufferLineDrawer();

    void drawLine(const glm::vec3 &from, const glm::vec3 &to);
    void upload();
    void render();
};