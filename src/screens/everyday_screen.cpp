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

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform vec4 u_color;

        out vec3 v_color;

        void main() {
            v_color = vec3(u_color.r + (a_pos.x/4.), u_color.g + (a_pos.y/4.), u_color.b + (a_pos.z/4.));
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
//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
//    Texture texture = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());


    BezierCurve curve = BezierCurve(vec3(-8, 0, -5), vec3(-3, 0, 5), vec3(4, -1, -2), vec3(4, -1, 3));

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
    float f = 0.3;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

//        glClearColor(0.5 * 212.0/255.0, 0.5 * 172.0/255.0, 0.5 * 138.0/255.0, 1.0);
        glClearColor(16.0/255.0, 32.0/255.0, 64.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
//            camera.position = vec3(sin(time * 0.5) * 10, 7, cos(time * 0.5) * 10);
            camera.position = vec3(5, 7, 5);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST
        float v = fmodf(time*0.1, 1.0);
        shader.use();
//        curve.debugDraw();
        vec3 from = curve.getPoint(0), to, deriv, tangent, up, normal;
//        for (float v=0.1; v<1.0; v += 0.09)
//        {
            to = curve.getPoint(v);
            deriv = curve.getDerivative(v);
            tangent = normalize(deriv);
            up = normalize(cross(from, to));
            normal = normalize(cross(up, tangent));

//            gizmos.drawLine(from, to, vec4(deriv.r, deriv.g, deriv.b, 1.0));
            gizmos.drawCube(to, 0.01f, COLOR::PINK);
            gizmos.drawLine(to, to+tangent, COLOR::GREEN);
            gizmos.drawLine(to, to+up, COLOR::BLUE);
            gizmos.drawLine(to, to+normal, COLOR::RED);


//            cross(getDerivative(t), getPoint(t));

//            from = to;
//        }

        for (float n=0.0; n<v; n += 0.01)
        {
            vec3 oldUp = from+up;
            vec3 oldNormal = from+normal;
//            gizmos.drawLine(from+normal, from+up, vec4(0.8,0.5,0.3, 1.0));

            to = curve.getPoint(n);
            deriv = curve.getDerivative(n);
            tangent = normalize(deriv);
            up = normalize(cross(from, to));
            normal = normalize(cross(up, tangent));

            gizmos.drawLine(oldUp, to+up, vec4(0.8,0.5,0.3, 1.0));
            gizmos.drawLine(oldNormal, to+normal, vec4(0.8,0.5,0.3, 1.0));
            if (int(n*100) % 10 == 0)
                gizmos.drawLine(to+up, to+normal, vec4(0.8,0.8,0.3, 1.0));


            from = to;
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};