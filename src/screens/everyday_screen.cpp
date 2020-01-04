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
#include "../util/splines/line.h"

using namespace glm;
using namespace MATH;

#define MULTILINE(...) #__VA_ARGS__

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
            v_color = vec3(u_color.r + (a_pos.x), u_color.g + (a_pos.y), u_color.b + (a_pos.z));
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;

        in vec3 v_color;
        in vec3 v_pos;

        void main() {
//            if (sqrt(pow(v_pos.x, 2.0) + pow(v_pos.y, 2.0) + pow(v_pos.z, 2.0)) < (abs(sin(u_time*0.5)) + 0.5) )
//                discard;
            outputColor = vec4(v_color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

    Line line = Line({scale(rotate(translate(mat4(1), vec3(0,0,0)), radians((4-0) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(1,0,0)), radians((4-1) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(1,0,1)), radians((4-2) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(0,0,1)), radians((4-3) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE)});

public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
        VertexBuffer::uploadSingleMesh(sphere);
//        texture.bind(0, shader, "u_texture");
    }

    void setup(GLFWwindow* window) {}


    double time = 0;
    bool anyKeyPressed = false;
    float edit1 = 1.7;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(154.0/255.0, 147.0/255.0, 135.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) * 3, 1, cos(time * 0.5) * 3);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST
        shader.use();
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_Q))
            edit1 -= 0.01;
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_W))
            edit1 += 0.01;
        std::cout << edit1 << "\n";

//        line.debugDraw(&gizmos);
//
//        for (int i=0; i<line.length; i++) {
//            vec3 position = line.getPointPosition(i);
//
//            for (float t=0; t<fmodf(time * 0.1, 1.0); t+=0.1)
//                for (float f=0; f<2*PI; f += 0.1)
//                    gizmos.drawLine(vec3(sin(f),cos(f), 0), vec3(sin(f+0.5),cos(f+0.5), 0), COLOR::PINK, line.lerpIndexed(i, t));
//        }
        vec4 color = vec4(0, 0, 0, 1.0);

        int MAX = 50;
        float phiStep = PI / MAX;
        float thetaStep = 2*PI / MAX;

        float t = remap(sin(time*0.5), -1, 1, 0.2, 1.5);
        float t2 = remap(cos(time*0.5), -1, 1, 0.2, 1.5);

        for (int y=0; y<MAX; y ++) // [-90,90]
        {
            float phi = remap(y, 0, MAX, -0.5*PI, 0.5*PI);
            float r1  = supershape(phi, 5, t, t2, t2);
            float r1s = supershape(phi+phiStep, 5, t, t2, t2);
            for (int x=0; x<MAX; x++) // [0,360]
            {
                float theta = remap(x, 0, MAX, 0, 2*PI);
                float r2  = supershape(theta, 5, t, t2, t2);
                float r2s = supershape(theta+thetaStep, 5, t, t2, t2);

                vec3 a1 = vec3(r1  * cos(phi)         * r2  * cos(theta),             r1  * sin(phi),           r1  * cos(phi)         * r2  * sin(theta));
                vec3 a2 = vec3(r1  * cos(phi)         * r2s * cos(theta+thetaStep),   r1  * sin(phi),           r1  * cos(phi)         * r2s * sin(theta+thetaStep));
                vec3 b1 = vec3(r1s * cos(phi+phiStep) * r2  * cos(theta),             r1s * sin(phi+phiStep),   r1s * cos(phi+phiStep) * r2  * sin(theta));
                vec3 b2 = vec3(r1s * cos(phi+phiStep) * r2s * cos(theta+thetaStep),   r1s * sin(phi+phiStep),   r1s * cos(phi+phiStep) * r2s * sin(theta+thetaStep));
                gizmos.drawLine(a1, a2, color);
                gizmos.drawLine(a1, b1, color);
                gizmos.drawLine(a1, b2, color);
            }
        }

//        std::cout << " " << y << " " << x/y << "\n";
    }

    const float a = 1.0;
    const float b = 1.0;

    float supershape(float theta, float m, float n1, float n2, float n3)
    {
        return pow(   pow(abs((1.0/a) * cos(m * theta / 4.0)), n2)  +  pow(abs((1.0/b) * sin(m * theta / 4.0)), n3),   -1.0/n1);
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};