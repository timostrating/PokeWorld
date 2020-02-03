//
// Created by sneeuwpop on 31-01-20.
//

#include "route.h"
#include "../graphics/camera.h"

void Route::render(float time)
{
    shader.use();
    glUniformMatrix4fv(MVP, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    glUniform1f(u_time, time);

    mesh->render();
}
