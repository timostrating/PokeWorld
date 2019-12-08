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

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    ShaderProgram skyShader = ShaderProgram::fromAssetFiles("shaders/lib/skybox.vert", "shaders/lib/skybox.frag");
    std::vector<std::string> faces = {
        "../../../../assets/textures/test/skybox/right.jpg",
        "../../../../assets/textures/test/skybox/left.jpg",
        "../../../../assets/textures/test/skybox/top.jpg",
        "../../../../assets/textures/test/skybox/bottom.jpg",
        "../../../../assets/textures/test/skybox/front.jpg",
        "../../../../assets/textures/test/skybox/back.jpg"
    };
    Cubemap* skycubemap = new Cubemap(faces);

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh skybox = SharedMesh(Mesh::skybox());


    float randoms[999];

public:
    EverydayScreen()
    {
        shader.use();

        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(skybox);

        for (float & i : randoms) i = MATH::random();
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
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.25) * 10, 1, cos(time * 0.25) * 10);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        skyShader.use();
        glUniformMatrix4fv(skyShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 100.0f * VEC3::ONE))[0][0]);
        skybox->render();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

//        shader.use();
//        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * translate(mat4(1.0f), vec3(0, 0, 0)) )[0][0]);
//        cube->render();

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};