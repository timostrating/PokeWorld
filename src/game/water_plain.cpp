//
// Created by sneeuwpop on 03-01-20.
//

#include "water_plain.h"
#include "../graphics/camera.h"

void WaterPlain::render()
{
    waterShader.use();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * modelMatrix)[0][0]);
    mesh->render();
}

