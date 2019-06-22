//
// Created by sneeuwpop on 16-6-19.
//

#pragma once

#include <GLFW/glfw3.h>

class Screen
{
public:
    virtual void setup(GLFWwindow* window) {};
    virtual void render(double deltaTime) = 0;
};