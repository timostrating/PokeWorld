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
#include "../graphics/mesh.h"
#include "../graphics/vertex_buffer.h"
#include "../util/input/keyboard.h"


class QuadModelTestScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    SharedMesh mesh = SharedMesh(new Mesh(4, 6));

    QuadModelTestScreen()
    {
        // Shader Program
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");
        shaderProgram.use();

        mesh->vertices.insert(mesh->vertices.begin(), {
                -1, -1, 0,
                -1,  1, 0,
                1,  1, 0,
                1, -1, 0,

//                //  x,     y,    z
//                -0.6f, -0.6f, 0.0f,
//                 0.6f, -0.6f, 0.0f,
//                 0.0f,  0.6f, 0.0f,
        });
        mesh->indicies.insert(mesh->indicies.begin(), /*{0,1,2}*/ {2, 1, 0, 0, 3, 2});

        // Vertex Buffer
        VertexBuffer vertexBuffer = VertexBuffer();
        vertexBuffer.add(mesh);
        vertexBuffer.upload();

        // Model View Projection
        MVPLocation = shaderProgram.uniformLocation("MVP");
        camera.position = glm::vec3(0,0,2);
    }

    double time = 0;
    bool anyKeyPressed = false;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime)
    {
        time += deltaTime;

        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::anyKeyEverPressed())
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *2,  0,  cos(time * 0.5) *2);
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

        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};