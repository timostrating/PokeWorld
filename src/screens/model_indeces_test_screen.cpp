//
// Created by sneeuwpop on 20-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"


class ModelIndecesTestScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    constexpr static const GLfloat g_vertex_buffer_data[] = {
//            -1.0f, -1.0f, 0.0f,
//            1.0f, -1.0f, 0.0f,
//            1.0f,  1.0f, 0.0f,
//            -1.0f,  1.0f, 0.0f

            -1, -1, 0,
            -1,  1, 0,
            1,  1, 0,
            1, -1, 0,
    };

    // One color for each vertex. They were generated randomly.
    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    constexpr static const GLfloat g_color_buffer_data[] = {
            0.583f, 0.771f, 0.014f,
            0.609f, 0.115f, 0.436f,
            0.327f, 0.483f, 0.844f,
            0.014f, 0.327f, 0.771f,
    };

//    constexpr static const GLushort g_indices[6] = {0, 1, 2, 0, 2, 3};
    constexpr static const GLushort g_indices[] = {2, 1, 0, 0, 3, 2};
//    std::vector<unsigned short> g_indices = {2, 1, 0, 0, 3, 2};


    GLuint vaoId;
    GLuint vertex_buffer, element_buffer;

    ModelIndecesTestScreen() {

        // SHADER PROGRAM
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/3dcube.vert", "shaders/3dcube.frag");
        shaderProgram.begin();

        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);

        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &element_buffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_indices.size() * sizeof(unsigned short), &g_indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(
                0,              // must match the layout in the shader.
                3,              // size
                GL_FLOAT,       // type
                GL_FALSE,       // normalized?
                0,              // stride
                (void*)0        // array buffer offset
        );
        glEnableVertexAttribArray(0);

        GLuint color_buffer;
        glGenBuffers(1, &color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
                1,              // must match the layout in the shader.
                3,              // size
                GL_FLOAT,       // type
                GL_FALSE,       // normalized?
                0,              // stride
                (void*)0        // array buffer offset
        );
        glEnableVertexAttribArray(1);

        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
        camera.position = glm::vec3(0,0,5);
    }

    double time = 0;
    bool anyKeyPressed = false;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime) {
        time += 0.016;

        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::anyKeyEverPressed())
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *5,  0,  cos(time * 0.5) *5);
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }



        mat4 mvp = camera.combined * modelMatrix;

        glUniformMatrix4fv(
                MVPLocation,   // Location
                1,             // we want to edit one matrix
                GL_FALSE,      // do not transpose (rotate rows and cols around)
                &mvp[0][0]     // we share the pointer to the first value, 4fv knows to where it needs to go
        );

//        glDrawArrays(GL_TRIANGLES, 0, 6*2*3); // Starting from vertex 0; a cube had 6 sides, 2 triangles per side and 3 points per triangle;
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glDrawElements(GL_TRIANGLES, sizeof(g_indices), GL_UNSIGNED_SHORT, (void*)0);

//        glDisableVertexAttribArray(0); TODO we are not disabeling it

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};
