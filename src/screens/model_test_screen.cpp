//
// Created by sneeuwpop on 20-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "screen.h"
#include "../graphics/shader_program.h"

class ModelTestScreen : public Screen {



public:
    ModelTestScreen() {

        // SHADER PROGRAM
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");
        shaderProgram.use();

        static const float vertices[] = {
            //  x,     y,    z
            -0.6f, -0.6f, 0.0f,
             0.6f, -0.6f, 0.0f,
             0.0f,  0.6f, 0.0f,
        };


        // VBO
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(
                0,              // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,              // size
                GL_FLOAT,       // type
                GL_FALSE,       // normalized?
                0,              // stride
                (void*)0        // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glBindAttribLocation(shaderProgram.getId(), 0, "a_pos");


        // CAMERA
        glm::mat4 model = glm::mat4(1.0); // identity matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(4, 3, 3), // camera position
            glm::vec3(0, 0, 0), // camera loops at
            glm::vec3(0, 1, 0)  // camera Up vector
        );
        glm::mat4 projection = glm::perspective(glm::radians(45.0), 16.0 / 9.0, 0.1, 100.0);

        glm::mat4 mvp = projection * view * model;  // Remember, matrix multiplication is the other way around


        GLint MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
        glUniformMatrix4fv(
                MVPLocation,   // Location
                1,             // we want to edit one matrix
                GL_FALSE,      // do not transpose (rotate rows and cols around)
                &mvp[0][0]     // we share the pointer to the first value, 4fv knows to where it needs to go
        );

    }

    double time = 0;

    void render(double deltaTime) {
        time += deltaTime;

        glClearColor(fmod(time, 1), 0.0f, fmod(time * 2, 1), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }
};