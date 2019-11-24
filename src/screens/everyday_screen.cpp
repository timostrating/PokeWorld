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

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{
    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform float u_time;

        void main() {
           gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;
        uniform vec3 u_color;

        void main() {
            if (gl_FragCoord.y < 390.0)
                outputColor = vec4(vec3(1.0, 1.0, 1.0) + u_color, 1.0);
            if (gl_FragCoord.y > 410.0)
                outputColor = vec4(vec3(1.0, 0.0, 0.0) + u_color, 1.0);
            if (sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0)) < 70.0)
                outputColor = vec4(0.0, 0.0, 0.0, 1.0);
            if (sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0)) < 50.0)
                outputColor = vec4(1.0, 1.0, 1.0, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation, colorLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();

        MVPLocation = shader.uniformLocation("MVP");
        colorLocation = shader.uniformLocation("u_color");

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(104.0/255.0, 192.0/255.0, 105.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(sin(time * 0.3f) *4, 0, cos(time * 0.3f) *4);;
        camera.lookAt(VEC3::ZERO);
        camera.Camera::update();

        glUniform1f(shader.uniformLocation("u_time"),  time);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        for (float i=-0.99f; i<1; i += 0.05f)
        {
            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, sin(i * 0.5f * MATH::PI))), cos(i * 0.5f * MATH::PI) * vec3(1,1,1)) )[0][0]);
            glUniform3f(colorLocation, sin(i * 10.0f) / 4.0, sin(i * 10.0f) / 4.0, sin(i * 10.0f) / 4.0 );
            mesh->render();
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};