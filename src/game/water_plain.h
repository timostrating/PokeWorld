//
// Created by sneeuwpop on 03-01-20.
//

#pragma once


#include <glad/glad.h>
#include <ext.hpp>
#include "../graphics/shader_program.h"
#include "../graphics/mesh.h"
#include "../util/math/math.h"

using namespace glm;

class WaterPlain
{
    mat4 modelMatrix = rotate(scale(translate(mat4(1.0f), vec3(0,-1,0)), vec3(100)), radians(-90.0f), VEC3::X);

    ShaderProgram waterShader = ShaderProgram::fromAssetFiles("shaders/water.vert", "shaders/water.frag");
    GLuint mvpId;

    SharedMesh mesh = SharedMesh(Mesh::quad());


public:
    WaterPlain()
    {
        mvpId = waterShader.uniformLocation("MVP");
        VertexBuffer::uploadSingleMesh(mesh);
    }

    void render();
    void debugRender();

};


