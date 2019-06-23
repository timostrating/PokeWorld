//
// Created by sneeuwpop on 23-6-19.
//

#pragma once

#include <GLFW/glfw3.h>


namespace INPUT::KEYBOARD
{
    void setup(GLFWwindow *window);
    bool pressed(int keyCode);
    bool anyKeyEverPressed();
}