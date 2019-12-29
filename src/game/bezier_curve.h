//
// Created by sneeuwpop on 29-12-19.
//

#pragma once

#include "../util/math/math.h"
#include "../util/debug/gizmos.h"

class BezierCurve {
    Gizmos gizmos;
    std::vector<vec3> points;


public:
    BezierCurve(vec3 a, vec3 b, vec3 c, vec3 d)
    {
        points.insert(points.begin(), {a,b,c,d});

        if (points.size() < 4 || points.size() % 2 != 0)
            nice_error("BezierCurve points incorrect");
    }

    void debugDraw()
    {
        for (auto p : points)
            gizmos.drawCube(p, 0.1, COLOR::WHITE);

        for (int i=0; i<points.size()-1; i++)
            gizmos.drawLine(points[i], points[i+1], COLOR::WHITE);
    }

    vec3 getPoint(float t);
    vec3 getDerivative(float t);
};
