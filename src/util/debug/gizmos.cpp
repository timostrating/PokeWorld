//
// Created by sneeuwpop on 30-09-19.
//

#include "gizmos.h"
#include "../../graphics/vertex_buffer.h"
#include "../../graphics/mesh.h"

void Gizmos::drawLine(const vec3 &from, const vec3 &to) {
    SharedMesh mesh = SharedMesh(new Mesh(2, 0, false));
    mesh->renderMode = GL_LINE_STRIP;
    mesh->vertices.insert(mesh->vertices.begin(), {
            from.x, from.y, from.z,
            to.x, to.y, to.z,
    });
    VertexBuffer::uploadSingleMesh(mesh);
    mesh->render();
}
