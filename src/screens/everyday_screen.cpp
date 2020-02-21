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

        void main() {
            v_pos = a_pos;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";


    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;
        #define PI 3.1415926535

        out vec4 outputColor;

        in vec3 v_pos;
        uniform float u_time;

        const vec2 u_resolution = vec2(800.0, 800.0);


        float circle(vec2 samplePosition, float radius){
            return length(samplePosition) - radius;
        }

        float rectangle(vec2 samplePosition, vec2 halfSize){
            vec2 componentWiseEdgeDistance = abs(samplePosition) - halfSize;
            float outsideDistance = length(max(componentWiseEdgeDistance, 0.0));
            float insideDistance = min(max(componentWiseEdgeDistance.x, componentWiseEdgeDistance.y), 0.0);
            return outsideDistance + insideDistance;
        }


        vec2 rotate(vec2 samplePosition, float rotation){
            float angle = rotation * PI * 2.0 * -1.0;
            float sine = sin(angle);
            float cosine = cos(angle);
            return vec2(cosine * samplePosition.x + sine * samplePosition.y, cosine * samplePosition.y - sine * samplePosition.x);
        }

        vec2 translate(vec2 samplePosition, vec2 offset){
            return samplePosition - offset;
        }

        vec2 scale(vec2 samplePosition, float scale){
            return samplePosition / scale;
        }

        vec3 hsv2rgb_smooth( in vec3 c )
        {
            vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

            rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing

            return c.z * mix( vec3(1.0), rgb, c.y);
        }

        float f( float H, float Sl, float L, float n )
        {
            float k = mod((n + H/30.0), 30.0);
            float a = Sl * min(L, 1.0-L);
            return L - a * max(-1.0, min(min(k-3.0, 9.0-k), 1.0));
        }

        // https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB_alternative
        vec3 hsl2rgb( in vec3 c )
        {
            float H = mod(c.x, 1.0) * 360.0;
            float Sl = mod(c.y, 1.0);
            float L = mod(c.z, 1.0);

            float R = f(H,Sl,L, 0.0);
            float G = f(H,Sl,L, 8.0);
            float B = f(H,Sl,L, 4.0);

            return vec3(R, G, B);
        }

        void main( void )
        {
            vec2 pos = gl_FragCoord.xy/vec2(800.0);

            outputColor = vec4(hsl2rgb(vec3(pos + vec2(u_time, 0), (sin(u_time) + 1.0)*0.5)), 1.0);
        }

)glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/fire.vert", "shaders/fire.frag");
    ShaderProgram fireShader = ShaderProgram(vertSource, fragSource);

//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

//    ShaderProgram postProcessingShader = ShaderProgram::fromAssetFiles("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    GLuint MVP, u_resolution;
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
        time += deltaTime * 0.2;
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
            camera.position = vec3(3, 3, 3);
            camera.lookAt(vec3(0.0, 0.0, 0.0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

//        screenFbo.bind();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//            shader.use();
//            glUniformMatrix4fv(shader.uniformLocation("M"), 1, GL_FALSE, &(translate(mat4(1), vec3(0,0,0)))[0][0]);
//            glUniformMatrix4fv(shader.uniformLocation("VP"), 1, GL_FALSE, &(Camera::main->combined)[0][0]);
//            glUniform1f(shader.uniformLocation("u_time"), time);
//            glUniform3f(shader.uniformLocation("eye_position"), camera.position.x, camera.position.y, camera.position.z);
//            cube->render();
//            vec3 v = vec3(sin(time), cos(time), 0);
//            gizmos.drawLine(1.5f * v, -1.5f * v, vec4(0.71, 0.1, 0.1, 1.0));

//        screenFbo.unbind();

        //////////////////////////////////////////////////////////////////////////////////////////////// POST PROCESSING

        camera.saveState();
        camera.position = vec3(0, 0, 1 / tan(radians(camera.fov * 0.5)));
        camera.lookAt(vec3(0, 0, 0));
        camera.Camera::update();
//
        fireShader.use();
        glUniformMatrix4fv(fireShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined)[0][0]);
        glUniform1f(fireShader.uniformLocation("u_time"), time);
//        glUniform2f(u_resolution, camera.width, camera.height);
//        screenFbo.colorTexture->bind(0, postProcessingShader, "u_texture");
//        screenFbo.depthTexture->bind(1, postProcessingShader, "u_depth");
        quad->render();
        camera.restoreState();

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