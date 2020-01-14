//
// Created by sneeuwpop on 16-6-19.
//

#pragma once

#include <GLFW/glfw3.h>
#include "interfaces/screen.h"

namespace GAME
{
    static unsigned int g_width = 800;
    static unsigned int g_height = 800;

    bool init();
    void run();
    void setScreen(Screen *screen);
}