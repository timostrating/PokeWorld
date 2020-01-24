//
// Created by sneeuwpop on 20-6-19.
//

#pragma once

#include "../../graphics/mesh.h"
#include "gizmos.h"

namespace GL_DEBUG
{
    void printContext();
    void enableGLDebug();
    void debugVertexNormals(SharedMesh mesh, mat4 transform, Gizmos *gizmos);
}