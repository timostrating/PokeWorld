//
// Created by sneeuwpop on 18-11-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/debug/gizmos.h"
#include "../graphics/texture.h"

using namespace glm;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        void main() {
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";









    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;
        #define PI 3.14159265358979

        out vec4 outputColor;

        uniform sampler2D u_texture;
        uniform sampler2D u_texture2;
        uniform float u_time;

        vec3 getNormal(in vec2 sphereCenter, in float sphereRadius, in vec2 point){
            // Need to figure out how far the current point is from the center to lerp it
            float distFromCenter = distance(point, sphereCenter);
            float weight = distFromCenter/sphereRadius;

            // Z component is zero since at the edge the normal will be on the XY plane
            vec3 edgeNormal = vec3(point - sphereCenter, 0);
            edgeNormal = normalize(edgeNormal);

            // We know the normal at the center of the sphere points directly at the viewer,
            // so we can use that in our mix/lerp.
            return mix(vec3(0,0,1), edgeNormal, weight);
        }

        float rand(vec2 co){
            return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
        }

        void main() {
            float size = sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0));

            if (size <= 210.0 ) {
                vec3 lightdir = normalize(vec3(cos(u_time), 0.0, sin(u_time)));

                vec3 normal = getNormal(vec2(400.0, 400.0), 200.0f, gl_FragCoord.xy);

                vec3 northVector = vec3(0, 1, 0);
                vec3 eastVector  = vec3(1, 0, 0);
                vec3 vertPoint = normal;


                float lat = acos(dot(northVector, normal));
                float v = lat / PI;
                float u;

                float lon = (acos( dot( vertPoint, eastVector) / sin(lat))) / (2.0 * PI);
                if(dot(cross(northVector, eastVector), vertPoint) > 0.0){
                    u = lon;
                }
                else{
                    u = 1.0 - lon;
                }

                u -= u_time * 0.02;
                float light = clamp(dot(normal, lightdir), 0.0, 1.0);
                light *= 1.5;
                light += 0.1;

                vec4 texCol = texture(u_texture, vec2(u,v));

                if (texCol.b > texCol.r + texCol.g)
                    texCol = 0.4 * texCol + 0.8 * light * vec4(16.0/255.0, 30.0/255.0, 40.0/255.0, 1.0);

                if (size <= 200.0)
                    outputColor = light * texCol + light * 0.2 * texture(u_texture2, vec2(u - u_time * 0.02, v)) + vec4(.0,.0,.0,1.0);

                else {
                    float t = (size - 200.0) / 10.0;
                    vec3 colorA = vec3(253.0/255.0, 253.0/255.0, 253.0/255.0);
                    vec3 colorB = vec3(20.0/255.0, 66.0/255.0, 92.0/255.0);

                    light -= 0.4;
                    light *= 0.2;

                    outputColor = vec4(mix(colorA, colorB, t), light);
                }

            } else {
                float t = sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0)) / sqrt(pow(400.0, 2.0) + pow(400.0, 2.0));
                vec3 colorA = vec3( 7.0/255.0, 16.0/255.0, 21.0/255.0);
                vec3 colorB = vec3(24.0/255.0, 42.0/255.0, 55.0/255.0);

                vec3 v = vec3(.0);
                if (abs(rand(vec2(gl_FragCoord.x, gl_FragCoord.y))) < 0.0002) { v = vec3(0.5, 0.5, 0.8); }

                outputColor = vec4(mix(colorB, colorA, t) + v, 1.0);
            }
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    Texture texture = Texture::fromAssetFile("textures/earth.jpg");
    Texture texture2 = Texture::fromAssetFile("textures/turbulance.jpg");

    Gizmos gizmos;
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        MVPLocation = shader.uniformLocation("MVP");
        texture.bind(0, shader, "u_texture");
        texture2.bind(1, shader, "u_texture2");

        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(18.0/255.0, 32.0/255.0, 42.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 0, + -1.0f / tanf(radians(22.5f)));
            camera.lookAt(VEC3::ZERO);
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        shader.use();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * translate(mat4(1.0f), vec3(0, 0, 0)) )[0][0]);
        quad->render();

    }

    void drawStar() {

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};