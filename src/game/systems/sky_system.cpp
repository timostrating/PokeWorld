//
// Created by sneeuwpop on 10-01-20.
//

#include "sky_system.h"
#include "../../util/math/math.h"
#include "../../graphics/camera.h"

void SkySystem::update() {
    skyShader.use();
    glUniformMatrix4fv(MVPsky, 1, GL_FALSE, &(Camera::main->combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 400.0f * VEC3::ONE))[0][0]);
    skybox->render();
}
