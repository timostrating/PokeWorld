//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"
#include "../util/debug/nice_error.h"

void Mesh::render() {
    if (!vertexBuffer)
        throw nice_error("You need to upload this mesh to a vertexBuffer first");

    vertexBuffer->bind();

    glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_SHORT, (void*)0);
//    glDrawArrays(renderMode, 0, nrOfVerts);
}