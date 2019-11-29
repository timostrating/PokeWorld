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

        void main() {
            if (sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0)) <= 200.0 ) {
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

//                u -= u_time * 0.05;
                float light = clamp(dot(normal, lightdir), 0.0, 1.0);
                light *= 1.1;
                light += 0.05;

                vec4 texCol = texture(u_texture, vec2(u,v)) * 4.0f;

                outputColor = light * texCol * vec4(255.0/255.0, 142.0/255.0, 74.0/255.0, 1.0);

            } else {
                float t = sqrt(pow(gl_FragCoord.x - 400.0, 2.0) + pow(gl_FragCoord.y - 400.0, 2.0)) / sqrt(pow(400.0, 2.0) + pow(400.0, 2.0));
                vec4 colorA = vec4( 4.0/255.0,  8.0/255.0, 14.0/255.0, 1.0);
                vec4 colorB = vec4(20.0/255.0, 66.0/255.0, 92.0/255.0, 1.0);

                     if (abs(gl_FragCoord.x - 373.0) < 1.0 && abs(gl_FragCoord.y - 111.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 113.0) < 1.0 && abs(gl_FragCoord.y - 382.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 182.0) < 1.0 && abs(gl_FragCoord.y - 773.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 234.0) < 1.0 && abs(gl_FragCoord.y - 317.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 681.0) < 1.0 && abs(gl_FragCoord.y - 736.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 789.0) < 1.0 && abs(gl_FragCoord.y - 612.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 278.0) < 1.0 && abs(gl_FragCoord.y - 171.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 78.0)  < 1.0 && abs(gl_FragCoord.y - 571.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 48.0)  < 1.0 && abs(gl_FragCoord.y - 121.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 791.0) < 1.0 && abs(gl_FragCoord.y - 91.0)  < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 123.0) < 1.0 && abs(gl_FragCoord.y - 571.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 673.0) < 1.0 && abs(gl_FragCoord.y - 792.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 571.0) < 1.0 && abs(gl_FragCoord.y - 71.0)  < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 131.0) < 1.0 && abs(gl_FragCoord.y - 123.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else if (abs(gl_FragCoord.x - 780.0) < 1.0 && abs(gl_FragCoord.y - 221.0) < 1.0) { outputColor = vec4(0.8, 0.8, 0.9, 1.0); }
                else {
                    outputColor = mix(colorB, colorA, t);
                }
            }
        })glsl";

    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    Texture texture = Texture::fromAssetFile("textures/heightmap2.jpg");

    Gizmos gizmos;
    SharedMesh quad = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();
        MVPLocation = shader.uniformLocation("MVP");
        texture.bind(0, shader, "u_texture");

        VertexBuffer::uploadSingleMesh(quad);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime)
    {
        time += deltaTime;
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor( 4.0/255.0,  8.0/255.0, 14.0/255.0, 1.0f);
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