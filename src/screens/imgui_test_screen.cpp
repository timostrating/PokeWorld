//
// Created by sneeuwpop on 18-6-19.
//

#include "imgui.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "../util/interfaces/screen.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

class ImGuiTestScreen : public Screen
{

    double time = 0;

public:
    ImGuiTestScreen()
   {
   }

    void setup(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        const char* glsl_version = "#version 300 es"; // TODO: We should not hardcoded that here

        // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32 and imgui_impl_dx11)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    bool my_tool_active = true;
    bool showDemoWindow = true;


    void render(double deltaTime) {
        time += deltaTime;

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Another Window", &my_tool_active);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            my_tool_active = false;
        ImGui::End();

        // Any application code here
        ImGui::ShowDemoWindow(&showDemoWindow);

        // Render dear imgui into screen
        ImGui::Render();
        glClearColor(fmod(time, 1), 0.0f, fmod(time * 2, 1), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ~ImGuiTestScreen()
    {
        ImGui_ImplGlfw_Shutdown();
    }
};