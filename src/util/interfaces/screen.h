//
// Created by sneeuwpop on 16-6-19.
//

#pragma once

#include <GLFW/glfw3.h>
#include "resizeable.h"


class Screen : Resizeable
{
public:
    virtual void setup(GLFWwindow* window) {};
    virtual void render(double deltaTime) = 0;
    virtual void resize(int width, int height) {};
};