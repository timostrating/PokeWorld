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
    constexpr static char vertSource[] = "#version 300 es\n"
                        "layout (location = 0) in vec3 a_pos;"

                        "uniform mat4 MVP;"

                        "out vec2 v_pos;"

                        "void main() {"
                            "v_pos = a_pos.xy;"
                            "gl_Position = MVP * vec4(a_pos, 1);"
                        "}";

    constexpr static char fragSource[] = "#version 300 es\n"
                        "precision mediump float;"
                        "in vec2 v_pos;"

                        "uniform float u_time;"
                        "out vec4 outputColor;"

                        "void main() {"
                             "outputColor = vec4( abs(sin(v_pos.x * 50.0 + u_time)), abs(sin(v_pos.x * 50.0 - u_time * 1.33333)) + abs(sin(v_pos.y * 50.0 - u_time)), abs(sin(v_pos.y * 50.0 + u_time)), 1);"
                        "}";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation, timeLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();

        MVPLocation = shader.uniformLocation("MVP");
        timeLocation = shader.uniformLocation("u_time");

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    float screenFit = 2.0f * tan(45.0f);
    mat4 modelMatrix = scale(translate(mat4(1.0f), vec3(0, 0, 0)), screenFit * VEC3::ONE);

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(247.0/255.0, 223.0/255.0, 13.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(0, 0, -2);
        camera.lookAt(VEC3::ZERO);
        camera.Camera::update();

        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &(camera.combined * modelMatrix)[0][0]);
        glUniform1f(timeLocation, time);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};