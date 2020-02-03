//
// Created by sneeuwpop on 03-01-20.
//

#pragma once

#include <vector>
#include <ext.hpp>
#include "../debug/gizmos.h"

using namespace glm;
using namespace MATH;

class Line
{

public:

    std::vector<mat4> lineparts;
    int length = 0;

    Line(std::vector<mat4> lines) : lineparts(lines) { length = lineparts.size(); }

    vec3 getPointPosition(int i);
    mat4 lerpIndexed(const int i, const float t);

    void debugDraw(Gizmos* gizmos);

    Mesh* wrapMeshAround(std::vector<vec3> *points, bool positionAndNormals = false, bool smooth = false, bool loop = true);
};


