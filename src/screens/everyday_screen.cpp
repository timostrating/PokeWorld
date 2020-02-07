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

        uniform mat4 M;
        uniform mat4 VP;

        out vec3 world_pos;
        out vec3 world_normal;

        void main() {
            vec3 in_normal = a_pos;  // HACK
            world_pos = mat3(M) * a_pos; //careful here
            world_normal = normalize(mat3(M) * in_normal);

            gl_Position = M * VP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        uniform float u_time;
        uniform vec3 eye_position;

        const vec3 DiffuseLight = vec3(0.25, 0.15, 0.1);
        const vec3 RimColor  = 0.1 * vec3(0.15, 0.2, 0.25);

        const float gamma = 1.0/0.6; //higher gamma to get a darker image

        in vec3 world_pos;
        in vec3 world_normal;

        void main(){
            vec3 light_position = vec3(sin(u_time), 2.0, cos(u_time));
            //get light an view directions
            vec3 L = normalize( light_position - world_pos);
            vec3 V = normalize( eye_position - world_pos);

            //diffuse lighting
            vec3 diffuse = DiffuseLight * max(0.0, dot(L,world_normal));

            //rim lighting
            float rim = 1.0 - max(dot(V, world_normal), 0.0);
            rim = smoothstep(0.6, 1.0, rim);
            vec3 finalRim = RimColor * vec3(rim, rim, rim);

            vec3 finalColor = finalRim + diffuse + vec3(110.0/255.0, 120.0/255.0, 130.0/255.0);

            vec3 finalColorGamma = vec3(pow(finalColor.r, gamma), pow(finalColor.g, gamma), pow(finalColor.b, gamma));

            outputColor = vec4(finalColorGamma, 1);

        })glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
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

        glClearColor(50/255.0, 50/255.0, 50/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(1.5, 2, 1.5);
            camera.lookAt(vec3(0.0, 0.5, 0.0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(shader.uniformLocation("M"), 1, GL_FALSE, &(translate(mat4(1), vec3(0,0,0)))[0][0]);
        glUniformMatrix4fv(shader.uniformLocation("VP"), 1, GL_FALSE, &(Camera::main->combined)[0][0]);
        glUniform1f(shader.uniformLocation("u_time"), time);
        glUniform3f(shader.uniformLocation("eye_position"), camera.position.x, camera.position.y, camera.position.z);
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