//
// Created by sneeuwpop on 29-12-19.
//

#pragma once

#include "../math/math.h"
#include "../debug/gizmos.h"

class BezierCurve {
    Gizmos gizmos;
    std::vector<vec3> points;


public:
    BezierCurve(std::vector<vec3> points) : points(points)
    {
        if (points.size() < 4 || points.size() % 2 != 0)
            nice_error("BezierCurve points incorrect");
    }

    void debugDraw();

    vec3 getPoint(float t);
    vec3 getDerivative(float t);
};
