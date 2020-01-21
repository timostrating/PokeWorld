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
    mat4 modelMatrix = scale(translate(mat4(1.0f), vec3(-10,-2.5,10)), vec3(10.99, 2.5, 10.99));

    ShaderProgram waterShader = ShaderProgram::fromAssetFiles("shaders/water.vert", "shaders/water.frag");
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");
    Texture waterNormal = Texture::fromAssetFile("textures/water-dudv.jpg");
    GLuint mvpId;

    SharedMesh cube = SharedMesh(Mesh::cube());
    WaterSystem* waterSystem;

public:
    WaterPlane(WaterSystem *sys) : waterSystem(sys)
    {
        waterShader.use();
        mvpId = waterShader.uniformLocation("MVP");

//        reflectionTexture.bind(0, waterShader, "u_reflectionTexture");

        VertexBuffer::uploadSingleMesh(cube);

        flatShader.use();
    }

    void render();
};


