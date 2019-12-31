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
#include "../game/bezier_curve.h"

using namespace glm;
using namespace MATH;

#define MULTILINE(...) #__VA_ARGS__

class particle
{
public:

    vec3 pos;
    particle() : pos(vec3(0)) {}
};

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform vec4 u_color;

        out vec3 v_color;
        out vec3 v_pos;

        void main() {
            v_pos = a_pos;
            v_color = vec3(u_color.r + (a_pos.x/10.), u_color.g + (a_pos.y/10.), u_color.b + (a_pos.z/10.));
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec3 v_color;
        in vec3 v_pos;

        void main() {
            if (sqrt(pow(v_pos.x, 2.0) + pow(v_pos.y, 2.0)) > 1.)
                discard;
            outputColor = vec4(v_color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());


    BezierCurve curve = BezierCurve(vec3(-8, 0, -5), vec3(-3, 0, 5), vec3(4, -1, -2), vec3(4, -1, 3));
    particle particles[1200] = {};

public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
//        texture.bind(0, shader, "u_texture");
    }

    void setup(GLFWwindow* window) {}


    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

//        glClearColor(0.5 * 212.0/255.0, 0.5 * 172.0/255.0, 0.5 * 138.0/255.0, 1.0);
        glClearColor(16.0/255.0, 16.0/255.0, 24.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time) * 0.1, 0.2, 3);
//            camera.position = vec3(5, 7, 5);
            camera.lookAt(vec3(0, 1, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST
        float v = fmodf(time * 0.5, 4.0);
        shader.use();

        srand(0);

        vec3 positions[3] = {vec3(0), vec3(-1,0,0), vec3(1,0,0)};
        vec3 colors[3] = {vec3(0.8, 0.3, 0.3), vec3(0.3, 0.8, 0.3), vec3(0.3, 0.3, 0.8)};
        int i=0;
        for (int n=1; n<=3; n++)
        {
            glUniform4f(shader.uniformLocation("u_color"), colors[n-1].r, colors[n-1].g, colors[n-1].b, 1.0);
            for (; i<n*400; i++)
            {
                if (v < 1.5) {
                    particles[i].pos.x = 0;
                    particles[i].pos.y = remap(v, 0, 1.5, 0, (n==1)?1.5:1);

                } else {

                    vec2 pos = MATH::randomVec2(-1, 1);
                    if (length(normalize(MATH::randomVec2(-1, 1))) < length(MATH::randomVec2(-1, 1)))
                        pos = normalize(pos);
                    particles[i].pos.x += ((n==1)?1.0:0.6) * 0.1f * pos.x;
                    particles[i].pos.y += ((n==1)?1.0:0.6) * 0.1f * pos.y;
                }
                glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), positions[n-1] + particles[i].pos + vec3(0,0,n*0.01)), ((v>3.0)?remap(v, 3, 3.5, 1, 0):1) * remap(v, 1.5, 3, 0, 1) * 0.03f * vec3(1)))[0][0]);
                quad->render();
            }
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};