//
// Created by sneeuwpop on 10-01-20.
//

#include "sky.h"
#include "../util/math/math.h"
#include "../graphics/camera.h"

void Sky::render()
{
    float rotation = (skySystem == nullptr) ? 0 : skySystem->rotation;

    skyShader.use();
    mat4 fixedPosition = Camera::main->projection * mat4(
            Camera::main->view[0][0], Camera::main->view[0][1], Camera::main->view[0][2], Camera::main->view[0][3],
            Camera::main->view[1][0], Camera::main->view[1][1], Camera::main->view[1][2], Camera::main->view[1][3],
            Camera::main->view[2][0], Camera::main->view[2][1], Camera::main->view[2][2], Camera::main->view[2][3],
                                   0,                        0,                        0, Camera::main->view[3][3]);
    glUniformMatrix4fv(MVPsky, 1, GL_FALSE, &(fixedPosition * rotate(transform, radians(rotation), VEC3::Y))[0][0]);
    skybox->render();
}
