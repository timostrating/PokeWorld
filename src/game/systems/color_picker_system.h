//
// Created by sneeuwpop on 10-01-20.
//

#pragma once

#include <vector>
#include "../../util/interfaces/system.h"
#include "../../util/interfaces/game_object.h"
#include "../../graphics/shader_program.h"
#include "../../graphics/frame_buffer.h"
#include "../../util/game.h"

class ColorPickerSystem : public System
{
    std::vector<GameObject*> *gameObjects;
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");
    GLint u_color, MVP;
    FrameBuffer fbo;


public:
    // TODO resize
    ColorPickerSystem() : fbo(GAME::g_width, GAME::g_height)
    {
        fbo.addColorBuffer(GL_RGB);
        fbo.addDepthBuffer();

        u_color = flatShader.uniformLocation("u_color");
        MVP = flatShader.uniformLocation("MVP");
    }

    void setGameObjects(std::vector<GameObject*> *gameObjects_);

    void update(float deltaTime);
    void renderGUI();
};


