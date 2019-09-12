//
// Created by sneeuwpop on 12-09-19.
//

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../graphics/mesh.h"
#include "../graphics/vertex_buffer.h"
#include "../util/input/keyboard.h"
#include "../game/LSystem.h"


class TreeScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    SharedMesh mesh = SharedMesh(new Mesh(4, 6));

    TreeScreen()
    {
        // Shader Program
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");
        shaderProgram.begin();

        mesh->vertices.insert(mesh->vertices.begin(), {
                -1, 0, -1,
                -1, 0,  1,
                1, 0,  1,
                1, 0, -1,
        });
        mesh->indicies.insert(mesh->indicies.begin(), {2, 1, 0, 0, 3, 2});

        // Vertex Buffer
        VertexBuffer vertexBuffer = VertexBuffer();
        vertexBuffer.add(mesh);
        vertexBuffer.upload();

        // Model View Projection
        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
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
            camera.position = vec3(sin(time * 0.5) *2,  2,  cos(time * 0.5) *2);
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }


        mat4 mvp = camera.combined * modelMatrix;
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &mvp[0][0]);

        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};