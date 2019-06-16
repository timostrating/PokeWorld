//
// Created by sneeuwpop on 16-6-19.
//

#ifndef POKE_GAME_H
#define POKE_GAME_H

#include <GLFW/glfw3.h>
#include "../screens/screen.h"

namespace game
{
    bool init();
    void run();
    void setScreen(Screen *screen);
}


#endif //POKE_GAME_H
