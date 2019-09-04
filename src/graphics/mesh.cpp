//
// Created by sneeuwpop on 20-7-19.
//

#include "mesh.h"

Mesh::Mesh(const unsigned int nrOfVerts) {
    this->nrOfVerts = nrOfVerts;
}

void Mesh::render() {
    glDrawArrays(renderMode, 0, nrOfVerts); // Starting from vertex 0; 3 vertices total -> 1 triangle;
}

