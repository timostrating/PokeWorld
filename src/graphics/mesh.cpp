//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"

Mesh::Mesh(const unsigned int nrOfVerts, const unsigned int nrOfIndices) {
    this->nrOfVerts = nrOfVerts;
    this->nrOfIndices = nrOfIndices;
}

void Mesh::render() {
//    glDrawArrays(renderMode, 0, nrOfVerts); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_SHORT, (void*)0);
}

