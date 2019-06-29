//
// Created by sneeuwpop on 28-6-19.
//

#pragma once

#include <glad/glad.h>
#include <vector>


class Mesh
{

public:
    std::vector<float> vertices;
    GLenum renderMode = GL_TRIANGLES;
    GLenum drawType = GL_STATIC_DRAW;

    inline void render()
    {
        glDrawArrays(renderMode, 0, vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }

};
