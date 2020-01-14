//
// Created by sneeuwpop on 10-01-20.
//

#include <iostream>
#include <imgui.h>
#include "color_picker_system.h"
#include "../../graphics/camera.h"
#include "../../util/input/mouse.h"

void ColorPickerSystem::setGameObjects(std::vector<GameObject *> *gameObjects_)
{
    gameObjects = gameObjects_;
}

void ColorPickerSystem::update(float deltaTime) {
    int x = INPUT::MOUSE::getMousePosX();
    int y = Camera::main->height - INPUT::MOUSE::getMousePosY();

    if (x == 0 || x == Camera::main->width || y == 0 || y == Camera::main->height) // TODO Mouse out of screen
        return;

    fbo.bind();

    glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    flatShader.use();
    int i = 0;
    for (auto &go : *gameObjects)
    {
        i++;
        if (go->colorPickerData == nullptr)
            continue;

        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;
        glUniform4f(u_color, r/255.0f, g/255.0f, b/255.0f, 1);
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &(Camera::main->combined * go->colorPickerData->transform)[0][0]);
        go->colorPickerData->selectionMesh->render();
    }

    /// https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
    glFlush();
    glFinish();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char data[3];
    glReadPixels(x, y, 1,1, GL_RGB, GL_UNSIGNED_BYTE, data);
    int pickedID = data[0] + data[1] * 256 + data[2] * 256*256;

    if (pickedID != 0 && pickedID <= gameObjects->size())
    {
        gameObjects->at(pickedID-1)->onHover();
//        std::cout << "HOVER: ("<<INPUT::MOUSE::getMousePosX() << "," << INPUT::MOUSE::getMousePosY() << ") " << pickedID-1 << "\n";
        if (INPUT::MOUSE::leftClick())
        {
            gameObjects->at(pickedID-1)->onClick();
            std::cout << "clicked on object "<< pickedID-1 <<"\n";
        }
    }

    fbo.unbind();
}

void ColorPickerSystem::renderGUI()
{
    ImGui::Text("ColorPickerSystem:");
    ImGui::Text("");
    fbo.renderGUI();
}
