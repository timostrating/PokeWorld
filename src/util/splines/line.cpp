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

vec3 calculateNormal(vec3 a, vec3 b, vec3 c) {

    vec3 u = b-a;
    vec3 v = c-a;

//    vec3 normal = vec3(
//        u.y * v.z -
//    );

    return normalize(cross(u, v));
}

Mesh* Line::wrapMeshAround(std::vector<vec3> *points, bool positionAndNormals, bool smooth)
{
    Mesh* mesh = (positionAndNormals)? new Mesh(0, 0, VA_POSITION_NORMAL) : new Mesh(0, 0, VA_POSITION);

    for (int i=0; i<=length; i++)
    {
        vec3 mida = getPointPosition(i);
        vec3 midb = getPointPosition(i+1);
        for (int j=0; j<points->size(); j++)
        {
            vec4 a1 = lerpIndexed(i, 0) * vec4(points->at(j), 1);
            vec4 a2 = lerpIndexed(i, 0) * vec4(points->at((j+1)%points->size()), 1);
            vec4 b1 = lerpIndexed(i, 1) * vec4(points->at(j), 1);
            vec4 b2 = lerpIndexed(i, 1) * vec4(points->at((j+1)%points->size()), 1);

            if (positionAndNormals == false) {
                //                                           POSITION        POSITION        POSITION
                mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z});
                mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z});

            } else { // add normals too
                if (smooth) // smooth normal based on the displacement of the line
                {
                    vec3 na1 = normalize(vec3(a1)-mida),  na2 = normalize(vec3(a2)-mida),  nb1 = normalize(vec3(b1)-midb),  nb2 = normalize(vec3(b2)-midb);
                    //                                           POSITION        NORMAL              POSITION        NORMAL              POSITION        NORMAL
                    mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, na1.x,na1.y,na1.z,  a2.x,a2.y,a2.z, na2.x,na2.y,na2.z,  b1.x,b1.y,b1.z, nb1.x,nb1.y,nb1.z,  });
                    mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, nb2.x,nb2.y,nb2.z,  b1.x,b1.y,b1.z, nb1.x,nb1.y,nb1.z,  a2.x,a2.y,a2.z, na2.x,na2.y,na2.z,  });

                } else { // normal of the triangle is the normal of every vertex.

                    vec3 n1 = calculateNormal(a1, a2, b1);
                    vec3 n2 = calculateNormal(b2, b1, a2);

                    //                                           POSITION        NORMAL              POSITION        NORMAL              POSITION        NORMAL
                    mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, n1.x,n1.y,n1.z,  a2.x,a2.y,a2.z, n1.x,n1.y,n1.z,  b1.x,b1.y,b1.z, n1.x,n1.y,n1.z,  });
                    mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, n2.x,n2.y,n2.z,  b1.x,b1.y,b1.z, n2.x,n2.y,n2.z,  a2.x,a2.y,a2.z, n2.x,n2.y,n2.z,  });
                }
            }
        }
    }

    mesh->nrOfVerts = mesh->vertices.size() /  mesh->attributes.getVertSize();
    return mesh;
}