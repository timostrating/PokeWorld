//
// Created by sneeuwpop on 28-6-19.
//

#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include "vertex_buffer.h"

class VertexBuffer;
class Mesh;
typedef std::shared_ptr<Mesh> SharedMesh;

class Mesh
{
public:
    static Mesh* quad();
    static Mesh* triangle();
    static Mesh* cube();
    static Mesh* skybox();
    static Mesh* sphere();

    Mesh(unsigned int nrOfVerts, unsigned int nrOfIndices) : nrOfVerts(nrOfVerts), nrOfIndices(nrOfIndices) {};

    std::vector<float> vertices = {};
    std::vector<unsigned short> indicies = {};

    VertexBuffer *vertexBuffer = nullptr;

    GLenum renderMode = GL_TRIANGLES;

    unsigned int nrOfVerts = 0;
    unsigned int nrOfIndices = 0;

    void render();

};
