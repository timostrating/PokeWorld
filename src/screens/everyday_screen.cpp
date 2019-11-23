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
    constexpr static char vertSource[] = MULTILINE(#version 300 es \n
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        out vec3 v_addcolor;
        uniform float u_time;

        void main() {
            if (a_pos.y < 1.0)
               v_addcolor = vec3(0.0, 0.3  + 0.1f * sin(u_time), 0.0);
            else
               v_addcolor = vec3(0.0, 0.0, 0.0);

           gl_Position = MVP * vec4(a_pos, 1.0);
        });

    constexpr static char fragSource[] = MULTILINE(#version 300 es \n
        precision mediump float;

        in vec3 v_addcolor;
        out vec4 outputColor;

        uniform vec3 u_color;

        void main() {
            outputColor = vec4(u_color + v_addcolor, 1.0);
        });

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation, colorLocation;

    ShaderProgram groundShader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());
    SharedMesh cube = SharedMesh(Mesh::cube());

    static const int SIZE = 25;
    vec3 data[2*SIZE];

public:
    EverydayScreen()
    {
        for (int i=0; i<2*SIZE; i+=2)
        {
            data[i] = MATH::randomVec3(-0.8f, 0.8f);
            data[i+1] = vec3(MATH::random(0.1f, 0.2f), MATH::random(0.2f, 0.6f), MATH::random(0.1f, 0.2f));
        }

        shader.use();

        MVPLocation = shader.uniformLocation("MVP");
        colorLocation = shader.uniformLocation("u_color");

        VertexBuffer::uploadSingleMesh(mesh);
        VertexBuffer::uploadSingleMesh(cube);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    mat4 modelMatrix = rotate(translate(mat4(1.0f), vec3(0, 0, 0)), radians(90.0f), VEC3::X);

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(47.0/255.0, 46.0/255.0 + 0.01f * sin(time), 15.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        groundShader.use();

        camera.position = vec3(sin(time * 0.25f) *3.5, 1, cos(time * 0.25f) *3.5);;
        camera.lookAt(0.5f * VEC3::Y);
        camera.Camera::update();

        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * modelMatrix)[0][0]);
        glUniform3f(colorLocation, 21.0/255.0, 22.0/255.0 + 0.3f + 0.1f * sin(time), 22.0/255.0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        mesh->renderMode = GL_TRIANGLES;
        mesh->render();

        glUniform3f(colorLocation, 67.0/255.0, 76.0/255.0, 35.0/255.0);
        mesh->renderMode = GL_LINES;
        mesh->render();

        shader.use();
        for (int i=0; i<2*SIZE; i+=2)
        {
            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(data[i].x, data[i+1].y, data[i].z)), data[i+1]) )[0][0]);
            glUniform1f(shader.uniformLocation("u_time"),  time);

            glUniform3f(colorLocation, (22.0+i)/255.0, (22.0+i)/255.0, (22.0+i)/255.0);
            cube->renderMode = GL_TRIANGLES;
            cube->render();

            glUniform3f(colorLocation, 67.0/255.0, 76.0/255.0, 35.0/255.0);
            cube->renderMode = GL_LINES;
            cube->render();
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};