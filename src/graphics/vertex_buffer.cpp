//
// Created by sneeuwpop on 29-6-19.
//

#include "vertex_buffer.h"


VertexBuffer::VertexBuffer()
{

}

void VertexBuffer::add(Mesh *mesh)
{
    // VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertices.size(), mesh->vertices.data(), mesh->drawType);

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

void VertexBuffer::upload()
{

}