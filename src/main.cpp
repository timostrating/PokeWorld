//
// Created by sneeuwpop on 16-6-19.
//

#include "screens/triangle_test_screen.cpp"
#include "screens/imgui_test_screen.cpp"
#include "screens/model_test_screen.cpp"
#include "screens/model_indices_test_screen.cpp"
#include "screens/quad_model_test_screen.cpp"
#include "screens/tree_screen.cpp"
#include "screens/everyday_screen.cpp"
#include "screens/main_screen.cpp"
#include "util/game.h"

#include "util/external/unit_test.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "util/external/stb_image.hpp"

int main(int argc, char* argv[])
{

#ifndef __EMSCRIPTEN__
    RUN_ALL_TESTS();
#endif

    if (!GAME::init())
        return -1;

    MainScreen screen;
    GAME::setScreen(&screen);

    GAME::run();

    return 0;
}