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
        uniform vec2 u_uv;
        uniform sampler2D u_texture;

        void main() {
            vec3 pos = a_pos;
            if (pos.y > 0.0) {
                vec3 v = texture(u_texture, u_uv).rgb;
                pos.y = 30.0 * (v.r + v.g + v.b);
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
            vec3 v = vec3(.8, .8, .8);
            vec3 t = texture(u_texture, u_uv).rgb;
            if (t.r < 0.7) { v = vec3(t.r, 0.6, 0.6); }
            if (t.r < 0.5) { v = vec3(0.4, t.g, 0.4); }
            if (t.r < 0.3) { v = vec3(0.2, 0.2, t.b); }

            v += distance(u_uv, vec2(0.5, 0.5)) * 0.7;
            outputColor = vec4(v, 1.0);
        })glsl";


    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    Texture texture = Texture::fromAssetFile("/textures/heightmap2.jpg");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::cube());

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
        glClearColor(209.0/255.0, 222.0/255.0, 222.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(sin(time * 0.2f), 0.5, cos(time * 0.2f)); // -1.0 / tanf(radians(22.5))
        camera.lookAt(0.2f * VEC3::Y);
        camera.Camera::update();

        texture.bind(0, shader, "u_texture");
        glUniform1f(shader.uniformLocation("u_time"), time);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        for (float x=-1; x<1.0; x += 0.01)
            for (float z=-1; z<1.0; z += 0.01)
            {
                glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * (scale(translate(mat4(1.0f), vec3(x, 0, z)), 0.005f * VEC3::ONE)) )[0][0]);
                glUniform2f(shader.uniformLocation("u_uv"), (x + 1.0) / 2.0, (z + 1.0) / 2.0);
                glUniform3f(shader.uniformLocation("u_color"), abs(x), 0.0, abs(z));
                mesh->render();
            }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};