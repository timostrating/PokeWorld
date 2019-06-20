//
// Created by sneeuwpop on 16-6-19.
//

#ifndef POKE_SCREEN_H
#define POKE_SCREEN_H

#include <GLFW/glfw3.h>

class Screen
{
public:
    virtual void setup(GLFWwindow* window) = 0;
    virtual void render(double deltaTime) = 0;
};

#endif //POKE_SCREEN_H
