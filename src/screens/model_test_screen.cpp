//
// Created by sneeuwpop on 20-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include <fstream>

#include "screen.h"
#include "glm/glm.hpp"
#include "../graphics/shader_program.h"

class ModelTestScreen : public Screen {



public:
    ModelTestScreen() {

        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");
        shaderProgram.use();

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