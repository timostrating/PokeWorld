//
// Created by sneeuwpop on 03-01-20.
//

#include "line.h"

mat4 lerp(const mat4 a, const mat4 b, const float t)
{
    mat4 value = mat4(1);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            value[i][j] = (1-t) * a[i][j] + t*b[i][j];
    return value;
}

vec3 Line::getPointPosition(int i)
{
    i = i % length;
    return vec3(lineparts[i][3][0], lineparts[i][3][1], lineparts[i][3][2]);
}

mat4 Line::lerpIndexed(const int i, const float t)
{
    return lerp(lineparts[i%length], lineparts[(i+1)%length], t);
}

void Line::debugDraw(Gizmos *gizmos)
{
    for (int i=0; i<length; i++)
    {
        gizmos->drawCube(getPointPosition(i), 0.01f, COLOR::WHITE);
        gizmos->drawLine(getPointPosition(i), getPointPosition(i+1), vec4(.8, .8, .8, 1));
    }
}
