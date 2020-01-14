//
// Created by sneeuwpop on 03-01-20.
//

#pragma once


#include <glad/glad.h>
#include <ext.hpp>
#include "../graphics/shader_program.h"
#include "../graphics/mesh.h"
#include "../util/math/math.h"
#include "../util/interfaces/game_object.h"
#include "../graphics/texture.h"
#include "systems/water_system.h"

using namespace glm;
using namespace MATH;

class WaterPlane : public GameObject
{
    mat4 modelMatrix = rotate(scale(translate(mat4(1.0f), vec3(0,0,0)), vec3(20)), radians(-90.0f), VEC3::X);

    ShaderProgram waterShader = ShaderProgram::fromAssetFiles("shaders/water.vert", "shaders/water.frag");
    Texture reflectionTexture = Texture::testCheckerboard();
    GLuint mvpId;

    SharedMesh mesh = SharedMesh(Mesh::quad());
    WaterSystem* waterSystem;

public:
    WaterPlane(WaterSystem *sys) : waterSystem(sys)
    {
        waterShader.use();
        mvpId = waterShader.uniformLocation("MVP");

//        reflectionTexture.bind(0, waterShader, "u_reflectionTexture");

        VertexBuffer::uploadSingleMesh(mesh);
    }

    void render();
};


