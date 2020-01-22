//
// Created by sneeuwpop on 1-7-19.
//

#include <iostream>
#include "vertex_buffer.h"
#include "../util/debug/nice_error.h"

GLuint VertexBuffer::activeVaoId = 0;

VertexBuffer* VertexBuffer::add(SharedMesh mesh)
{
    if (mesh->vertexBuffer)
        throw nice_error("This mesh is already added to a vertexBuffer");

    mesh->vertexBuffer = this;
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

VertexBuffer *VertexBuffer::with(VertAttributes &vertAttributes)
{
    return new VertexBuffer(vertAttributes);
}

// static
void VertexBuffer::bindDefault()
{
    if (activeVaoId != 0) {
        activeVaoId = 0;
        glBindVertexArray(0);
    }
}

// non-static
void VertexBuffer::bind()
{
    if (activeVaoId != vaoId) {
        activeVaoId = vaoId;
        glBindVertexArray(vaoId);
    }
}

void VertexBuffer::uploadSingleMesh(SharedMesh mesh) { with(mesh->attributes)->add(mesh)->upload(); }
void VertexBuffer::upload()
{
    bind();

    // Vertex Buffer
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalNrOfVerts * vertSize, NULL, GL_STATIC_DRAW);

    // Index Buffer
    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * totalNrOfIndicies, NULL, GL_STATIC_DRAW);

    // Vertex Buffer
    GLuint vertsOffset = 0, indicesOffset = 0;
    for (const std::weak_ptr<Mesh>& m : meshes)
    {
        if (m.expired())
            throw nice_error("Trying to upload a VertBuffer whose Meshes are already destroyed");

        SharedMesh mesh = m.lock();
        GLuint
                meshVertSize = mesh->nrOfVerts * sizeof(float) * vertSize,
                meshIndicesSize = mesh->nrOfIndices * sizeof(unsigned short);

        glBufferSubData(GL_ARRAY_BUFFER, vertsOffset, meshVertSize, mesh->vertices.data());
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, meshIndicesSize, mesh->indicies.data());

        vertsOffset += meshVertSize;
        indicesOffset += meshIndicesSize;
    }

//    glBufferSubData_test(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * totalNrOfVerts * vertSize);
//    glBufferSubData_test(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * totalNrOfIndicies);


    GLint offset = 0;
    for (int i = 0; i < vertAttributes.nrOfAttributes(); i++) {
        VertAttr &attr = vertAttributes.get(i);
        glVertexAttribPointer(
            i,                               // must match the layout in the shader.
            attr.size,                       // size
            GL_FLOAT,                        // type
            attr.normalized,                 // normalized?
            vertSize * sizeof(float),        // stride
            (void *)(uintptr_t)offset        // array buffer offset
        );
        glEnableVertexAttribArray(i);
        offset += attr.size * sizeof(float);
    }

}

//VertexBuffer::~VertexBuffer()
//{
//    // TODO test if it is safe to delete the buffer
//    glDeleteVertexArrays(1, &vaoId);
//    glDeleteBuffers(1, &vboId);
//    glDeleteBuffers(1, &iboId);
//}