//
// Created by sneeuwpop on 16-6-19.
//

#include "screens/triangle_test_screen.cpp"
#include "screens/imgui_test_screen.cpp"
#include "screens/model_test_screen.cpp"
#include "screens/model_indices_test_screen.cpp"
#include "util/game.h"

int main(void)
{
    if (!GAME::init())
        return -1;

    ModelIndicesTestScreen screen;
    GAME::setScreen(&screen);

    GAME::run();

    return 0;
}