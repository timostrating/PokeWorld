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

using namespace glm;


class Tmp2 : public GameObject
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;
        uniform float u_time;

        out vec4 color;

        float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
        vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
        vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

        float noise(vec3 p){
            vec3 a = floor(p);
            vec3 d = p - a;
            d = d * d * (3.0 - 2.0 * d);

            vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
            vec4 k1 = perm(b.xyxy);
            vec4 k2 = perm(k1.xyxy + b.zzww);

            vec4 c = k2 + a.zzzz;
            vec4 k3 = perm(c);
            vec4 k4 = perm(c + 1.0);

            vec4 o1 = fract(k3 * (1.0 / 41.0));
            vec4 o2 = fract(k4 * (1.0 / 41.0));

            vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
            vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

            return o4.y * d.y + o4.x * (1.0 - d.y);
        }

        const int FBM_OCTAVES = 3;
        const float H = 1.5;

        // http://iquilezles.org/www/articles/fbm/fbm.htm
        float fbm(vec3 x)
        {
            float G = exp2(-H);
            float f = 1.0;
            float a = 1.0;
            float t = 0.0;
            for( int i=0; i<FBM_OCTAVES; i++ )
            {
                t += a*noise(f*x);
                f *= 2.0;
                a *= G;
            }
            return t;
        }

        float pattern( in vec3 p )
        {
            vec3 q = vec3( fbm( p + vec3(0.0, 0.0, 0.0) ), fbm( p + vec3(5.2,1.3,2.5) ), fbm(vec3(3.4,2.1,7.1) ) );

            return fbm( p + 1.5 * q ) * 0.7;
        }

        const vec3 grass1 = vec3(112.0/255.0, 139.0/255.0,  70.0/255.0);
        const vec3 grass2 = vec3( 50.0/255.0,  86.0/255.0,  49.0/255.0);
        const vec3 v_normal = vec3(0.0, 1.0, 0.0);

        void main() {
            float a = pattern(a_pos);
            float b = pattern(a_pos * 5.0);
            float r = mix(a, b, 0.2);

            vec3 grass = mix(grass1, grass2*0.8, r);

            vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(1.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
            vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(0.0/255.0,   10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

            vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
            float NdotL1 = dot(normalize(v_normal), normalize(light_pos));
            vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

            color = vec4(ambiend * grass, 1.0);

            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;
        in vec4 color;

        void main() {
            outputColor = vec4(color);
        })glsl";

public:
    mat4 transform = scale(translate(mat4(1), vec3(1, 1, 1)), 0.2f * vec3(1));
    ShaderProgram flatShader = ShaderProgram(vertSource, fragSource);

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
        glUniform1f(flatShader.uniformLocation("u_time"), time);

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
        new Route(),
        new Tree(translate(mat4(1), vec3(-2,1,0))),
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
        time += deltaTime * 0.1;

        colorPickerSystem->update(deltaTime);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB)) debug = true;
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_1)) renderGUI = true;

        if (debug) { camera.debugUpdate(deltaTime); }   // free camera
        else       { camera.update(deltaTime);}         // normal camera


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


            if (debug) {
                camera.debugDraw();
                for (auto &go : gameObjects)
                    go->debugRender();
                terrain->debugRender();
            }
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