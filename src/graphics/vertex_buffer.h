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

    GLuint vboId, iboId;

    unsigned int totalNrOfVerts = 0;


    int VERTSIZE = 3;  // TODO: using vertex Attributes


public:
    inline VertexBuffer() {}

    void add(SharedMesh mesh);

    void upload();
};