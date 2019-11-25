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
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/flat_texture.vert", "shaders/lib/flat_texture.frag");
    Texture texture = Texture::fromAssetFile("textures/pokemon-stadium.png");

    Gizmos gizmos;
    SharedMesh mesh = SharedMesh(Mesh::quad());

public:
    EverydayScreen()
    {
        shader.use();

        MVPLocation = shader.uniformLocation("MVP");

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void setup(GLFWwindow* window) {}

    double time = 0;

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(104.0/255.0, 192.0/255.0, 105.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////// CAMERA

        shader.use();

        camera.position = vec3(sin(time * 0.3f) *4, 1, cos(time * 0.3f) *4);;
        camera.lookAt(VEC3::ZERO);
        camera.Camera::update();

        texture.bind(0, shader, "u_texture");

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &(camera.combined * (translate(mat4(1.0f), vec3(0, 0, 0))) )[0][0]);
        mesh->render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};