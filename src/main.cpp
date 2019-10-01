//
// Created by sneeuwpop on 16-6-19.
//

#include "screens/triangle_test_screen.cpp"
#include "screens/imgui_test_screen.cpp"
#include "screens/model_test_screen.cpp"
#include "screens/model_indices_test_screen.cpp"
#include "screens/quad_model_test_screen.cpp"
#include "screens/tree_screen.cpp"
#include "util/interfaces/game.h"

#define CATCH_CONFIG_RUNNER
#include "../external/catch.hpp"

int main(int argc, char* argv[])
{
    int result = Catch::Session().run( argc, argv );
    if (!GAME::init())
        return -1;

    TreeScreen screen;
    GAME::setScreen(&screen);

    GAME::run();

    return result;
}