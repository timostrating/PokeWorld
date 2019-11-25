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

using namespace glm;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec2 v_uv;

        void main() {
            v_uv = 1.0 - (a_pos.xy + 1.0) / 2.0;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec2 v_uv;
        uniform sampler2D u_texture;
        uniform float u_time;

        void main() {
            outputColor = texture(u_texture, v_uv);

            float v = sin(0.5 * u_time);

            vec4 rValue = texture(u_texture, v_uv - vec2(v * sin(v_uv.x - 0.5) * 0.9, v * sin(v_uv.y - 0.5) * 0.7));
            vec4 gValue = texture(u_texture, v_uv - vec2(v * sin(v_uv.x - 0.5) * 0.8, v * sin(v_uv.y - 0.5) * 0.8));
            vec4 bValue = texture(u_texture, v_uv - vec2(v * sin(v_uv.x - 0.5) * 0.7, v * sin(v_uv.y - 0.5) * 0.9));
            outputColor = vec4(rValue.r, gValue.g, bValue.b, 1.0);
        })glsl";


    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    Texture texture = Texture::fromAssetFile("textures/pokemon-stadium.png");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();

        MVPLocation = shader.uniformLocation("MVP");

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(104.0/255.0, 192.0/255.0, 105.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(0, 0, -1.0 / tanf(radians(22.5)));
        camera.lookAt(VEC3::ZERO);
        camera.Camera::update();

        texture.bind(0, shader, "u_texture");
        glUniform1f(shader.uniformLocation("u_time"), time);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * (translate(mat4(1.0f), vec3(0, 0, 0))) )[0][0]);
        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};