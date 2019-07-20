//
// Created by sneeuwpop on 1-7-19.
//

#include <iostream>
#include "vertex_buffer.h"
#include "../util/nice_error.h"

void VertexBuffer::add(SharedMesh mesh)
{
    meshes.push_back(mesh);

    totalNrOfVerts += mesh->nrOfVerts;
}

void VertexBuffer::upload()
{
    // VBO
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * totalNrOfVerts * VERTSIZE, NULL, GL_STATIC_DRAW);

    for (const std::weak_ptr<Mesh>& m : meshes)
    {
        if (m.expired())
            throw nice_error("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * mesh->nrOfVerts * VERTSIZE, mesh->vertices.data());
    }


    // Let's make sure our loop of glBufferSubData == our original totalNrOfVerts size
    GLint size = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if(sizeof(GL_FLOAT) * totalNrOfVerts * VERTSIZE != size)
    {
        glDeleteBuffers(1, &vboId);
        printf("%lu != %d \n", sizeof(GL_FLOAT) * totalNrOfVerts, size);
        throw nice_error("Loading a vertex buffer into OpenGL for later rendering did not work. "
                         "Let's make sure our loop of glBufferSubData == our original totalNrOfVerts size");
    }


    // FOR EACH ATTIBUTE ...
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


//        glVertexAttribPointer(
//                0,              // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                3,              // size
//                GL_FLOAT,       // type
//                GL_FALSE,       // normalized?
//                0,              // stride
//                (void*)0        // array buffer offset
//        );
//        glEnableVertexAttribArray(0);
//
//        glGenBuffers(1, &iboId);    // create IndexBuffer
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nrOfIndices, NULL, GL_STATIC_DRAW);
}

