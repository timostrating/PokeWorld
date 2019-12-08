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
    GLint MVPLocation, colorLocation;

//    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");

    Gizmos gizmos;
    SharedMesh quad = SharedMesh(Mesh::quad());

    float randoms[999];

public:
    EverydayScreen()
    {
        shader.use();
        MVPLocation = shader.uniformLocation("MVP");
        colorLocation = shader.uniformLocation("u_color");

        VertexBuffer::uploadSingleMesh(quad);

        for (float & i : randoms) i = MATH::random();
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;
    float iCopy = 0;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(69.0/255.0, 69.0/255.0, 69.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, 1.0f / tanf(radians(22.5f)));
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        int i = static_cast<int>(iCopy);
        iCopy += 0.25f;

        for (float v=-1; v<0.99; v += 0.1f, i++) {
            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(v + 0.05, 0.65f, 0)), vec3(.05f, 0.3f, 1.0f)) )[0][0]);
            glUniform4f(colorLocation, randoms[i*3 +0], randoms[i*3 +1], randoms[i*3 +2], 1.0f);
            quad->render();

            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(v + 0.05, 0, 0)), vec3(.05f, 0.3f, 1.0f)) )[0][0]);
            glUniform4f(colorLocation, MATH::randomGoldenRatio(i*3 +0), MATH::randomGoldenRatio(i*3 +1), MATH::randomGoldenRatio(i*3 +2), 1.0f);
            quad->render();

            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(v + 0.05, -0.65f, 0)), vec3(.05f, 0.3f, 1.0f)) )[0][0]);
            vec4 color = COLOR::hsvToColor(MATH::randomGoldenRatio(i*3 +0), 0.5f, 0.95f, 1.0f);
            glUniform4f(colorLocation, color.r, color.g, color.b, color.a);
            quad->render();
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};