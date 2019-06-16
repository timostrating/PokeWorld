//
// Created by sneeuwpop on 16-6-19.
//

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>


GLFWwindow *g_window;

void render(); // TODO: HEADER FILE
void create();

static const float positions[6] = {
    0.0f,  0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f,
};

GLuint compile_shader(GLenum shaderType, const char *src)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char *buf = (char*)malloc(maxLength+1);
        glGetShaderInfoLog(shader, maxLength, &maxLength, buf);
        printf("%s\n", buf);
        free(buf);
        return 0;
    }

    return shader;
}

GLuint create_program(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindAttribLocation(program, 0, "apos");
    glBindAttribLocation(program, 1, "acolor");
    glLinkProgram(program);
    return program;
}

void printContext()
{
    std::cout << "[OpenGL] " << glGetString(GL_VERSION) << std::endl;
    std::cout << "[Vendor] " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "[GLSL] " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
//    std::cout << " [Extensions] " << glGetString(GL_EXTENSIONS) << std::endl;
}

void create()
{
//    GLuint vertexbuffer;
//    glGenBuffers(1, &vertexbuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
//    glEnableVertexAttribArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

    static const char vertex_shader[] =
            "attribute vec4 apos;"
            "attribute vec4 acolor;"
            "varying vec4 color;"
            "void main() {"
            "color = acolor;"
            "gl_Position = apos;"
            "}";
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] =
            "precision lowp float;"
            "varying vec4 color;"
            "void main() {"
            "gl_FragColor = color;"
            "}";
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = create_program(vs, fs);
    glUseProgram(program);

    static const float pos_and_color[] = {
            //     x,     y, r, g, b
            -0.6f, -0.6f, 1, 0, 0,
            0.6f, -0.6f, 0, 1, 0,
            0.f,   0.6f, 0, 0, 1,
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos_and_color), pos_and_color, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void render()
{
    std::cout << "oke \n";

    glClearColor(0.7f, 0.0f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glfwSwapBuffers(g_window);
    glfwPollEvents();
}

int main(void)
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
        return 1;
    }


    create();
    printContext();


#ifdef __EMSCRIPTEN__ // Main loop
    emscripten_set_main_loop(render, 0, 1);
#else
    while (!glfwWindowShouldClose(g_window)) { render(); }
#endif
    glfwTerminate();

    return 0;
}