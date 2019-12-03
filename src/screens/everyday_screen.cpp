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

        #define t u_time * 0.25
        #define r vec2(800.0, 800.0)
        #define ff pow(cos(t), 2.0) * 200.

        void main() {
            vec3 colorA = vec3(0.6, 0.8, 0.1);
            vec3 colorB = vec3(0.1, 0.3, 0.8);

            vec3 c = vec3(0.0, 0.0, 0.0);
            if (abs(gl_FragCoord.x - 400.0) >= 200.0 || abs(gl_FragCoord.y - 400.0) >= 200.0) {
                if (sqrt(pow(gl_FragCoord.x - ff, 2.0)          + pow(gl_FragCoord.y - ff, 2.0)) < 100.0 ) { c = vec3(1.0); }
                if (sqrt(pow(800.0 - gl_FragCoord.x - ff, 2.0)  + pow(gl_FragCoord.y - ff, 2.0)) < 100.0 ) { c = vec3(1.0); }
                if (sqrt(pow(gl_FragCoord.x - ff, 2.0)          + pow(800.0 - gl_FragCoord.y - ff, 2.0)) < 100.0 ) { c = vec3(1.0); }
                if (sqrt(pow(800.0 - gl_FragCoord.x - ff, 2.0)  + pow(800.0 - gl_FragCoord.y - ff, 2.0)) < 100.0 ) { c = vec3(1.0); }
            }

            float mixValue = sqrt(pow(gl_FragCoord.x, 2.0) + pow(gl_FragCoord.y, 2.0)) / sqrt(pow(r.x, 2.0) + pow(r.y, 2.0));

            vec3 mixColor = mix(colorA, colorB, mixValue);
            outputColor = vec4(c + mixColor, 1.0);
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

        glClearColor(18.0/255.0, 32.0/255.0, 42.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, + -1.0f / tanf(radians(22.5f)));
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * translate(mat4(1.0f), vec3(0, 0, 0)) )[0][0]);
        quad->render();

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};