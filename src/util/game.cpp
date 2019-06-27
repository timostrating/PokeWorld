//
// Created by sneeuwpop on 16-6-19.
//

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../util/interfaces/screen.h"
#include "gl_debug.h"
#include "input/keyboard.h"
#include "input/mouse.h"


namespace GAME {
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
        g_window = glfwCreateWindow(1200, 800, "Hello World", NULL, NULL);
        if (!g_window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(g_window);
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL context\n";
            return -1;
        }


        INPUT::KEYBOARD::setup(g_window);
        INPUT::MOUSE::setup(g_window);

//        GLDEBUG::enableGLDebug();
        GLDEBUG::printContext();

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

        INPUT::KEYBOARD::lateUpdate();
        INPUT::MOUSE::lateUpdate();

        prevTime = curTime;
    }

    void framebuffer_size_callback(GLFWwindow* _, int width, int height)
    {
//        std::cout << "framebuffer_size_callback " << width << " " << height << "\n";
        glViewport(0, 0, width, height);
        if (screen)
            screen->resize(width, height);
    }

    void run()
    {
        prevTime = glfwGetTime();

        glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);  // https://www.glfw.org/docs/latest/window_guide.html#window_fbsize

#ifdef __EMSCRIPTEN__ // Main loop
        emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
        emscripten_set_main_loop(tick, 0, 1);
#else
        while (!glfwWindowShouldClose(g_window)) { tick(); }
#endif

        glfwDestroyWindow(g_window);
    }

    void setScreen(Screen *newScreen)
    {
        screen = newScreen;
        screen->setup(g_window);
    }
}