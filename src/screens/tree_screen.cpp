//
// Created by sneeuwpop on 12-09-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <stack>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../graphics/mesh.h"
#include "../util/input/keyboard.h"
#include "../game/lsystem.h"
#include "../util/buffer_line_drawer.h"
using namespace glm;

class TreeScreen : public Screen
{
public:
    FlyingCamera camera = FlyingCamera();
    GLint MVPLocation;

    BufferLineDrawer lineDrawer = BufferLineDrawer();
    float rotation = 25; // in degrees

    TreeScreen()
    {
        // Shader Program
        ShaderProgram shaderProgram = ShaderProgram::fromAssetFiles("shaders/default.vert", "shaders/default.frag");
        shaderProgram.begin();

        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', "F[-X][+X],FX");

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 direction = vec3(0, 0.2f, 0);

        std::stack<vec3> memory = std::stack<vec3>();

        lSystem.applyNtimes(5);

        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (str[i]) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (direction); lineDrawer.drawLine(oldPoint, curPoint); break;
                case '+': direction = glm::rotate(direction, radians(rotation), VEC3::Z); break;
                case '-': direction = glm::rotate(direction, -radians(rotation), VEC3::Z); break;
                case '[': memory.push(curPoint); memory.push(direction); break;
                case ']': direction = memory.top(); memory.pop();
                          curPoint = memory.top(); memory.pop(); break;
            }
        }

        lineDrawer.upload();

        // Model View Projection
        MVPLocation = glGetUniformLocation(shaderProgram.getId(), "MVP");
    }

    double time = 0;
    bool anyKeyPressed = false;
    mat4 modelMatrix = translate(mat4(1.0f), vec3(0, 0, 0)); // identity matrix

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed)
            camera.update(deltaTime);
        else {
            if (INPUT::KEYBOARD::anyKeyEverPressed())
                anyKeyPressed = true;
            camera.position = vec3(sin(time * 0.5) *3,  1,  cos(time * 0.5) *3);
            camera.lookAt(VEC3::Y);
            camera.Camera::update();
        }

        mat4 mvp = camera.combined * modelMatrix;
        glUniformMatrix4fv( MVPLocation, 1, GL_FALSE, &mvp[0][0]);

        lineDrawer.render();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};