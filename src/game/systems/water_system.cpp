//
// Created by sneeuwpop on 11-01-20.
//

#include <imgui.h>
#include <iostream>
#include "water_system.h"
#include "../../graphics/camera.h"

void WaterSystem::setGameObjects(std::vector<GameObject *> *gameObjects_)
{
    gameObjects = gameObjects_;
}

void WaterSystem::update(float deltaTime)
{
    Camera::main->position.y -= 2 * Camera::main->position.y; // TODO: underwater
    Camera::main->invertPitch();
    Camera::main->update();
    reflectionFbo.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &go : *gameObjects) {
            go->render();
        }

    reflectionFbo.unbind();
    Camera::main->position.y = -Camera::main->position.y;
    Camera::main->invertPitch();
    Camera::main->update();

    refractionFbo.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &go : *gameObjects) {
            go->render();
        }

    refractionFbo.unbind();
}

void WaterSystem::renderGUI()
{
    ImGui::Text("WaterSystem:");
    ImGui::Text("");
    reflectionFbo.renderGUI();
    refractionFbo.renderGUI();
}
