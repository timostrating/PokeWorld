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

    ShaderProgram skyShader = ShaderProgram(vertSource, fragSource);
//    ShaderProgram skyShader = ShaderProgram::fromAssetFiles("shaders/lib/skybox.vert", "shaders/lib/skybox.frag");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());


    float randoms[999];

public:
    EverydayScreen()
    {
        skyShader.use();

        VertexBuffer::uploadSingleMesh(cube);

        for (float & i : randoms) i = MATH::random();
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        glUniform1f(skyShader.uniformLocation("u_time"), time);

        glClearColor(38.0/255.0, 50.0/255.0, 56.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, 1);
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        skyShader.use();
        glUniformMatrix4fv(skyShader.uniformLocation("MVP"), 1, GL_FALSE,
                           &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 1.0f * VEC3::ONE))[0][0]);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        float t2 = (sin(time * 0.3) + 1.0) / 2.0;

        vec4 color1 = vec4(59.0/255.0, 72.0/255.0, 77.0/255.0, 1.0);
        vec3 a = vec3(-0.8, -0.8, 0.0);
        vec3 b = vec3(-0.8,  0.8, 0.0);
        vec3 c = vec3( 0.8,  0.8, 0.0);
        vec3 d = vec3( 0.8, -0.8, 0.0);

        gizmos.drawCube(a, 0.02f, COLOR::WHITE);
        gizmos.drawCube(b, 0.02f, COLOR::WHITE);
        gizmos.drawCube(c, 0.02f, COLOR::WHITE);
        gizmos.drawCube(d, 0.02f, COLOR::WHITE);

        gizmos.drawLine(a, b, COLOR::WHITE);
        gizmos.drawLine(b, c, COLOR::WHITE);
        gizmos.drawLine(c, d, COLOR::WHITE);

        vec3 mid = VEC3::ZERO, midAC = VEC3::ZERO, midBD = VEC3::ZERO;
        for (float t=0.0f; t<t2; t+=0.01f) {
            vec3 midAB = lerp(a, b, t);
            vec3 midBC = lerp(b, c, t);
            vec3 midCD = lerp(c, d, t);

            gizmos.drawLine(midAB, midBC, color1 + vec4(1.0, 0.0, 0.0, 1.0));
            gizmos.drawLine(midBC, midCD, color1 + vec4(0.0, 1.0, 0.0, 1.0));

            midAC = lerp(midAB, midBC, t);
            midBD = lerp(midBC, midCD, t);
            gizmos.drawLine(midAC, midBD, color1 + vec4(0.0, 0.0, 1.0, 1.0));

            mid = lerp(midAC, midBD, t);
            gizmos.drawCube(mid, 0.01f, 0.7f * COLOR::WHITE);
        }
        gizmos.drawCube(midAC + vec3(0, 0, 0.001f), 0.01f, color1 + vec4(1.0, 0.0, 0.0, 1.0));
        gizmos.drawCube(midBD + vec3(0, 0, 0.001f), 0.01f, color1 + vec4(0.0, 1.0, 0.0, 1.0));
        gizmos.drawCube(mid + vec3(0, 0, 0.002f), 0.01f, COLOR::WHITE);
    }

    vec3 lerp(vec3 from, vec3 to, float t) {
        return (1.0f - t) * from + t * to;
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};