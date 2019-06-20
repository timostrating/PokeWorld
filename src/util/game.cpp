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


namespace game {
    GLFWwindow *g_window;
    Screen *screen;

    // https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/
    void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         GLsizei length, const GLchar* message, const void* userParam){

        std::cout << "---------------------opengl-callback-start------------" << std::endl;
        std::cout << "message: "<< message << std::endl;
        std::cout << "type: ";
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: std::cout << "ERROR"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED_BEHAVIOR"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED_BEHAVIOR"; break;
            case GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE"; break;
            case GL_DEBUG_TYPE_OTHER: std::cout << "OTHER"; break;
        }
        std::cout << std::endl;

        std::cout << "id: " << id << std::endl;
        std::cout << "severity: ";
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_LOW: std::cout << "LOW"; break;
            case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "MEDIUM"; break;
            case GL_DEBUG_SEVERITY_HIGH: std::cout << "HIGH"; break;
        }
        std::cout << std::endl;
        std::cout << "---------------------opengl-callback-end--------------" << std::endl;
    }

    void printContext()
    {
        std::cout << "[OpenGL] " << glGetString(GL_VERSION) << std::endl;
        std::cout << "[Vendor] " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "[GLSL] " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "[Extensions] " << glGetString(GL_EXTENSIONS) << std::endl;
    }

    void enableGLDebug()
    {
        glEnable(GL_DEBUG_OUTPUT);
        if (glDebugMessageCallback)
        {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(openglCallbackFunction, nullptr);
            glDebugMessageControl(GL_DONT_CARE, // all sources
                                  GL_DONT_CARE, // all types
                                  GL_DONT_CARE, // all severity(ies)
                                  0, // count
                                  nullptr, // If count​ is zero then the value of ids​ is ignored
                                  true); // enable of disable
        }
        else {
            std::cout << "_!_ glDebugMessageCallback not available _!_" << std::endl;
        }
    }

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
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL context\n";
            return -1;
        }

        enableGLDebug();
        printContext();

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
        screen->setup(g_window);
    }
}