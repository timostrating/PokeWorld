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

        out vec3 v_pos;

        void main() {
            v_pos = a_pos;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;
        uniform vec3 u_camPos;

        in vec3 v_pos;

        void main() {
            vec3 light_pos = vec3(sin(u_time) * 3.0, 3.0, cos(u_time) * 3.0);

            vec3 color1 = vec3(50.0/255.0, 165.0/255.0, 65.0/255.0);
            vec3 color2 = vec3(28.0/255.0, 135.0/255.0, 80.0/255.0);
            vec3 color3 = vec3(21.0/255.0, 50.0/255.0, 36.0/255.0);

            vec3 color = mix(color2, color1, (v_pos.y - 1.) * 10.0);

            outputColor = vec4(color, 1.0);
        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");
//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());



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

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(1.5+ sin(time*0.6)*2, 2, 1.5+ cos(time*0.6)*2);
            camera.lookAt(vec3(1.5, 1, 1.5));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST



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