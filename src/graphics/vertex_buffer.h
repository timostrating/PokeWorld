//
// Created by sneeuwpop on 29-6-19.
//

#pragma once

#include <string>
#include <vector>
#include "mesh.h"


//enum VertexAttribute
//{
//    POSITION = 0,
//    NORMAL = 0,
//};


class VertexBuffer
{

private:
    std::vector<std::weak_ptr<Mesh>> meshes = {};

    GLuint vaoId, vboId, iboId;

    unsigned int totalNrOfVerts = 0;
    unsigned int totalNrOfIndicies = 0;

    bool dynamic = false;
    int VERTSIZE = 3;  // TODO: using vertex Attributes

public:
    // try not to use this. It is more efficient to put more meshes (with the same VertAttributes) in 1 VertBuffer
    static void uploadSingleMesh(SharedMesh mesh);

    explicit VertexBuffer(bool dynamic = false);
    ~VertexBuffer();

    VertexBuffer* add(SharedMesh mesh);

    void upload();
};