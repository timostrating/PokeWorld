//
// Created by sneeuwpop on 11-01-20.
//

#pragma once


#include <vector>
#include "../../util/interfaces/game_object.h"
#include "../../util/interfaces/system.h"
#include "../../graphics/frame_buffer.h"
#include "../../util/game.h"

class WaterSystem : public System
{
    std::vector<GameObject*> *gameObjects;
    FrameBuffer reflectionFbo, refractionFbo;

public:
    WaterSystem() : reflectionFbo(GAME::g_width, GAME::g_height), refractionFbo(GAME::g_width, GAME::g_height)
    {
        reflectionFbo.addColorTexture(GL_RGB, GL_REPEAT, GL_LINEAR);
        reflectionFbo.addDepthTexture(GL_REPEAT, GL_LINEAR);

        refractionFbo.addColorTexture(GL_RGB, GL_REPEAT, GL_LINEAR);
        refractionFbo.addDepthTexture(GL_REPEAT, GL_LINEAR);
    }

    void update(float deltaTime);

    void bindTexture(ShaderProgram &shader) {
        reflectionFbo.depthTexture->bind(0, shader, "u_depth");
    }

    void setGameObjects(std::vector<GameObject *> *gameObjects_);

    void renderGUI();
};


