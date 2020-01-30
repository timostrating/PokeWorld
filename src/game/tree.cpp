//
// Created by sneeuwpop on 28-01-20.
//

#include "tree.h"
#include "../graphics/camera.h"

void Tree::debugRender()
{
    for (auto l : lines) gizmos.drawLine(l.getPointPosition(0), l.getPointPosition(1), COLOR::WHITE);
}

void Tree::render(float time)
{
    flatShader.use();
    glUniformMatrix4fv(MVP, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    glUniform1f(u_time, time);
    tree->render();
//        GL_DEBUG::debugVertexNormals(tree, transform, &gizmos);
}

