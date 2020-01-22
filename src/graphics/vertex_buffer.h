//
// Created by sneeuwpop on 29-6-19.
//

#pragma once

#include <string>
#include <vector>
#include "mesh.h"
#include "vert_attributes.h"


class Mesh;
typedef std::shared_ptr<Mesh> SharedMesh;

class VertexBuffer
{

private:
    std::vector<std::weak_ptr<Mesh>> meshes = {};

    GLuint vaoId, vboId = 0, iboId = 0;

    unsigned int totalNrOfVerts = 0;
    unsigned int totalNrOfIndicies = 0;
    unsigned int vertSize = 0;

    VertAttributes vertAttributes;


public:
    // try not to use this. It is more efficient to put more meshes (with the same VertAttributes) in 1 VertBuffer
    static void uploadSingleMesh(SharedMesh mesh);

    static VertexBuffer* with(VertAttributes &vertAttributes);
    VertexBuffer(VertAttributes &vertAttributes) : vertAttributes(vertAttributes), vertSize(vertAttributes.getVertSize())
    {
        glGenVertexArrays(1, &vaoId);
    }
//    ~VertexBuffer(); // TODO: add a destructor

    VertexBuffer* add(SharedMesh mesh);

    void bind();
    void upload();

    static GLuint activeVaoId;
    static void bindDefault();
};