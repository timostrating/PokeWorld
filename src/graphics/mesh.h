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

    inline void doVBOstuff()
    {
        // VBO
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(
                0,              // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,              // size
                GL_FLOAT,       // type
                GL_FALSE,       // normalized?
                0,              // stride
                (void*)0        // array buffer offset
        );
        glEnableVertexAttribArray(0);
    }

    inline void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }
};
