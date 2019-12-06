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
#include "../graphics/texture.h"
#include "../game/marching_cubes_terrain.h"

using namespace glm;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        void main() {
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;

        void main() {
            outputColor = vec4(1.0, 0.1, 1.0, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

    Gizmos gizmos;
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        MVPLocation = shader.uniformLocation("MVP");

        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(69.0/255.0, 69.0/255.0, 69.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.3) * 15, 10, cos(time * 0.3) * 15);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }
        camera.debugDraw();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * rotate(scale(translate(mat4(1.0f), vec3(0, -0.01, 0)), 5.0f * VEC3::ONE), radians(90.0f), VEC3::X) )[0][0]);
        quad->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};