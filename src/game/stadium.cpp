//
// Created by sneeuwpop on 13-01-20.
//

#include "stadium.h"
#include "../graphics/camera.h"

void Stadium::render()
{
    shader.use();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    mesh->render();
}

void Stadium::onClick()
{
    // TODO: fireworks
}
