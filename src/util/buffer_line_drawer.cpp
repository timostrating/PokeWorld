//
// Created by sneeuwpop on 20-09-19.
//

#include "buffer_line_drawer.h"


BufferLineDrawer::BufferLineDrawer()
{
    mesh->renderMode = GL_LINES;
}

void BufferLineDrawer::drawLine(const glm::vec3 &from, const glm::vec3 &to)
{
    if (uploaded)
        throw nice_error("drawLine() is not allowed right now after uploading");

    vertexCount += 2;
    mesh->vertices.insert(mesh->vertices.begin(), {
            from.x, from.y, from.z,
            to.x, to.y, to.z
    });
}

void BufferLineDrawer::upload()
{
    mesh->nrOfVerts = vertexCount;
    uploaded = true;
    // Vertex Buffer
    VertexBuffer vertexBuffer = VertexBuffer();
    vertexBuffer.add(mesh);
    vertexBuffer.upload();
}

void BufferLineDrawer::render()
{
    if (!uploaded)
        throw nice_error("Upload before rendering");

    mesh->render();
}
