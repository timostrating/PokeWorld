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

using namespace glm;


class MainScreen : public Screen
{
    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform vec4 u_color;

        out vec3 v_color;

        void main() {
            v_color = vec3(u_color.r + (a_pos.x/100.), u_color.g + (a_pos.y/100.), u_color.b + (a_pos.z/100.));
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec3 v_color;

        void main() {
            outputColor = vec4(v_color, 1.0);
        })glsl";


public:
    FlyingCamera camera = FlyingCamera();
    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

    Gizmos gizmos;

    ShaderProgram skyShader = ShaderProgram::fromAssetFiles("shaders/lib/skybox.vert", "shaders/lib/skybox.frag");
    std::vector<std::string> faces = {
            "../../../../assets/textures/test/skybox/right.jpg",
            "../../../../assets/textures/test/skybox/left.jpg",
            "../../../../assets/textures/test/skybox/top.jpg",
            "../../../../assets/textures/test/skybox/bottom.jpg",
            "../../../../assets/textures/test/skybox/front.jpg",
            "../../../../assets/textures/test/skybox/back.jpg"
    };
    Cubemap* skycubemap = new Cubemap(faces);
    SharedMesh skybox = SharedMesh(Mesh::skybox());
    GLint MVPsky;

    MarchingCubesTerrain terrain;

    SharedMesh cube = SharedMesh(Mesh::cube());

    MainScreen()
    {
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(skybox);

        MVPsky = skyShader.uniformLocation("MVP");
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
        glClearColor(64.0/255.0, 64.0/255.0, 64.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skyShader.use();
        glUniformMatrix4fv(MVPsky, 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(0, 0, 0)), 400.0f * VEC3::ONE))[0][0]);
        skybox->render();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA


        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *250,  150,  cos(time * 0.5) *250);
            camera.lookAt(VEC3::Y);
            camera.Camera::update();
        }

        terrain.render();

        shader.use(); // imgui may have changed the current shader
        srand(0); // trees
        for (int i=0; i<200; i++)
            drawTree(MATH::random(-100,100), MATH::random(-100,100));


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
        shader.use();
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

    void drawTree(float x, float z) {
        float height = MATH::random(5, 10);
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(x, 0.5 * height, z)), vec3(1, 0.5 * height, 1)))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), height * 0.1, 0.0, 0.0, 1.0);
        cube->render();
        vec3 r = MATH::randomVec3(0.5, 1);
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(translate(mat4(1.0f), vec3(x, r.y * 0.1 + height, z)), vec3(r.x * 5, r.y * 5, r.z * 5)))[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), r.x * 0.0, r.y * 0.7, r.z * 0.3, 1.0);
        cube->render();
    }

    void drawBox(mat4 transform, vec4 color = vec4(1.0, 0.0, 1.0, 1.0))
    {
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
        glUniform4f(shader.uniformLocation("u_color"), color.r, color.g, color.b, color.a);
        cube->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};