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

    ShaderProgram waterShader = ShaderProgram(vertSource, fragSource);
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
    Texture texture = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        waterShader.use();
        texture.bind(0, waterShader, "u_texture");

        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        waterShader.use();
        glUniform1f(waterShader.uniformLocation("u_time"), time);

//        glClearColor(97.0/255.0, 116.0/255.0, 140.0/255.0, 1.0);
        glClearColor(0.8 * 208.0/255.0, 0.8 * 217.0/255.0, 0.8 * 242.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(1.5, 1, 1.5);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

//        glUniformMatrix4fv(waterShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(translate(mat4(1.0f), vec3(0, -0.05, 0)), radians(-90.0f), VEC3::X))[0][0]);
//        quad->render();

        shader.use();
        srand(0);
        for (float v=0.0; v< 2*PI; v += 0.01f) {
            float gray = random(0.6f, 0.9f);
            glUniform4f(shader.uniformLocation("u_color"), gray * 43.0/255.0, gray * 54.0/255.0, gray * 64.0/255.0, 1.0);
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(scale(translate(mat4(1.0f), random(1.0f, 1.5f) * vec3(sin(v), 0, cos(v))), random(0.05f, 0.1f) * VEC3::ONE), radians(random(0.0f, 360.0f)), VEC3::X))[0][0]);
            cube->render();
        }

        for (float v=0.0; v< 2*PI; v += 0.1f) {
//            glUniform4f(shader.uniformLocation("u_color"), 93.0/255.0, 145.0/255.0, 125.0/255.0, 1.0);
            glUniform4f(shader.uniformLocation("u_color"), 43.0/255.0, 64.0/255.0, 54.0/255.0, 1.0);
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(scale(translate(mat4(1.0f), random(1.0f, 1.5f) * vec3(sin(v), 0.08, cos(v))), random(0.02f, 0.05f) * VEC3::ONE), radians(random(0.0f, 360.0f)), VEC3::X))[0][0]);
            cube->render();
        }

        waterShader.use();
        glUniform1f(waterShader.uniformLocation("u_time"), time);

        glUniformMatrix4fv(waterShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(translate(mat4(1.0f), vec3(0, -0.02, 0)), radians(-90.0f), VEC3::X))[0][0]);
        glUniform1f(waterShader.uniformLocation("u_alpha"), 1.0);
        quad->render();
        glUniformMatrix4fv(waterShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * rotate(translate(mat4(1.0f), vec3(0, 0.02, 0)), radians(-90.0f), VEC3::X))[0][0]);
        glUniform1f(waterShader.uniformLocation("u_alpha"), 0.1);
        quad->render();


    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};