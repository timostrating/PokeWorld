//
// Created by sneeuwpop on 16-6-19.
//

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "game.h"
#include "gl_debug.cpp"


namespace game {
    GLFWwindow *g_window;
    Screen *screen;

    bool init()
    {
        /* Initialize the library */
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        glfwWindowHint(GLFW_RESIZABLE , 1);
        /* Create a windowed mode window and its OpenGL context */
        g_window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
        if (!g_window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(g_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL context\n";
            return -1;
        }

        gl_debug::enableGLDebug();
        gl_debug::printContext();

        return true;
    }


    double prevTime = 0;
    int framesInSecond = 0;
    double timeTowardsSecond = 999;

    void tick()
    {
        double curTime = glfwGetTime();
        double deltaTime = curTime - prevTime;

        framesInSecond++;

        if ((timeTowardsSecond += deltaTime) > 1)
        {
            std::string fps = std::to_string(framesInSecond) + " fps";
            glfwSetWindowTitle(g_window, fps.c_str());
            framesInSecond = 0;
            timeTowardsSecond -= 1;
        }

        if (screen)
            screen->render(deltaTime);

        glfwSwapBuffers(g_window);
        glfwPollEvents();

        prevTime = curTime;
    }


    void run()
    {
        prevTime = glfwGetTime();

#ifdef __EMSCRIPTEN__ // Main loop
        emscripten_set_main_loop(tick, 0, 1);
#else
        while (!glfwWindowShouldClose(g_window)) { tick(); }
#endif
    }

    void setScreen(Screen *newScreen)
    {
        screen = newScreen;
    }
}