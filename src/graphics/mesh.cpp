//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"
#include "../util/debug/nice_error.h"

Mesh* Mesh::quad() {
    Mesh* mesh = new Mesh(4, 6);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
            -1,  1,  0,
             1,  1,  0,
             1, -1,  0,     });
    mesh->indicies.insert(mesh->indicies.begin(), {2, 1, 0, 0, 3, 2});
    return mesh;
}

Mesh* Mesh::triangle() {
    Mesh* mesh = new Mesh(3, 3);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
             1, -1,  0,
             0,  1,  0, });
    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2} );
    return mesh;
}

Mesh *Mesh::cube() {
    Mesh* mesh = new Mesh(12*3, 12*3);
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
    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

void Mesh::render() {
    if (!vertexBuffer)
        throw nice_error("You need to upload this mesh to a vertexBuffer first");

    vertexBuffer->bind();

    if (nrOfIndices != 0)
        glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_SHORT, (void*)0);
    else
        glDrawArrays(renderMode, 0, nrOfVerts);
}


