//
// Created by sneeuwpop on 20-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/camera.h"


class ModelTestScreen : public Screen
{
public:
    Camera camera = Camera();
    GLint MVPLocation;

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

        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
    }

    double time = 0;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime) {
        time += deltaTime;

        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.position = vec3(sin(time * 0.5) *2,  0,  cos(time * 0.5) *2);
        camera.update();

        mat4 mvp = camera.combined * modelMatrix;

        glUniformMatrix4fv(
                MVPLocation,   // Location
                1,             // we want to edit one matrix
                GL_FALSE,      // do not transpose (rotate rows and cols around)
                &mvp[0][0]     // we share the pointer to the first value, 4fv knows to where it needs to go
        );

        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};