//
// Created by sneeuwpop on 29-6-19.
//

#pragma once

#include <string>
#include "mesh.h"


enum VertexAttribute
{
    POSITION = 0,
    NORMAL = 0,
};


class VertexBuffer
{

public:
    VertexBuffer();

    void add(Mesh *mesh);
    void upload();
};