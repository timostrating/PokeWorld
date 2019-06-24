//
// Created by sneeuwpop on 24-6-19.
//

#pragma once

#include <GLFW/glfw3.h>

namespace INPUT::MOUSE
{
    void setup(GLFWwindow *window);
    void lateUpdate();

    float getScrollDelta();
};