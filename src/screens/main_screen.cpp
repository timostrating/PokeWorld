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

using namespace glm;

class GameObject
{
public:
    mat4 modelMatrix = mat4(1.0f); // identity matrix
    ShaderProgram *shader;
    SharedMesh mesh;

    GLuint MVPLocation;

    GameObject(SharedMesh mesh, ShaderProgram *shader) : mesh(mesh), shader(shader)
    {
        MVPLocation = shader->uniformLocation("MVP");
        VertexBuffer::uploadSingleMesh(mesh);
        modelMatrix = translate(modelMatrix, MATH::randomVec3(-1, 1));
    }

    void render()
    {
        shader->use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(Camera::main->combined * modelMatrix)[0][0]);
        mesh->render();
    }
};


class MainScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    ShaderProgram defaultShaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");

    Gizmos gizmos;

    std::vector<GameObject> gameObjects;


    MainScreen()
    {
        // Shader Program
        defaultShaderProgram.use();
        gameObjects.insert(gameObjects.begin(), GameObject(SharedMesh(Mesh::cube()), &defaultShaderProgram));
        gameObjects.insert(gameObjects.begin(), GameObject(SharedMesh(Mesh::cube()), &defaultShaderProgram));
        gameObjects.insert(gameObjects.begin(), GameObject(SharedMesh(Mesh::cube()), &defaultShaderProgram));
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

        for (auto &go : gameObjects)
        {
            go.render();
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        defaultShaderProgram.use();
        VertexBuffer::bindDefault();

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("LSystem pattern");
            // todo
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