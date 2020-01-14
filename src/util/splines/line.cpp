//
// Created by sneeuwpop on 03-01-20.
//

#include "line.h"

using namespace glm;


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

Mesh* Line::wrapMeshAround(std::vector<vec3> *points)
{
    Mesh* mesh = new Mesh(0, 0);

    vec3 oldPos = getPointPosition(0);
    for (int i=0; i<=length; i++)
    {
        vec3 newPos = getPointPosition(i);
        for (int j=0; j<points->size(); j++) {
            vec4 a1 = lerpIndexed(i, 0) * vec4(points->at(j), 1);
            vec4 a2 = lerpIndexed(i, 0) * vec4(points->at((j+1)%points->size()), 1);
            vec4 b1 = lerpIndexed(i, 1) * vec4(points->at(j), 1);
            vec4 b2 = lerpIndexed(i, 1) * vec4(points->at((j+1)%points->size()), 1);

            mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z});
            mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z});
        }
        oldPos = newPos;
    }

    mesh->nrOfVerts = mesh->vertices.size() / 3;
    return mesh;
}