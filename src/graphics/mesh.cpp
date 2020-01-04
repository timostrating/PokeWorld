//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"
#include "../util/debug/nice_error.h"
#include "../util/math/math.h"

using namespace MATH;

Mesh* Mesh::quad()
{
    Mesh* mesh = new Mesh(4, 6);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
            -1,  1,  0,
             1,  1,  0,
             1, -1,  0,     });
    mesh->indicies.insert(mesh->indicies.begin(), {2, 1, 0, 0, 3, 2});
    return mesh;
}

Mesh* Mesh::triangle()
{
    Mesh* mesh = new Mesh(3, 3);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
             1, -1,  0,
             0,  1,  0, });
    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2} );
    return mesh;
}

Mesh *Mesh::cube()
{
    Mesh* mesh = new Mesh(12*3, 0);
    mesh->vertices.insert(mesh->vertices.begin(), { // TODO optimize
            //  x,    y,    z       x,    y,    z       x,    y,    z
            -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,   1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,   1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f
    });
//    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

Mesh *Mesh::skybox()
{
    Mesh* mesh = new Mesh(12*3, 0);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
    });
//    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

Mesh *Mesh::sphere()
{
    Mesh* mesh = new Mesh(0, 0);
    float step = 2*PI / 10;

    for (float phi=-0.5*PI; phi < 0.5*PI - 0.01; phi+=step) // [-90,90]
    {
        bool firstPhiLoop = abs(-0.5 * PI - phi) > 0.01,
             lastPhiLoop  = phi + step < 0.5*PI - 0.01;

        for (float theta=0; theta < 2*PI - 0.01; theta+=step) // [0,360]
        {
            vec3 a1 = vec3(cos(phi) * cos(theta), sin(phi), cos(phi)*sin(theta));
            vec3 a2 = vec3(cos(phi) * cos(theta+step), sin(phi), cos(phi)*sin(theta+step));
            vec3 b1 = vec3(cos(phi+step) * cos(theta), sin(phi+step), cos(phi+step)*sin(theta));
            vec3 b2 = vec3(cos(phi+step) * cos(theta+step), sin(phi+step), cos(phi+step)*sin(theta+step));

            if (firstPhiLoop)
                mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z});
            if (lastPhiLoop)
                mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z});
        }
    }
    mesh->nrOfVerts = mesh->vertices.size() / 3;
    return mesh;
}

void Mesh::render()
{
    if (!vertexBuffer)
        throw nice_error("You need to upload this mesh to a vertexBuffer first");

    vertexBuffer->bind();

    if (nrOfIndices != 0)
        glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_SHORT, (void*)0);
    else
        glDrawArrays(renderMode, 0, nrOfVerts);
}


