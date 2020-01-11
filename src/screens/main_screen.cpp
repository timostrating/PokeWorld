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
#include "../game/systems/sky_system.h"
#include "../game/systems/color_picker_system.h"

using namespace glm;

class Tmp : public GameObject {
public:
    SharedMesh cube = SharedMesh(Mesh::cube());
    vec3 random = MATH::randomVec3(-10, 10);
    mat4 transform = scale(translate(mat4(1), random), MATH::random(0.1, 0.3) * vec3(1));
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");
    bool clicked = false;

    Tmp() {
        VertexBuffer::uploadSingleMesh(cube);
        colorPickerData = new ColorPickerData {cube, transform};
    }

    void render() {
        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
        if (clicked)
            glUniform4f(flatShader.uniformLocation("u_color"), 1, 0, 1, 1);
        else
            glUniform4f(flatShader.uniformLocation("u_color"), MATH::remap(random.x, -10, 10, 0, 1), MATH::remap(random.y, -10, 10, 0, 1), MATH::remap(random.z, -10, 10, 0, 1), 1);
        cube->render();
    }

    void onHover() {
        random = MATH::randomVec3(-10, 10);
    }

    void onClick() {
        clicked = true;
    }
};

class MainScreen : public Screen
{

public:
    FlyingCamera camera = FlyingCamera();
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/flat_color.vert", "shaders/lib/flat_color.frag");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());

    SkySystem* skySystem = new SkySystem();
    ColorPickerSystem* colorPickerSystem = new ColorPickerSystem();

    std::vector<GameObject*> gameObjects = {
//        new MarchingCubesTerrain(),
//        new WaterPlane(),
    };

    MainScreen()
    {
        VertexBuffer::uploadSingleMesh(cube);

        for (int i=0; i<1000; i++)
            gameObjects.insert(gameObjects.begin(), new Tmp());

        colorPickerSystem->setClickAbles(&gameObjects);
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

    void render(double deltaTime)
    {
        time += deltaTime;

        colorPickerSystem->update();

        glClearColor(100/255.0, 100/255.0, 100/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skySystem->update();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
            for (auto &go : gameObjects)
                go->debugRender();

        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *30,  12,  cos(time * 0.5) *30);
//            camera.position = vec3(15,  10,  15);
            camera.lookAt(VEC3::Y);
            camera.Camera::update();
        }

        flatShader.use();
        for (auto &go : gameObjects)
            go->render();

//        flatShader.use();
//        srand(0); // trees
//        for (int i=0; i<200; i++)
//            drawTree(MATH::random(-100,100), MATH::random(-100,100));


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        flatShader.use();
        VertexBuffer::bindDefault();

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

//        ImGui::Begin("LSystem pattern");
//            for (auto &go : gameObjects)
//                go->renderGui();
//        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};