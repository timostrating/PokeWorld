//
// Created by sneeuwpop on 30-09-19.
//

#pragma once

#include <glm/glm.hpp>
#include "../../graphics/mesh.h"
#include "../../graphics/shader_program.h"
#include "../math/math.h"

using namespace glm;

class Gizmos
{
private:
    ShaderProgram shaderProgram;
    SharedMesh mesh;
    GLint fromId, toId, mvpId, colorId;

public:
    Gizmos();

    void drawLine(const vec3 &from, const vec3 &to, const vec4 &color = COLOR::DEFAULT);
    void drawCube(const vec3 &pos, const float radius, const vec4 &color = COLOR::DEFAULT);
    void drawBox(const vec3 &pos, const vec3 radius, const vec4 &color = COLOR::DEFAULT);
};


