//
// Created by sneeuwpop on 03-01-20.
//

#include "water_plane.h"
#include "../graphics/camera.h"

void WaterPlane::render()
{
    waterShader.use();
    waterSystem->bindTexture(waterShader);
//    reflectionTexture.bind(0, waterShader, "u_reflectionTexture");

    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * modelMatrix)[0][0]);
    mesh->render();
}

void WaterPlane::onClick()
{
    glUniform4f(waterShader.uniformLocation("u_color"), random(0,1), random(0,1), random(0,1), 1);
}