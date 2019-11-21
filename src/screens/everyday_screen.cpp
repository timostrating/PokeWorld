//
// Created by sneeuwpop on 18-11-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/debug/gizmos.h"

using namespace glm;

class EverydayScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/library/flat_color.vert", "shaders/library/flat_color.frag");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(new Mesh(6, 4*3));

    EverydayScreen()
    {
        // Shader Program
        shader.use();
        float h = 0.1f;
        float w = 0.1f;
        float s = 1.5f;

        mesh->vertices.insert(mesh->vertices.begin(), {
           0, s, 0,
           -1, -h, 0,
           +w, -3*h, 0,
           0, -s, 0,
           1, +h, 0,
           -w, +3*h, 0
        });

        mesh->indicies.insert(mesh->indicies.begin(), {0, 5, 1,  1, 5, 2,  2, 3, 4,  4, 2, 5});

        // Model View Projection
        MVPLocation = shader.uniformLocation("MVP");
        glUniform3f(shader.uniformLocation("color"), 0.0f/255.0, 0.0f/255.0, 0.0f/255.0);

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    mat4 modelMatrix = translate(mat4(1.0f), vec3(0, 0, 0));

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(247.0/255.0, 223.0/255.0, 13.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(sin(time) *5,  0,  cos(time) *5);
        camera.lookAt(VEC3::ZERO);
        camera.Camera::update();

        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &(camera.combined * modelMatrix)[0][0]);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};