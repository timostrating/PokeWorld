//
// Created by sneeuwpop on 18-12-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <vector>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../graphics/cubemap.h"
#include "../game/marching_cubes_terrain.h"
#include "../game/water_plane.h"
#include "../game/sky.h"
#include "../game/systems/color_picker_system.h"
#include "../game/systems/water_system.h"
#include "../game/stadium.h"
#include "../util/input/mouse.h"

using namespace glm;

class Tmp : public GameObject {
public:
    SharedMesh sphere = SharedMesh(Mesh::sphere());
    mat4 transform = scale(translate(mat4(1), vec3(-15, 1, 15)), MATH::random(0.1, 0.3) * vec3(1));
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    float color = 1;
    bool clicked = false;

    Tmp() {
        VertexBuffer::uploadSingleMesh(sphere);
        colorPickerData = new ColorPickerData {sphere, transform};
    }

    void render() {
        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
        if (clicked)
            glUniform4f(flatShader.uniformLocation("u_color"), 1, 0, 1, 1);
        else
            glUniform4f(flatShader.uniformLocation("u_color"), 0.2, color, 0.2, 1);
        sphere->render();
    }

    void onHover() { color = MATH::random(0.5, 1); }
    void onClick() { clicked = true; }
};


class MainScreen : public Screen
{

public:
    FlyingCamera camera = FlyingCamera();
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());

    ColorPickerSystem* colorPickerSystem = new ColorPickerSystem();
    WaterSystem* waterSystem = new WaterSystem();
    SkySystem* skySystem = new SkySystem();

    MarchingCubesTerrain* terrain = new MarchingCubesTerrain();
    WaterPlane* waterPlane = new WaterPlane(waterSystem);

    std::vector<GameObject*> gameObjects = {
        new Sky(skySystem),
        new Stadium(),
        new Tmp(),
    };

    MainScreen()
    {
        VertexBuffer::uploadSingleMesh(cube);

        colorPickerSystem->setGameObjects(&gameObjects);
        waterSystem->setGameObjects(&gameObjects, terrain);

        camera.position = vec3(50,  20,  50);
        camera.lookAt(vec3(0, 0, 0));
        camera.Camera::update();
    }

    void setup(GLFWwindow* window)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        //  TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        const char* glsl_version = "#version 300 es"; // TODO: We should not hardcoded that here

        // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32 and imgui_impl_dx11)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    float time = 0;
    bool debug = false;
    bool renderGUI = false;

    float s = 0;
    float t = 1;

    float velocityX = 0;
    int oldX = INPUT::MOUSE::getMousePosX();
    int newX = oldX;

    float velocityY = 0;
    int oldY = INPUT::MOUSE::getMousePosY();
    int newY = oldY;

    float r = 50.0;

    void render(double deltaTime)
    {
        time += deltaTime;

        colorPickerSystem->update(deltaTime);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB)) debug = true;
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_1)) renderGUI = true;

        if (debug) {
            camera.update(deltaTime);
            camera.debugDraw();
            terrain->debugRender();
            for (auto &go : gameObjects)
                go->debugRender();

        } else {

            velocityX -= 0.5f * deltaTime;
            if (velocityX < 0.0f) velocityX = 0.0f;

            velocityY -= 0.5f * deltaTime;
            if (velocityY < 0.0f) velocityY = 0.0f;


            newX = INPUT::MOUSE::getMousePosX();
            if (INPUT::MOUSE::leftClick())
                velocityX = (oldX - newX) / static_cast<float>(camera.width);
            s += velocityX * PI;
            oldX = newX;

            newY = INPUT::MOUSE::getMousePosY();
            if (INPUT::MOUSE::leftClick())
                velocityY = (oldY - newY) / static_cast<float>(camera.width);
            t += velocityY * PI;
            t = clamp(t, 0.01f, 0.99f * PI);
            oldY = newY;

#ifdef __EMSCRIPTEN__
            r += INPUT::MOUSE::getScrollDelta() * 5.0;
#else
            r -= INPUT::MOUSE::getScrollDelta() * 5.0;
#endif
            r = clamp(r, 20.0f, 100.0f);


            camera.position = vec3(
                    sin(s) * sin(t) * r,  // y
                    cos(t) * r,           // z
                    cos(s) * sin(t) * r   // x
                );
            camera.lookAt(vec3(0, 0, 0));
            camera.Camera::update();
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////// SYSTEMS

        skySystem->update(deltaTime);

        /////////////////////////////////////////////////////////////////////////////////////////////////// GAME OBJECTS

        flatShader.use();
        terrain->render(time);
        for (auto &go : gameObjects)
            go->render();

        waterSystem->update(deltaTime);
        waterPlane->render();



        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        flatShader.use();
        VertexBuffer::bindDefault();

        if (renderGUI == false) { return; }

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Systems");
            waterSystem->renderGUI();
            ImGui::Separator();
            colorPickerSystem->renderGUI();
        ImGui::End();

        ImGui::Begin("GameObjects");
            terrain->renderGui();
            waterPlane->renderGui();
            for (auto &go : gameObjects) {
                go->renderGui();
                ImGui::Separator();
            }
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