//
// Created by sneeuwpop on 18-11-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/splines/line.h"
#include "../graphics/frame_buffer.h"

using namespace glm;
using namespace MATH;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec3 v_pos;
        out mat4 v_MVP;

        void main() {
            v_pos = a_pos;
            v_MVP = MVP;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";


    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;
        #define PI 3.1415926535

        out vec4 outputColor;

        in vec3 v_pos;
        in mat4 v_MVP;
        uniform float u_time;

        const vec2 u_resolution = vec2(800.0, 800.0);

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

        float clamp01(float v) { return clamp(v, 0.0, 1.0); }

        vec3 green = vec3(57.0/255.0, 109.0/255.0, 23.0/255.0);
        vec3 between = vec3(148.0/255.0, 130.0/255.0, 19.0/255.0);
        vec3 brown = vec3(115.0/255.0, 57.0/255.0, 28.0/255.0);


        const int MAX_MARCHING_STEPS = 255;
        const float MIN_DIST = 0.0;
        const float MAX_DIST = 100.0;
        const float EPSILON = 0.0001;

        float sphereSDF(vec3 p) {
            return length(p) - 0.3;
        }

        float sceneSDF(vec3 samplePoint) {
            return sphereSDF(samplePoint) - 0.2 * noise( vec3(u_time) + samplePoint * 3.0);
        }

        vec3 estimateNormal(vec3 p) {
            return normalize(vec3(
                sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
                sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
                sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
            ));
        }

        float shortestDistanceToSurface(vec3 eye, vec3 marchingDirection, float start, float end) {
            float depth = start;
            for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
                float dist = sceneSDF(eye + depth * marchingDirection);
                if (dist < EPSILON) {
                    return depth;
                }
                depth += dist;
                if (depth >= end) {
                    return end;
                }
            }
            return end;
        }

        vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
            vec2 xy = fragCoord - size / 2.0;
            float z = size.y / tan(radians(fieldOfView) / 2.0);
            return normalize(vec3(xy, -z));
        }

        const vec3 lightColor = vec3(1.0);
        const vec3 objectColor = vec3(0., 1., 0.);
//        const vec3 ambient = vec3(0.1);

        void main( void )
        {

            vec3 dir = rayDirection(45.0, vec2(800.), gl_FragCoord.xy);
            vec3 eye = vec3(0.0, 0.0, 5.0);
            float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);

            if (dist > MAX_DIST - EPSILON) {
                // Didn't hit anything
                outputColor = vec4(0.1, 0.1, 0.1, 1.0);
                return;
            }

            vec3 lightDir = vec3(sin(u_time * 0.5) * 3., 3., cos(u_time * 0.5) * 3.);
            vec3 samplePoint = eye + dir * dist;
            vec3 normal = estimateNormal(samplePoint);

            float diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
            vec3 ambient = vec3(1. - noise( vec3(u_time) + samplePoint * 3.0));
            vec3 result = (ambient + diffuse) * objectColor;

            outputColor = vec4(result, 1.0);
        }

)glsl";

    FlyingCamera camera = FlyingCamera();

//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/fire.vert", "shaders/fire.frag");
    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

//    ShaderProgram postProcessingShader = ShaderProgram::fromAssetFiles("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    GLint MVP, u_resolution;
//    FrameBuffer screenFbo = FrameBuffer(800, 800);


public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
        VertexBuffer::uploadSingleMesh(sphere);
//        texture.bind(0, shader, "u_texture");

//        MVP = postProcessingShader.uniformLocation("MVP");
//        u_resolution = postProcessingShader.uniformLocation("u_resolution");
//        screenFbo.addColorTexture();
//        screenFbo.addDepthTexture();
//        screenFbo.unbind();
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

    void render(double deltaTime) {
        time += deltaTime;
        shader.use();
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(5, 5, 5);
            camera.lookAt(vec3(0.0, -0.2, 0.0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

//        screenFbo.bind();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(rotate(glm::scale(Camera::main->combined, vec3(2,2,2)), radians(static_cast<float>(time) * 15.0f), VEC3::Y))[0][0]);
            cube->render();

//        screenFbo.unbind();



        //////////////////////////////////////////////////////////////////////////////////////////////// POST PROCESSING

//        camera.saveState();
//        camera.position = vec3(0, 0, 1 / tan(radians(camera.fov * 0.5)));
//        camera.lookAt(vec3(0, 0, 0));
//        camera.Camera::update();
////
//        fireShader.use();
//        glUniformMatrix4fv(fireShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined)[0][0]);
//        glUniform1f(fireShader.uniformLocation("u_time"), time);
////        glUniform2f(u_resolution, camera.width, camera.height);
////        screenFbo.colorTexture->bind(0, postProcessingShader, "u_texture");
////        screenFbo.depthTexture->bind(1, postProcessingShader, "u_depth");
//        quad->render();
//        camera.restoreState();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
//        shader.use();
//        VertexBuffer::bindDefault();
//
//        // Feed inputs to dear imgui, start new frame
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        ImGui::Begin("LSystem pattern");
//        ImGui::SliderFloat("m", &m, -10, 10);
//        ImGui::SliderFloat("n1", &n1, -10, 10);
//        ImGui::SliderFloat("n2", &n2, -10, 10);
//        ImGui::SliderFloat("n3", &n3, -10, 10);
//        ImGui::End();
//
//        // Render dear imgui into screen
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};