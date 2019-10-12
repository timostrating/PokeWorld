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
#include "../util/debug/gizmos.h"


class ModelTestScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    SharedMesh mesh = SharedMesh(new Mesh(12*3, 0, false)); // TODO: remove some verts
    ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");

    Gizmos gizmos;

    ModelTestScreen()
    {
        mesh->vertices.insert(mesh->vertices.begin(), {
            //  x,    y,    z       x,    y,    z       x,    y,    z
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
        });

        // Vertex Buffer
        VertexBuffer::uploadSingleMesh(mesh);

        // Model View Projection
        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
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
            camera.position = vec3(sin(time * 0.5) *5,  2,  cos(time * 0.5) *5);
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }

        mat4 mvp = camera.combined * modelMatrix;

        shaderProgram.use();
        glUniformMatrix4fv(
                MVPLocation,   // Location
                1,             // we want to edit one matrix
                GL_FALSE,      // do not transpose (rotate rows and cols around)
                &mvp[0][0]     // we share the pointer to the first value, 4fv knows to where it needs to go
        );
        mesh->render();

        gizmos.drawLine(VEC3::ZERO, VEC3::X * 5.0f, COLOR::RED);
        gizmos.drawLine(VEC3::ZERO, VEC3::Y * 5.0f, COLOR::GREEN);
        gizmos.drawLine(VEC3::ZERO, VEC3::Z * 5.0f, COLOR::BLUE);
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};
