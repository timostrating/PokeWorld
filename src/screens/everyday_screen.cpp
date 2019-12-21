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
#include "../graphics/cubemap.h"

using namespace glm;
using namespace MATH;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec2 v_uv;

        void main() {
            v_uv = 1.0 - (a_pos.xy + 1.0) / 2.0;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec2 v_uv;
        uniform float u_time;
        uniform float u_alpha;
        uniform sampler2D u_texture;

        void main() {
            vec4 color = vec4(97.0/255.0, 116.0/255.0, 140.0/255.0, 1.0);
            float noise = texture(u_texture, 0.2 * vec2(v_uv.x + 0.01 * u_time, v_uv.y + 0.015 * u_time)).x;
            color += 0.7 * vec4(smoothstep(0.7, 0.9, noise));

            color += vec4(sqrt(pow(v_uv.x - 0.5, 2.0) + pow(v_uv.y - 0.5, 2.0)));

            outputColor = vec4(color.xyz, u_alpha);
        })glsl";

    FlyingCamera camera = FlyingCamera();

//    ShaderProgram waterShader = ShaderProgram(vertSource, fragSource);
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
//    Texture texture = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(84.0/255.0, 84.0/255.0, 124.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.25) * 6.5, 3, cos(time * 0.25) * 6.5);
            camera.lookAt(vec3(0, 2, 0));
            camera.Camera::update();
        }
//        camera.debugDraw();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(scale(translate(mat4(1.0f), vec3(0, 0, 0)), 10.0f * VEC3::ONE), radians(-90.0f), VEC3::X))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), 0.15, 0.25, 0.15, 1.0);
        quad->render();

        float a = abs(sin(time * 1.0) * 0.5);
        float b = smoothstep(-1.0, 1.0, sin(time * 2.0)) * 0.2;
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 1 + a + b, 0)), vec3(1+b, 1-b, 1+b)))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), 1.0, 0.0, 1.0, 0.9);
        cube->render();

        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(scale(translate(mat4(1.0f), vec3(0, 3 + a + b, 0)), vec3(1.0, 0.1, 1)), static_cast<float>(time * 0.25f), VEC3::Y))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), 0.0, 1.0, 0.0, 0.9);
        quad->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};