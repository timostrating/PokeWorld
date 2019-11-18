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

using namespace glm;

class StarsScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    ShaderProgram defaultShaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");

    Gizmos gizmos;

    static const int SIZE = 100;
    vec3 data[SIZE];

    StarsScreen()
    {
        // Shader Program
        defaultShaderProgram.use();

        // Model View Projection
        MVPLocation = defaultShaderProgram.uniformLocation("MVP");

        for (int i=0; i < SIZE; i++) {
            data[i] = MATH::randomVec3(-1, 1);
        }
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    mat4 modelMatrix = translate(mat4(1.0f), vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(20.0/255.0, 30.0/255.0, 40.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        defaultShaderProgram.use(); // imgui may have changed the current shader

        camera.position = vec3(0, 0, -2);
        camera.lookAt(0, 0, 0);
        camera.update(deltaTime);

        mat4 mvp = camera.combined * modelMatrix;
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &mvp[0][0]);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        for (int i=0; i<SIZE; i++) {
            drawStar(data[i]);
        }
    }

    void drawStar(vec3 pos) {
        if (MATH::random() < 0.1f)
            return;

        gizmos.drawLine(pos - pos.z * 0.1f * VEC3::Y,        pos + pos.z * 0.1f * VEC3::Y, COLOR::WHITE);
        gizmos.drawLine(pos - pos.z * 0.1f * VEC3::X,        pos + pos.z * 0.1f * VEC3::X, COLOR::WHITE);
        gizmos.drawLine(pos - pos.z * 0.07f * vec3(1,1,0),   pos + pos.z * 0.07f * vec3(1,1,0), COLOR::WHITE);
        gizmos.drawLine(pos - pos.z * 0.07f * vec3(-1,1,0),  pos + pos.z * 0.07f * vec3(-1,1,0), COLOR::WHITE);

    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};