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
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"


class ModelIndecesTestScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    constexpr static const GLfloat g_vertex_buffer_data[] = {
            -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,   1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f
    };

    // One color for each vertex. They were generated randomly.
    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    constexpr static const GLfloat g_color_buffer_data[] = {
            0.583f, 0.771f, 0.014f,   0.609f, 0.115f, 0.436f,   0.327f, 0.483f, 0.844f,
            0.822f, 0.569f, 0.201f,   0.435f, 0.602f, 0.223f,   0.310f, 0.747f, 0.185f,
            0.597f, 0.770f, 0.761f,   0.559f, 0.436f, 0.730f,   0.359f, 0.583f, 0.152f,
            0.483f, 0.596f, 0.789f,   0.559f, 0.861f, 0.639f,   0.195f, 0.548f, 0.859f,
            0.014f, 0.184f, 0.576f,   0.771f, 0.328f, 0.970f,   0.406f, 0.615f, 0.116f,
            0.676f, 0.977f, 0.133f,   0.971f, 0.572f, 0.833f,   0.140f, 0.616f, 0.489f,
            0.997f, 0.513f, 0.064f,   0.945f, 0.719f, 0.592f,   0.543f, 0.021f, 0.978f,
            0.279f, 0.317f, 0.505f,   0.167f, 0.620f, 0.077f,   0.347f, 0.857f, 0.137f,
            0.055f, 0.953f, 0.042f,   0.714f, 0.505f, 0.345f,   0.783f, 0.290f, 0.734f,
            0.722f, 0.645f, 0.174f,   0.302f, 0.455f, 0.848f,   0.225f, 0.587f, 0.040f,
            0.517f, 0.713f, 0.338f,   0.053f, 0.959f, 0.120f,   0.393f, 0.621f, 0.362f,
            0.673f, 0.211f, 0.457f,   0.820f, 0.883f, 0.371f,   0.982f, 0.099f, 0.879f
    };

    ModelIndecesTestScreen() {

        // SHADER PROGRAM
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/3dcube.vert", "shaders/3dcube.frag");
        shaderProgram.use();




        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
                0,              // must match the layout in the shader.
                3,              // size
                GL_FLOAT,       // type
                GL_FALSE,       // normalized?
                0,              // stride
                (void*)0        // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glBindAttribLocation(shaderProgram.getId(), 0, "a_pos");

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
        glBindAttribLocation(shaderProgram.getId(), 1, "a_color");

        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
        camera.position = glm::vec3(0,0,5);
    }

    double time = 0;
    bool anyKeyPressed = false;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime) {
        time += deltaTime;

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

        glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data)); // Starting from vertex 0; 3 vertices total -> 1 triangle;
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};