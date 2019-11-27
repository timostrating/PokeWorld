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
#include "../util/external/stb_font.hpp"

using namespace glm;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform vec2 u_uv;
        uniform sampler2D u_texture;
        uniform float u_time;


        void main() {
            vec3 pos = a_pos;
            if (pos.y > 0.0) {
                vec3 v = texture(u_texture, u_uv).rgb;
                pos.y = 30.0 * (v.r + v.g + v.b) * abs(sin(u_time * 0.3));
            }
            gl_Position = MVP * vec4(pos, 1.0);
        })glsl";

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;
        uniform vec3 u_color;
        uniform vec2 u_uv;
        uniform sampler2D u_texture;

        void main() {
            vec3 v = vec3(200.0/255.0, 247.0/255.0, 64.0/255.0);
            vec3 t = texture(u_texture, u_uv).rgb * abs(sin(u_time * 0.3));
            if (t.r < 0.7) { v = vec3(200.0/255.0, 0.3 + t.g, 64.0/255.0); }
            if (t.r < 0.5) { v = vec3(241.0/255.0, 0.5 + t.g, 96.0/255.0); }
            if (t.r < 0.3) { v = vec3(0.0/255.0, 144.0/255.0, 0.7 + t.b); }

            outputColor = vec4(v, 1.0);
        })glsl";




    constexpr static char vertSource2[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        void main() {
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource2[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        void main() {
            vec3 v = vec3(.0, .0, .0);
            vec3 a = vec3(25.0/255.0,  50.0/255.0, 79.0/255.0);
            vec3 b = vec3(194.0/255.0, 198.0/255.0, 232.0/255.0);
            vec3 c = vec3(0.0/255.0, 144.0/255.0, 0.7);

            if (gl_FragCoord.y > 600.0) {
                v = vec3(
                    b[0] + (a[0] - b[0]) * ((gl_FragCoord.y - 600.0) / 200.0),
                    b[1] + (a[1] - b[1]) * ((gl_FragCoord.y - 600.0) / 200.0),
                    b[2] + (a[2] - b[2]) * ((gl_FragCoord.y - 600.0) / 200.0)
                );  }
            else {
                v = vec3(
                    c[0] + (b[0] - c[0]) * ((gl_FragCoord.y - 400.0) / 200.0),
                    c[1] + (b[1] - c[1]) * ((gl_FragCoord.y - 400.0) / 200.0),
                    c[2] + (b[2] - c[2]) * ((gl_FragCoord.y - 400.0) / 200.0)
                );  }

            outputColor = vec4(v, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram terrainShader = ShaderProgram(vertSource, fragSource);
    ShaderProgram skyShader = ShaderProgram(vertSource2, fragSource2);
    Texture texture = Texture::fromAssetFile("/textures/heightmap2.jpg");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        terrainShader.use();
        MVPLocation = terrainShader.uniformLocation("MVP");
        texture.bind(0, terrainShader, "u_texture");

        VertexBuffer::uploadSingleMesh(mesh);
        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(terrainShader.uniformLocation("u_time"), time);

        glClearColor(1.0/255.0, 2.0/255.0, 5.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(1,  0.25f,  -1);
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        skyShader.use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * (scale(translate(mat4(1.0f), vec3(-1, 0, 1)), 10.0f * VEC3::ONE)) )[0][0]);
        quad->render();


        terrainShader.use();

        float stepSize = 0.005f;
        for (float x=-1; x<1.0; x += stepSize)
            for (float z=-1; z<1.0; z += stepSize)
            {
                glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * (scale(translate(mat4(1.0f), vec3(x, 0, z)), 0.5f * stepSize * VEC3::ONE)) )[0][0]);
                glUniform2f(terrainShader.uniformLocation("u_uv"), (x + 1.0) / 2.0, (z + 1.0) / 2.0);
                glUniform3f(terrainShader.uniformLocation("u_color"), abs(x), 0.0, abs(z));
                mesh->render();
            }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};