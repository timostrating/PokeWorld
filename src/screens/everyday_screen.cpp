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

class EverydayScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/library/flat_color.vert", "shaders/library/flat_color.frag");
    ShaderProgram shader2 = ShaderProgram::fromAssetFiles("shaders/library/flat_color.vert", "shaders/library/flat_color.frag");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());
    SharedMesh mesh2 = SharedMesh(Mesh::cube());

    static const int SIZE = 500;
    vec3 data[SIZE];

    EverydayScreen()
    {
        // Shader Program
        shader.use();

        // Model View Projection
        MVPLocation = shader.uniformLocation("MVP");
        glUniform3f(shader.uniformLocation("color"), 34.0f/255.0, 139.0f/255.0, 34.0f/255.0);

        shader2.use();
        glUniform3f(shader2.uniformLocation("color"), 139.0f/255.0, 69.0f/255.0, 19.0f/255.0);

        VertexBuffer::uploadSingleMesh(mesh);
        VertexBuffer::uploadSingleMesh(mesh2);

        for (int i=0; i<SIZE; i++)
            data[i] = MATH::randomVec3(-1, 1);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;
    mat4 modelMatrix = rotate(translate(mat4(1.0f), vec3(0, 0, 0)), radians(90.0f), VEC3::X);
    mat4 modelMatrix2 = scale(rotate(translate(mat4(1.0f), vec3(0, 0.4f, 0)), radians(90.0f), VEC3::X), vec3(0.01f, 0.01f, 0.4f));

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor((50+20.0)/255.0, (50+30.0)/255.0, (50+40.0)/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(sin(time * 0.5) *3,  1,  cos(time * 0.5) *3);
        camera.lookAt(0.5f * VEC3::Y);
        camera.Camera::update();

        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &(camera.combined * modelMatrix)[0][0]);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        mesh->render();

        shader2.use();
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &(camera.combined * modelMatrix2)[0][0]);

        mesh2->render();

        for (int i=0; i<SIZE; i++)
        {
            float height = (static_cast<float>(i) / static_cast<float>(SIZE));
            gizmos.drawLine(height * VEC3::Y + vec3(0, 0.5f, 0), (1.0f - height) * 0.4f * data[i] + + vec3(0, 0.5f, 0), COLOR::GREEN);
        }
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};