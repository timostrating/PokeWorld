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
    glUniformMatrix4fv(MVPsky, 1, GL_FALSE, &(Camera::main->combined * rotate(scale(translate(mat4(1.0f), vec3(0, -70, 0)), 400.0f * VEC3::ONE), radians(rotation), VEC3::Y))[0][0]);
    skybox->render();
}
