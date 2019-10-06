//
// Created by sneeuwpop on 12-09-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <stack>
#include "imgui.h"
#include <GLFW/glfw3.h>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <iostream>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../game/lsystem.h"
#include "../util/debug/gizmos.h"

using namespace glm;

class TreeScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    ShaderProgram defaultShaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");

    Gizmos gizmos;

    TreeScreen()
    {
        // Shader Program
        defaultShaderProgram.use();

        // Model View Projection
        MVPLocation = glGetUniformLocation(defaultShaderProgram.getId(), "MVP");
    }

    void setup(GLFWwindow* window) {
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
    mat4 modelMatrix = translate(mat4(1.0f), vec3(0, 0, 0)); // identity matrix

    std::string buf = std::string("F[-X][+X]") + std::string(20, '\0');

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        defaultShaderProgram.use(); // imgui may have changed the current shader

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *3,  1,  cos(time * 0.5) *3);
            camera.lookAt(VEC3::Y);
            camera.Camera::update();
        }

        mat4 mvp = camera.combined * modelMatrix;
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &mvp[0][0]);

        //////////////////////////////////////////////////////////////////////////////////////////////////////// LSYSTEM
        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', buf); // "F[-X][+X],FX"

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 direction = vec3(0, 0.2f, 0);

        std::stack<vec3> memory = std::stack<vec3>();

        float speed = 5;
        float rotation = radians(mod(time * speed, 30.0) + 5.0);

        lSystem.applyNtimes(5);
        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (toupper(str[i])) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (direction); gizmos.drawLine(oldPoint, curPoint); break;
                case '+': direction = glm::rotate(direction, rotation, VEC3::Z); break;
                case '-': direction = glm::rotate(direction, -rotation, VEC3::Z); break;
                case '[': memory.push(curPoint); memory.push(direction); break;
                case ']': direction = memory.top(); memory.pop();
                    curPoint = memory.top(); memory.pop(); break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("LSystem pattern");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Change the text and see what happens");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text(" F : Move forward");
        ImGui::Text(" + : Rotate right \n - : Rotate left");
        ImGui::Text(" [ : Store current location \n ] : Pop last stored location");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        //        char *buf = new char[10];
        ImGui::InputText("string", &buf[0], buf.size()+ 1 );
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};