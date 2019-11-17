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

    static void cursorPositionCallback(GLFWwindow* _, double xpos, double ypos)
    {
        mousePosX = xpos;
        mousePosY = ypos;
    }

    void scrollCallback(GLFWwindow* _, double xoffset, double yoffset)
    {
        nextScroll = yoffset;
    }

    void setup(GLFWwindow *window)
    {
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetScrollCallback(window, scrollCallback);
    }

    void lateUpdate()
    {
        scroll = 0;
        scroll = nextScroll;
        nextScroll = 0;
    }

}