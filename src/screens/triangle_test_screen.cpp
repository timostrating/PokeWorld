//
// Created by sneeuwpop on 16-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include "../util/interfaces/screen.h"
#include "glm/glm.hpp"

class TriangleTestScreen : public Screen {

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

public:
    TriangleTestScreen() {
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
                //      x,     y, r, g, b
                -0.6f, -0.6f, 1, 0, 0,
                0.6f, -0.6f, 0, 1, 0,
                0.0f,  0.6f, 0, 0, 1,
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

    double time = 0;

    void render(double deltaTime) {
        time += deltaTime;

        glClearColor(fmod(time, 1), 0.0f, fmod(time * 2, 1), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }
};