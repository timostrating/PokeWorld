//
// Created by sneeuwpop on 10-01-20.
//

#pragma once

#include "../../graphics/mesh.h"
#include "../debug/gizmos.h"
#include <glm/glm.hpp>

using namespace glm;

struct ColorPickerData
{
    SharedMesh selectionMesh = nullptr;
    mat4 transform = mat4(1);
};

class GameObject
{
public:
    ColorPickerData *colorPickerData = nullptr;

    virtual void render(float time) {}
    virtual void renderReflection(float time) { render(time); }
    virtual void debugRender(Gizmos* gizmos) {}
    virtual void renderGui() {}

    virtual void onHover() {}
    virtual void onClick() {}
};


