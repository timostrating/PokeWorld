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

using namespace glm;


class Tmp2 : public GameObject
{
public:
    mat4 transform = scale(translate(mat4(1), vec3(1, 1, 1)), 0.1f * vec3(1));
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    Line line = Line({scale(translate(mat4(1), vec3( 0,  0,  0)), 1.0f * VEC3::ONE),
                      scale(translate(mat4(1), vec3(-1,  8, -1)), 0.8f * VEC3::ONE),
                      scale(translate(mat4(1), vec3(-1, 12, -1)), 0.1f * VEC3::ONE)});

    Line line2= Line({scale(translate(mat4(1), vec3( 0,  0,  0)), 1.2f * VEC3::ONE),
                      scale(translate(mat4(1), vec3(2,  7, 2)), 0.9f * VEC3::ONE),
                      scale(translate(mat4(1), vec3(2, 13, 2)), 0.2f * VEC3::ONE)});

    std::vector<vec3> points = {vec3(-1,0,0), vec3(0,0,1), vec3(1,0,0)};

    SharedMesh mesh = SharedMesh(line.wrapMeshAround(&points, false, false));
    SharedMesh mesh2 = SharedMesh(line2.wrapMeshAround(&points, false, false));

    Tmp2() {
        VertexBuffer::uploadSingleMesh(mesh);
        VertexBuffer::uploadSingleMesh(mesh2);
    }

    void render(float time) {
        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
        glUniform4f(flatShader.uniformLocation("u_color"), 0, 0.3, 0, 1);

        mesh->render();
        mesh2->render();
    }

};



class Tmp : public GameObject
{
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

    void render(float time) {
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

    ColorPickerSystem* colorPickerSystem = new ColorPickerSystem();
    WaterSystem* waterSystem = new WaterSystem();

    MarchingCubesTerrain* terrain = new MarchingCubesTerrain();
    WaterPlane* waterPlane = new WaterPlane(waterSystem);

    std::vector<GameObject*> gameObjects = {
        new Sky(),
        new Stadium(),
        new Tmp(),
        new Tmp2(),
        new Tree(translate(mat4(1), vec3(0,1,0))),
    };

    ShaderProgram postProcessingShader = ShaderProgram::fromAssetFiles("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    GLuint MVP, u_resolution;
    SharedMesh quad = SharedMesh(Mesh::quad());
    FrameBuffer screenFbo = FrameBuffer(1600, 1600);


    MainScreen()
    {
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
    bool debug = false;
    bool renderGUI = false;

    void render(double deltaTime)
    {
        time += deltaTime * 0.2;

        colorPickerSystem->update(deltaTime);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB)) debug = true;
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_1)) renderGUI = true;

        if (debug) {
            camera.debugUpdate(deltaTime);
            camera.debugDraw();
            terrain->debugRender();
            for (auto &go : gameObjects)
                go->debugRender();
        } else {
            camera.update(deltaTime);
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////// SYSTEMS

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