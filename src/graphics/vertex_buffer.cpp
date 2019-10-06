//
// Created by sneeuwpop on 1-7-19.
//

#include <iostream>
#include "vertex_buffer.h"
#include "../util/debug/nice_error.h"

VertexBuffer* VertexBuffer::add(SharedMesh mesh)
{
    meshes.push_back(mesh);

    totalNrOfVerts += mesh->nrOfVerts;
    totalNrOfIndicies += mesh->nrOfIndices;

    return this;
}

void glBufferSubData_test(GLenum target, long targetSize)
{
    // Let's make sure our loop of glBufferSubData == our original totalNrOfVerts size
    GLint openglsize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &openglsize);
    if(targetSize != openglsize)
    {
        printf("%lu != %d \n", targetSize, openglsize);
        throw nice_error("Loading a vertex buffer into OpenGL for later rendering did not work. "
                         "Let's make sure our loop of glBufferSubData == our original totalNrOfVerts size");
    }
}

VertexBuffer *VertexBuffer::with()
{
    return new VertexBuffer();
}

void VertexBuffer::uploadSingleMesh(SharedMesh mesh) { with()->add(mesh)->upload(); }
void VertexBuffer::upload()
{
    // Vertex Array
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // Vertex Buffer
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * totalNrOfVerts * VERTSIZE, NULL, GL_STATIC_DRAW);

    // Index Buffer
    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * totalNrOfIndicies, NULL, GL_STATIC_DRAW);

    // Vertex Buffer
    for (const std::weak_ptr<Mesh>& m : meshes)
    {
        if (m.expired())
            throw nice_error("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * mesh->nrOfVerts * VERTSIZE, mesh->vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * mesh->nrOfIndices, mesh->indicies.data());
    }

    glBufferSubData_test(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * totalNrOfVerts * VERTSIZE);
    glBufferSubData_test(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * totalNrOfIndicies);


    // TODO: FOR EACH ATTIBUTE ...
    glVertexAttribPointer(
            0,              // must match the layout in the shader.
            VERTSIZE,       // size
            GL_FLOAT,       // type
            GL_FALSE,       // normalized?
            0,              // stride
            (void*)0        // array buffer offset
    );
    glEnableVertexAttribArray(0);
    // END FOR EACH

}

//VertexBuffer::~VertexBuffer()
//{
//    // TODO test if it is safe to delete the buffer
//    glDeleteVertexArrays(1, &vaoId);
//    glDeleteBuffers(1, &vboId);
//    glDeleteBuffers(1, &iboId);
//}