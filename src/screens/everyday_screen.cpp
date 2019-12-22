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

        out vec3 v_color;

        void main() {
            vec3 a = vec3(51.0/255.0, 27.0/255.0, 24.0/255.0);
            vec3 b = vec3(213.0/255.0, 163.0/255.0, 116.0/255.0);
            v_color = mix(a, b, (a_pos.y + 1.0) * 0.5);
//            v_color = vec3(51.0/255.0 + (a_pos.x/10.), 27.0/255.0 + (a_pos.y/10.), 24.0/255.0 + (a_pos.z/10.));
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec3 v_color;

        void main() {
            outputColor = vec4(v_color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/terrain.vert", "shaders/terrain.frag");
//    Texture texture = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());

    static const int COUNT = 10;
    SharedMesh stones[COUNT];
    static const int POINTS = 7;
    vec3 points[POINTS];

public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);

        for (int n=0; n<COUNT; n++) {
            stones[n] = SharedMesh(new Mesh(0, 0));
            srand(n);
            for (int i = 0; i < POINTS; i++)
                points[i] = MATH::randomPointOnSphere(180, 360);

            for (int i = 0; i < POINTS; i++)
                for (int j = 0; j < POINTS; j++)
                    for (int k = 0; k < POINTS; k++)
                        if (i != j && i != k && j != k)
                            stones[n]->vertices.insert(stones[n]->vertices.begin(),
                                                   {points[i].x, points[i].y, points[i].z,
                                                    points[j].x, points[j].y, points[j].z,
                                                    points[k].x, points[k].y, points[k].z});

            stones[n]->nrOfVerts = stones[n]->vertices.size() / 3;
            VertexBuffer::uploadSingleMesh(stones[n]);
        }

    }

    void setup(GLFWwindow* window) {}


    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(0.5 * 212.0/255.0, 0.5 * 172.0/255.0, 0.5 * 138.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) * 3.5, 0.5, cos(time * 0.5) * 3.5);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * translate(mat4(1.0f), vec3(0, 0, 0)))[0][0]);
//        glUniform4f(shader.uniformLocation("u_color"), 1.0, 0.5, 0.5, 1.0);
        stones[int(time) % COUNT]->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};