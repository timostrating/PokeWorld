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


        float scene(vec2 position) {
            vec2 pos = position;
            pos = translate(pos, vec2(0.0, -1.0));
            float pulseScale = 1.7 + 0.2*sin(u_time * PI);
            pos = scale(pos, pulseScale);
            pos = rotate(pos, (0.52 + 0.01 *sin(u_time)) * PI);
            float sceneDistance = rectangle(pos, vec2(1.0)) * pulseScale;
            sceneDistance = min(sceneDistance, circle(translate(pos, vec2(1.2, 0.0)), 1.0) * pulseScale);
            sceneDistance = min(sceneDistance, circle(translate(pos, vec2(0.0, -1.2)), 1.0) * pulseScale);

            return sceneDistance;
        }

//        void main() {
//            float dist = scene(v_pos.xy * 5.0);
//            vec3 color1 = vec3(213.0/255.0,   3.0/255.0,  25.0/255.0);
//            vec3 color2 = vec3(255.0/255.0, 195.0/255.0, 203.0/255.0);
//
//            outputColor = vec4(vec3(mix(mix(vec3(0.0), color2, smoothstep(0.5, 0.51, dist)), color1, smoothstep(0.5, 0.3, dist))), 1.0);
//        }

        // http://glslsandbox.com/e#61256.0
        vec2 uv;

        const vec2 ch_size  = vec2(1.0, 2.0) * 0.6;              // character size (X,Y)
        const vec2 ch_space = ch_size + vec2(1.0, 1.0);    // character distance Vector(X,Y)
        const vec2 ch_start = vec2 (ch_space.x, 1.); // start position
              vec2 ch_pos   = vec2 (0.0, 0.0);             // character position(X,Y)
        //      vec3 ch_color = vec3 (1.5, 0.75, 0.5);        // character color (R,G,B)
        //const vec3 bg_color = vec3 (0.2, 0.2, 0.2);        // background color (R,G,B)

        #define REPEAT_SIGN false // True/False; True=Multiple, False=Single

        /* 16 segment display...Akin to LED Display.

        Segment bit positions:
          __2__ __1__
         |\    |    /|
         | \   |   / |
         3  11 40 9  0
         |   \ | /   |
         |    \|/    |
          _12__ __8__
         |           |
         |    /|\    |
         4   / | \   7
         | 13 14  15 |
         | /   |   \ |
          __5__|__6__

        15 12 11 8 7  4 3  0
         |  | |  | |  | |  |
         0000 0000 0000 0000

        example: letter A

           12    8 7  4 3210
            |    | |  | ||||
         0001 0001 1001 1111

         binary to hex -> 0x119F
        */

        #define n0 ddigit(0x22FF);
        #define n1 ddigit(0x0281);
        #define n2 ddigit(0x1177);
        #define n3 ddigit(0x11E7);
        #define n4 ddigit(0x5508);
        #define n5 ddigit(0x11EE);
        #define n6 ddigit(0x11FE);
        #define n7 ddigit(0x2206);
        #define n8 ddigit(0x11FF);
        #define n9 ddigit(0x11EF);

        #define A ddigit(0x119F);
        #define B ddigit(0x927E);
        #define C ddigit(0x007E);
        #define D ddigit(0x44E7);
        #define E ddigit(0x107E);
        #define F ddigit(0x101E);
        #define G ddigit(0x807E);
        #define H ddigit(0x1199);
        #define I ddigit(0x4466);
        #define J ddigit(0x4436);
        #define K ddigit(0x9218);
        #define L ddigit(0x0078);
        #define M ddigit(0x0A99);
        #define N ddigit(0x8899);
        #define O ddigit(0x00FF);
        #define P ddigit(0x111F);
        #define Q ddigit(0x80FF);
        #define R ddigit(0x911F);
        #define S ddigit(0x8866);
        #define T ddigit(0x4406);
        #define U ddigit(0x00F9);
        #define V ddigit(0x2218);
        #define W ddigit(0xA099);
        #define X ddigit(0xAA00);
        #define Y ddigit(0x4A00);
        #define Z ddigit(0x2266);
        #define _ ch_pos.x += ch_space.x;
        #define s_dot     ddigit(0);
        #define s_minus   ddigit(0x1100);
        #define s_plus    ddigit(0x5500);
        #define s_greater ddigit(0x2800);
        #define s_less    ddigit(0x8200);
        #define s_sqrt    ddigit(0x0C02);
        #define nl1 ch_pos = ch_start;  ch_pos.y -= 3.0;
        #define nl2 ch_pos = ch_start;  ch_pos.y -= 6.0;
        #define nl3 ch_pos = ch_start;	ch_pos.y -= 9.0;

        float dseg(vec2 p0, vec2 p1)
        {
            vec2 dir = normalize(p1 - p0);
            vec2 cp = (uv - ch_pos - p0) * mat2(dir.x, dir.y,-dir.y, dir.x);
            return distance(cp, clamp(cp, vec2(0), vec2(distance(p0, p1), 0)));
        }

        bool bit(int n, int b)
        {
            return mod(floor(float(n) / exp2(floor(float(b)))), 2.0) != 0.0;
        }

        float d = 1e6;

        void ddigit(int n)
        {
            float v = 1e6;
            vec2 cp = uv - ch_pos;
            if (n == 0)     v = min(v, dseg(vec2(-0.405, -1.000), vec2(-0.500, -1.000)));
            if (bit(n,  0)) v = min(v, dseg(vec2( 0.500,  0.063), vec2( 0.500,  0.937)));
            if (bit(n,  1)) v = min(v, dseg(vec2( 0.438,  1.000), vec2( 0.063,  1.000)));
            if (bit(n,  2)) v = min(v, dseg(vec2(-0.063,  1.000), vec2(-0.438,  1.000)));
            if (bit(n,  3)) v = min(v, dseg(vec2(-0.500,  0.937), vec2(-0.500,  0.062)));
            if (bit(n,  4)) v = min(v, dseg(vec2(-0.500, -0.063), vec2(-0.500, -0.938)));
            if (bit(n,  5)) v = min(v, dseg(vec2(-0.438, -1.000), vec2(-0.063, -1.000)));
            if (bit(n,  6)) v = min(v, dseg(vec2( 0.063, -1.000), vec2( 0.438, -1.000)));
            if (bit(n,  7)) v = min(v, dseg(vec2( 0.500, -0.938), vec2( 0.500, -0.063)));
            if (bit(n,  8)) v = min(v, dseg(vec2( 0.063,  0.000), vec2( 0.438, -0.000)));
            if (bit(n,  9)) v = min(v, dseg(vec2( 0.063,  0.063), vec2( 0.438,  0.938)));
            if (bit(n, 10)) v = min(v, dseg(vec2( 0.000,  0.063), vec2( 0.000,  0.937)));
            if (bit(n, 11)) v = min(v, dseg(vec2(-0.063,  0.063), vec2(-0.438,  0.938)));
            if (bit(n, 12)) v = min(v, dseg(vec2(-0.438,  0.000), vec2(-0.063, -0.000)));
            if (bit(n, 13)) v = min(v, dseg(vec2(-0.063, -0.063), vec2(-0.438, -0.938)));
            if (bit(n, 14)) v = min(v, dseg(vec2( 0.000, -0.938), vec2( 0.000, -0.063)));
            if (bit(n, 15)) v = min(v, dseg(vec2( 0.063, -0.063), vec2( 0.438, -0.938)));
            ch_pos.x += ch_space.x;
            d = min(d, v);
        }

        vec3 hsv2rgb_smooth( in vec3 c )
        {
            vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

            rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing

            return c.z * mix( vec3(1.0), rgb, c.y);
        }
        void main( void )
        {
            vec2 aspect = u_resolution.xy / u_resolution.y;
            uv = ( gl_FragCoord.xy / u_resolution.y ) - aspect / 2.0;
            uv *= 15.0 + 3.0*abs(cos(u_time * PI));
            uv = translate(uv, vec2(-7., -1.));
            ch_pos = ch_start;

            S D F _ T E X T

            vec3 color2 = vec3(221.0/255.0, 255.0/255.0, 255.0/255.0);
            vec3 color1 = 0.95*color2; //vec3(255.0/255.0, 235.0/255.0, 183.0/255.0);

            vec2 pos = ( gl_FragCoord.xy / u_resolution.y ) - aspect / 2.0;
            float oke = clamp(circle(pos, 0.3), 0.0, 1.0);

            vec3 color = vec3(-1.0 * oke) + mix(color1, color2, 1.0- ((0.02 + 0.9*abs(sin(u_time * PI))) / d));  // shading
            outputColor = vec4(color, 1.0);
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