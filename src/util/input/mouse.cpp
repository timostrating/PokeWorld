//
// Created by sneeuwpop on 24-6-19.
//

#include "mouse.h"
#include <iostream>


namespace INPUT::MOUSE
{
    float scroll = 0; // normal mouse only scroll on the Y
    float nextScroll = 0; // TODO: smooth step
    float getScrollDelta() { return scroll; } // TODO: fancy macro

    double mousePosX = 0.0;
    double mousePosY = 0.0;

    int mouseButtons[3] = {GLFW_RELEASE};


    static void cursorPositionCallback(GLFWwindow* _, double xpos, double ypos)
    {
        mousePosX = xpos;
        mousePosY = ypos;
    }

    void scrollCallback(GLFWwindow* _, double xoffset, double yoffset)
    {
        nextScroll = yoffset;
    }

    void mouseCallback(GLFWwindow* _, int button, int action, int mods)
    {
        if (button > 3)
            return;

        mouseButtons[button] = action;
    }

    bool leftClick() { return mouseButtons[GLFW_MOUSE_BUTTON_LEFT];}
    bool rightClick() { return mouseButtons[GLFW_MOUSE_BUTTON_RIGHT];}

    void setup(GLFWwindow *window)
    {
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetMouseButtonCallback(window, mouseCallback);

    }

    void lateUpdate()
    {
        scroll = 0;
        scroll = nextScroll;
        nextScroll = 0;
    }

    int getMousePosX() { return static_cast<int>(mousePosX); }
    int getMousePosY() { return static_cast<int>(mousePosY); }

}