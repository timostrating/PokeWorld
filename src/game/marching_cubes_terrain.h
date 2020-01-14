//
// Created by sneeuwpop on 14-10-19.
//

#pragma once

#include "../util/debug/gizmos.h"
#include "../graphics/texture.h"
#include "../util/interfaces/game_object.h"

using namespace glm;

// FOR MORE INFO SEE:  http://paulbourke.net/geometry/polygonise/
class MarchingCubesTerrain : public GameObject
{

private:
    static const int size = 11;
    float noisefield[size][size][size] = {0};

    Gizmos gizmos;
    float falloff = 0.9f;

    ShaderProgram terrainShader = ShaderProgram::fromAssetFiles("shaders/terrain.vert", "shaders/terrain.frag");
//    Texture test = Texture::fromAssetFile("textures/rocks1.jpg");
    GLuint mvpId;

    float modelScale = 5.0f;
    vec3 position = vec3(-5, -9.5, -5);


public:
    SharedMesh mesh = SharedMesh(new Mesh(0, 0)); // Internal variables are our JOB
    mat4 transform = scale(translate(mat4(1.0f), vec3(position.x * modelScale, position.y * modelScale +1, position.z * modelScale)), vec3(modelScale));


    MarchingCubesTerrain();

    void render();
    void debugRender();

    int getCubeIndex(int x, int y, int z, float falloff);
};


