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
#include "../game/marching_cubes_terrain.h"
#include "../graphics/cubemap.h"

using namespace glm;

class TreeScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    ShaderProgram defaultShaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");

    Gizmos gizmos;

    const static int cacheSize = 100;
    vec3 plusCache[cacheSize];
    vec3 minCache[cacheSize];

    const float angle = radians(18.f);

//    ShaderProgram skyShader = ShaderProgram::fromAssetFiles("shaders/lib/skybox.vert", "shaders/lib/skybox.frag");
//    std::vector<std::string> faces = {
//            "../../../../assets/textures/test/skybox/right.jpg",
//            "../../../../assets/textures/test/skybox/left.jpg",
//            "../../../../assets/textures/test/skybox/top.jpg",
//            "../../../../assets/textures/test/skybox/bottom.jpg",
//            "../../../../assets/textures/test/skybox/front.jpg",
//            "../../../../assets/textures/test/skybox/back.jpg"
//    };
//    Cubemap* skycubemap = new Cubemap(faces);
//    SharedMesh skybox = SharedMesh(Mesh::skybox());


    TreeScreen()
    {
        for (int i=0; i<cacheSize; i++) {
            plusCache[i] = MATH::randomPointOnSphere(70, 180);
            minCache[i] = MATH::randomPointOnSphere(70, -180);
        }
        // Shader Program
        defaultShaderProgram.use();

        // Model View Projection
        MVPLocation = defaultShaderProgram.uniformLocation("MVP");
//        VertexBuffer::uploadSingleMesh(skybox);
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

    std::string buf = std::string("F[-x[+x]++x]") + std::string(1000, '\0');
    int applyNtimes = 5;
    bool twoD = false;
    float growthSize = 0.2f;

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        defaultShaderProgram.use(); // imgui may have changed the current shader

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *5,  2,  cos(time * 0.5) *5);
            camera.lookAt(VEC3::Y);
            camera.Camera::update();
        }


//        skyShader.use();
//        glUniformMatrix4fv(skyShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 100.0f * VEC3::ONE))[0][0]);
//        skybox->render();

        mat4 mvp = camera.combined * modelMatrix;
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &mvp[0][0]);


        //////////////////////////////////////////////////////////////////////////////////////////////////////// LSYSTEM
        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', buf); // "F[-X][+X],FX"

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 direction = VEC3::Y;

        std::stack<vec3> memory = std::stack<vec3>();

        lSystem.applyNtimes(applyNtimes);
        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (toupper(str[i])) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (direction) * growthSize; drawLine(oldPoint, curPoint, COLOR::BLACK); break;
                case '+': direction = glm::rotate(direction, angle, VEC3::Z); break;
                case '-': direction = glm::rotate(direction, -angle, VEC3::Z); break;
                case '/': direction = glm::rotate(direction, angle, VEC3::X); break;
                case '\\': direction = glm::rotate(direction, -angle, VEC3::X); break;
                case '!': direction = twoD ? vec3(plusCache[i % cacheSize].x, plusCache[i % cacheSize].y, 0) : plusCache[i % cacheSize]; break;
                case '&': direction = twoD ? vec3(minCache[i % cacheSize].x, minCache[i % cacheSize].y, 0) : minCache[1 % cacheSize]; break;
                case '[': memory.push(curPoint); memory.push(direction); break;
                case ']':
                    if (memory.empty()) break;
                    direction = memory.top(); memory.pop();
                    curPoint = memory.top(); memory.pop(); break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        defaultShaderProgram.use();
        VertexBuffer::bindDefault();

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
        ImGui::Text(" X : Replace this X with the pattern");
        ImGui::Text(" + : Rotate right \n - : Rotate left");
        if(!twoD) ImGui::Text(" / : Yaw right \n \\ : Yaw left");
        ImGui::Text(" ! : Random rightish \n & : Random leftisch");
        ImGui::Text(" [ : Store current location \n ] : Pop last stored location");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        //        char *buf = new char[10];
        ImGui::InputText("pattern", &buf[0], buf.size()+ 1 );
        ImGui::Text("Times to apply pattern");
        ImGui::SliderInt("", &applyNtimes, 1, 6);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SliderFloat("Growth size", &growthSize, 0, 1);
        ImGui::Checkbox("2d instead of 3d", &twoD);
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void drawLine(const vec3 from, const vec3 to, const vec4 color) {
        gizmos.drawLine(from, to, color);
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};