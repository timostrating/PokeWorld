//
// Created by sneeuwpop on 03-01-20.
//

#include "water_plane.h"
#include "../graphics/camera.h"

void WaterPlane::render()
{
    waterShader.use();
    waterSystem->bindTexture(waterShader);
    waterNormal.bind(3, waterShader, "u_dudvTexture");

    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * modelMatrix)[0][0]);
    cube->render();

//    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * scale(translate(mat4(1), vec3(0,-2,20)), vec3(20, 2, 1)))[0][0]);
//    glUniform4f(flatShader.uniformLocation("u_color"), 2.0/255.0,  23.0/255.0, 69.0/255.0, 0.9);
//    mesh->render();
//
//    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * rotate(scale(translate(mat4(1), vec3(20,10,0)), vec3(20, 2, 20)), radians(-90.0f), VEC3::Y))[0][0]);
//    glUniform4f(flatShader.uniformLocation("u_color"), 2.0/255.0,  23.0/255.0, 69.0/255.0, 0.9);
//    mesh->render();
}