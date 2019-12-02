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

        void main() {
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform sampler2D u_texture;
        uniform float u_time;

        float rand(vec2 co){
            return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
        }

        #define t u_time * 0.125
        #define r vec2(800.0, 800.0)

        void main() {
            vec3 c = vec3(0.);
            float l = 1.;
            float z= 4.2 + 0.5 * abs(cos(t));
            for(int i=0; i<3; i++) {
                vec2 uv = gl_FragCoord.xy/r;
                vec2 p = (gl_FragCoord.xy/r - 0.5) * vec2(1.0, r.x / r.y);
                z -= abs(tan(t) / 250.0);
                l = length(p);
                uv += p/l*(sin(z)+1.) * abs(sin(l)+tan(z*.5));
                c[i] += abs(tan(t) / 55.0) / (1.7 - texture(u_texture, uv));
            }

            vec3 v = vec3(.0);
            if (abs(rand(vec2(gl_FragCoord.x, gl_FragCoord.y))) < 0.001) { v += vec3(0.4, 0.3, 0.4); }
            if (abs(rand(vec2(gl_FragCoord.y, gl_FragCoord.x))) < 0.001) { v += vec3(0.3, 0.3, 0.4); }
            if (abs(rand(vec2(800. - gl_FragCoord.x, gl_FragCoord.y))) < 0.001) { v += vec3(0.2, 0.3, 0.4); }
            if (abs(rand(vec2(800. - gl_FragCoord.y, gl_FragCoord.x))) < 0.001) { v += vec3(0.6, 0.6, 0.6); }

            if (abs(rand(vec2(floor((gl_FragCoord.x - 400.) * abs(cos(t)) + 400.), floor((gl_FragCoord.y - 400.) * abs(cos(t)) + 400.) ))) < 0.001) { v += vec3(0.2, 0.2, 0.2); }

            outputColor = vec4(c/l + v, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    Texture texture = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        MVPLocation = shader.uniformLocation("MVP");
        texture.bind(0, shader, "u_texture");

        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(18.0/255.0, 32.0/255.0, 42.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, + -1.0f / tanf(radians(22.5f)));
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * translate(mat4(1.0f), vec3(0, 0, 0)) )[0][0]);
        quad->render();

    }

    void drawStar() {

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};