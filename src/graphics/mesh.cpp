//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"

void Mesh::render() {
    if (useIndicies)
        glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_SHORT, (void*)0);
    else
        glDrawArrays(renderMode, 0, nrOfVerts);
}