//
// Created by sneeuwpop on 18-11-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

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

        out vec3 v_pos;

        void main() {
            v_pos = a_pos;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;

        in vec3 v_pos;

        void main() {
            vec3 light_pos = vec3(sin(u_time) * 3.0, 3.0, cos(u_time) * 3.0);
            float NdotL1 = dot(normalize(v_pos), normalize(light_pos));

            if(NdotL1 > 0.5) {
                outputColor        =  vec4(50.0/255.0, 165.0/255.0, 65.0/255.0, 1.0);
            } else if(NdotL1 > 0.2) {
                outputColor        =  vec4(28.0/255.0, 135.0/255.0, 80.0/255.0, 1.0);
            } else {
                outputColor        =  vec4(24.0/255.0, 62.0/255.0, 46.0/255.0, 1.0);
            }
//            outputColor = vec4(v_color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
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


    void setup(GLFWwindow* window)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        const char* glsl_version = "#version 300 es"; // TODO: We should not hardcoded that here

        // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32 and imgui_impl_dx11)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        shader.use();
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(0.3 * 171.0/255.0, 0.3 * 150.0/255.0, 0.3 * 83.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(7, 3, 7);
            camera.lookAt(vec3(0, 2, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * translate(mat4(1), vec3(0,0,0)))[0][0]);
        sphere->render();

        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1), vec3(sin(time)*3,3,cos(time)*3)), 0.5f * vec3(1)))[0][0]);
        glUniform4f(flatShader.uniformLocation("u_color"), 1.0, 1.0, 0.8, 1.0);
        sphere->render();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
//        shader.use();
//        VertexBuffer::bindDefault();
//
//        // Feed inputs to dear imgui, start new frame
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        ImGui::Begin("LSystem pattern");
//        ImGui::SliderFloat("m", &m, -10, 10);
//        ImGui::SliderFloat("n1", &n1, -10, 10);
//        ImGui::SliderFloat("n2", &n2, -10, 10);
//        ImGui::SliderFloat("n3", &n3, -10, 10);
//        ImGui::End();
//
//        // Render dear imgui into screen
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};