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
#include "../game/marching_cubes_terrain.h"
#include "../game/water_plane.h"
#include "../game/sky.h"
#include "../game/systems/color_picker_system.h"
#include "../game/systems/water_system.h"
#include "../game/stadium.h"
#include "../game/tree.h"
#include "../game/route.h"
#include "../game/example_tree.h"

using namespace glm;

class Tmp2 : public GameObject
{
public:
    SharedMesh sphere = SharedMesh(Mesh::sphere());
    mat4 transform = scale(translate(mat4(1), vec3(-16, 1, 16)), 0.7f * vec3(1));
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    bool hover = false;
    bool* highSpeed;

    Tmp2(bool *highSpeed) : highSpeed(highSpeed)
    {
        VertexBuffer::uploadSingleMesh(sphere);
        colorPickerData = new ColorPickerData {sphere, transform};
    }

    void render(float time) {
        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
        if (hover) {
            if (*highSpeed)     {glUniform4f(flatShader.uniformLocation("u_color"), 0.4, 0.4, 1.0, 1);}
            else                {glUniform4f(flatShader.uniformLocation("u_color"), 0.4, 0.4, 0.7, 1);}
        } else {
            if (*highSpeed)     {glUniform4f(flatShader.uniformLocation("u_color"), 0.2, 0.2, 0.8, 1);}
            else                {glUniform4f(flatShader.uniformLocation("u_color"), 0.2, 0.2, 0.5, 1);}
        }
        sphere->render();

        hover = false;
    }
    void renderReflection(float time) { bool tmp = hover; render(time); hover = tmp; }

    void onHover() { hover = true; }
    void onClick() { *highSpeed = !(*highSpeed); }
};

class Tmp : public GameObject
{
public:
    SharedMesh sphere = SharedMesh(Mesh::sphere());
    mat4 transform = scale(translate(mat4(1), vec3(-4, 1, -1)), vec3(1));
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    bool hover = false;
    bool* renderTree;

    Tmp(bool *renderTree) : renderTree(renderTree)
    {
        VertexBuffer::uploadSingleMesh(sphere);
        colorPickerData = new ColorPickerData {sphere, transform};
    }

    void render(float time) {
        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
        if (hover) {
            if (*renderTree)    {glUniform4f(flatShader.uniformLocation("u_color"), 0.4, 1.0, 0.4, 1);}
            else                {glUniform4f(flatShader.uniformLocation("u_color"), 0.4, 0.7, 0.4, 1);}
        } else {
            if (*renderTree)    {glUniform4f(flatShader.uniformLocation("u_color"), 0.2, 0.8, 0.2, 1);}
            else                {glUniform4f(flatShader.uniformLocation("u_color"), 0.2, 0.5, 0.2, 1);}
        }
        sphere->render();

        hover = false;
    }
    void renderReflection(float time) { bool tmp = hover; render(time); hover = tmp; }

    void onHover() { hover = true; }
    void onClick() { *renderTree = !(*renderTree); }
};





class MainScreen : public Screen
{

public:
    FlyingCamera camera = FlyingCamera();
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    Gizmos gizmos;

    ColorPickerSystem* colorPickerSystem = new ColorPickerSystem();
    WaterSystem* waterSystem = new WaterSystem();

    MarchingCubesTerrain* terrain = new MarchingCubesTerrain();
    WaterPlane* waterPlane = new WaterPlane(waterSystem);

    bool debug = false;
    bool renderGUI = false;
    bool renderTree = false;
    bool highSpeed = false;
    ExampleTree* exampleTree = new ExampleTree();


    std::vector<GameObject*> gameObjects = {
        new Stadium(&debug),
        new Tmp(&renderTree),
        new Tmp2(&highSpeed),
        new Route(),
    };

    ShaderProgram postProcessingShader = ShaderProgram::fromAssetFiles("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    GLuint MVP, u_resolution;
    SharedMesh quad = SharedMesh(Mesh::quad());
    FrameBuffer screenFbo = FrameBuffer(1600, 1600);

    vec3 randomPointOnMap() {
        float x = random(-20,20);
        float z = random(-20,20);

        if (x<0 && z>-5) return randomPointOnMap(); // water
        if (x>5 && x<17 && z>0 && z<17) return randomPointOnMap(); // stadium

        return vec3(x,1,z);
    }
    MainScreen()
    {
        for (int i=0; i<60; i++) { gameObjects.insert(gameObjects.begin(), { new Tree(randomPointOnMap(), Tree::EXAMPLES[i % Tree::EXAMPLE_SIZE]) }); }
        gameObjects.insert(gameObjects.begin(), {new Sky()});

        colorPickerSystem->setGameObjects(&gameObjects);
        waterSystem->setGameObjects(&gameObjects, terrain);

        camera.position = vec3(50,  20,  50);
        camera.lookAt(vec3(0, 0, 0));
        camera.Camera::update();

        MVP = postProcessingShader.uniformLocation("MVP");
        u_resolution = postProcessingShader.uniformLocation("u_resolution");
        VertexBuffer::uploadSingleMesh(quad);
        screenFbo.addColorTexture();
        screenFbo.addDepthTexture();
        screenFbo.unbind();
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

    void render(double deltaTime)
    {
        time += deltaTime * 0.2; //((highSpeed)? 1.0 : 0.1);

        colorPickerSystem->update(deltaTime);

//        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB)) {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_1)) debug = true;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_2)) debug = false;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_3)) renderGUI = true;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_4)) renderGUI = false;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_5)) highSpeed = true;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_6)) highSpeed = false;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_Q)) renderTree = true;
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_W)) renderTree = false;
        }

//        if (debug) { camera.debugUpdate(deltaTime); }   // free camera
//        else       { camera.update(deltaTime);}         // normal camera
        camera.update(deltaTime);

        //////////////////////////////////////////////////////////////////////////////////////////////////////// SYSTEMS

        waterSystem->time = time;
        waterSystem->update(deltaTime);


        /////////////////////////////////////////////////////////////////////////////////////////////////// GAME OBJECTS

        screenFbo.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            flatShader.use();
            terrain->render(time);
            for (auto &go : gameObjects)
                go->render(time);

            waterPlane->render(time);
        screenFbo.unbind();


        //////////////////////////////////////////////////////////////////////////////////////////////// POST PROCESSING

        camera.saveState();
        camera.position = vec3(0, 0, 1 / tan(radians(camera.fov * 0.5)));
        camera.lookAt(vec3(0, 0, 0));
        camera.Camera::update();
//
        postProcessingShader.use();
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &(camera.combined * scale(mat4(1), vec3(camera.width / static_cast<float>(camera.height), 1.0, 1.0)))[0][0]);
        glUniform2f(u_resolution, camera.width, camera.height);
        screenFbo.colorTexture->bind(0, postProcessingShader, "u_texture");
        screenFbo.depthTexture->bind(1, postProcessingShader, "u_depth");
        quad->render();
        camera.restoreState();


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI

        if (renderTree) {
            glDisable(GL_DEPTH_TEST);
            exampleTree->render();
            glEnable(GL_DEPTH_TEST);
        }

        if (debug) {
            glDisable(GL_DEPTH_TEST);
//            camera.debugDraw();
            for (auto &go : gameObjects)
                go->debugRender(&gizmos);
            terrain->debugRender(&gizmos);
            glEnable(GL_DEPTH_TEST);
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI

        flatShader.use();
        VertexBuffer::bindDefault();

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (renderTree) {
            exampleTree->renderGui();
        }

        if (renderGUI) {
            ImGui::Begin("Systems");
                screenFbo.renderGUI();
                ImGui::Separator();
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
        }

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};