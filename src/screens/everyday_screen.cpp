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

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        out vec3 TexCoords;

        uniform mat4 MVP;

        void main()
        {
            TexCoords = a_pos;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;

        void main() {
            vec4 color = texture(skybox, TexCoords);
            color *= 0.5 + gl_FragCoord.y / 800.0;
            outputColor = vec4(color.b, color.b, color.b, 0.3 + 0.3 * abs(sin(u_time)));
        })glsl";

    FlyingCamera camera = FlyingCamera();

//    ShaderProgram skyShader = ShaderProgram(vertSource, fragSource);
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());


    float randoms[999];

public:
    EverydayScreen()
    {
        shader.use();

        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);

        for (float & i : randoms) i = MATH::random();
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;
    float max = 3;
    int ticker = 0;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);
        vec4 color1 = vec4(87.0/255.0, 53.0/255.0, 0.0/255.0, 1.0);
        vec4 color2 = vec4(150.0/255.0, 117.0/255.0, 63.0/255.0, 1.0);
        vec4 color3 = vec4(250.0/255.0, 233.0/255.0, 205.0/255.0, 1.0);

        //        float t2 = 8.0f * sin(time * 0.05);
        glClearColor(color1.r, color1.g, color1.b, color1.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, 2);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        shader.use();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(2, 0, -1)), 2.0f * VEC3::ONE))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), color3.r, color3.g, color3.b, color3.a);

        quad->render();

        glUniform4f(shader.uniformLocation("u_color"), color2.r, color2.g, color2.b, color2.a);
        float v = cos(time * 0.3) * 0.2;
        if (v < 0) { v = 0; time += 0.2; }

        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0.2f + v, 0, 0)), vec3(0.2f, 0.6f, 1)))[0][0]);
        quad->render();
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(-0.2f - v, 0, 0)), vec3(0.2f, 0.6f, 1)))[0][0]);
        quad->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};