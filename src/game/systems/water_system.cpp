//
// Created by sneeuwpop on 11-01-20.
//

#include <imgui.h>
#include <iostream>
#include "water_system.h"
#include "../../graphics/camera.h"

void WaterSystem::setGameObjects(std::vector<GameObject *> *gameObjects_, MarchingCubesTerrain *terrain_)
{
    terrain = terrain_;
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

    float tmp = Camera::main->farClippingPlane;
    Camera::main->farClippingPlane = 100;
    Camera::main->update();
    refractionFbo.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        waterTerrainShader.use();
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &(Camera::main->combined * terrain->transform)[0][0]);    ;
        terrain->mesh->render();

    refractionFbo.unbind();
    Camera::main->farClippingPlane = tmp;
    Camera::main->update();

}

void WaterSystem::bindTexture(ShaderProgram &shader)
{
    reflectionFbo.colorTexture->bind(0, shader, "u_reflectionTexture");
    refractionFbo.colorTexture->bind(1, shader, "u_refractionTexture");
}

void WaterSystem::renderGUI()
{
    ImGui::Text("WaterSystem:");
    ImGui::Text("");
    reflectionFbo.renderGUI();
    refractionFbo.renderGUI();
}
