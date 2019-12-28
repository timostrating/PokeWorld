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

class BezierCurve {
    Gizmos gizmos;
    std::vector<vec3> points;

private:
    vec3 lerp(vec3, vec3, float t)
    {

    }

public:
    BezierCurve(vec3 a, vec3 b, vec3 c, vec3 d)
    {
        points.insert(points.begin(), {a,b,c,d});

        if (points.size() < 4 || points.size() % 2 != 0)
            nice_error("BezierCurve points incorrect");
    }

    void debugDraw()
    {
        for (auto p : points)
            gizmos.drawCube(p, 0.1, COLOR::WHITE);

        for (int i=0; i<points.size()-1; i++)
            gizmos.drawLine(points[i], points[i+1], COLOR::WHITE);
    }

    vec3 getPoint(float t, float scale=1, float scale2=1)
    {
        float one_t = 1-t;
        vec3 p0 = scale*points[0], p1 = scale*points[1], p2 = scale2*points[2], p3 = scale2*points[3];
        return powf(one_t,3)*p0 + 3*powf(one_t, 2)*t*p1 + 3*one_t*powf(t,2)*p2 + powf(t,3)*p3;
    }

    vec3 getDerivative(float t, float scale=1)
    {
        float one_t = 1-t;
        vec3 p0 = scale*points[0], p1 = scale*points[1], p2 = scale*points[2], p3 = scale*points[3];
        return 3*powf(one_t,2)*(p1-p0) + 6*one_t*t*(p2-p1) + 3*powf(t,2)*(p3-p2);
    }
};

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


    BezierCurve curve = BezierCurve(vec3(-4, 0, -1), vec3(-4, 0, 5), vec3(4, 1, -2), vec3(4, 1, 3));

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
        glClearColor(16.0/255.0, 16.0/255.0, 16.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) * 10, 7, cos(time * 0.5) * 10);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
//        curve.debugDraw();
        vec3 from = curve.getPoint(0, abs(sin(time)), abs(cos(time))), to, deriv;
        for (float v=0.01; v<1.0; v += 0.01)
        {
            to = curve.getPoint(v, abs(sin(time)), abs(cos(time)));
            deriv = curve.getDerivative(v, 0.1);

            gizmos.drawLine(from, to,       vec4(deriv.r *10, deriv.g *1, deriv.b *10, 1.0));
            gizmos.drawLine(from, to+deriv, vec4(deriv.r *1, deriv.g *10, deriv.b *1, 1.0));
//            gizmos.drawLine(from, to-deriv, vec4(deriv.r *1, deriv.g *1, deriv.b *10, 1.0));

            from = to;
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};