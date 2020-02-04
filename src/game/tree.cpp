//
// Created by sneeuwpop on 28-01-20.
//

#include "tree.h"
#include "../graphics/camera.h"
#include "../util/debug/gl_debug.h"

const std::string Tree::EXAMPLES[Tree::EXAMPLE_SIZE] = {
        "F[-x+x+x]",
        "F/F[\\\\+x][-x]",
        "F[-x][+Fx]",
        "F[&-x]-F[!+x]",
        "F\\F[///-x][+x]",
        "F/F[\\\\\\+x][-x]",
        "FF[-x][+x]",
        "F/F[\\\\\\-x][+x]",
        "f[/x-x+x]",
        "f[/x\\-x\\+x]",
};

void Tree::debugRender(Gizmos* gizmos)
{
    for (auto l : lines) gizmos->drawLine(l.getPointPosition(0), l.getPointPosition(1), COLOR::WHITE, transform);
//    GL_DEBUG::debugVertexNormals(tree, transform, gizmos);
}

void Tree::render(float time)
{
    treeShader.use();
    glUniformMatrix4fv(treeMVP, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    glUniform1f(tree_time, time);
    tree->render();

    leavesShader.use();
    glUniformMatrix4fv(leavesMVP, 1, GL_FALSE, &(Camera::main->combined)[0][0]);
    glUniform3f(leaves_pos, position.x, position.y, position.z);
    glUniform1f(leaves_time, time);
    leaves->render();

}

