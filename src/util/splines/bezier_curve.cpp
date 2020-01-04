//
// Created by sneeuwpop on 29-12-19.
//

#include "bezier_curve.h"

void BezierCurve::debugDraw()
{
    for (auto p : points)
        gizmos.drawCube(p, 0.1, COLOR::WHITE);

    for (int i=0; i<points.size()-1; i++)
        gizmos.drawLine(points[i], points[i+1], COLOR::WHITE);
}


vec3 BezierCurve::getPoint(float t)
{
    float one_t = 1-t;
    vec3 p0 = points[0], p1 = points[1], p2 = points[2], p3 = points[3];
    return powf(one_t,3)*p0 + 3*powf(one_t, 2)*t*p1 + 3*one_t*powf(t,2)*p2 + powf(t,3)*p3;
}

vec3 BezierCurve::getDerivative(float t)
{
    float one_t = 1-t;
    vec3 p0 = points[0], p1 = points[1], p2 = points[2], p3 = points[3];
    return 3*powf(one_t,2)*(p1-p0) + 6*one_t*t*(p2-p1) + 3*powf(t,2)*(p3-p2);
}
