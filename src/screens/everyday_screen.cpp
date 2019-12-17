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
            vec3 color = vec3(13.0/255.0);
            float a = mod(gl_FragCoord.x + 30.0, 120.0);
            float b = mod(gl_FragCoord.y + 30.0, 120.0);
            float width = 1.0 + 19.0 * abs(cos(u_time * 0.5));

            if (a < width || b < width) color = vec3(64.0/255.0);
            a -= width/2.0;
            b -= width/2.0;
            if (sqrt(a*a + b*b) < sqrt(2.0 * pow(width/2.0, 2.0))) color = vec3(1.0);

            outputColor = vec4(color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");

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

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        //        float t2 = 8.0f * sin(time * 0.05);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0.001, 0.001, 2);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        shader.use();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 0.1f * VEC3::ONE))[0][0]);
        quad->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};