//
// Created by sneeuwpop on 16-6-19.
//

#include "screens/triangle_test_screen.cpp"
#include "screens/imgui_test_screen.cpp"
#include "util/game.h"

int main(void)
{
    if (!game::init())
        return -1;

    ImGuiTestScreen screen;
    game::setScreen(&screen);

    game::run();

    return 0;
}