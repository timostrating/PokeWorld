//
// Created by sneeuwpop on 03-01-20.
//

#include "water_plane.h"
#include "../graphics/camera.h"

void WaterPlane::render(float time)
{
    waterShader.use();
    waterSystem->bindTexture(waterShader);

    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * modelMatrix)[0][0]);
    glUniform1f(u_time, time);
    cube->render();
}