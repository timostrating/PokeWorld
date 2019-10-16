//
// Created by sneeuwpop on 14-10-19.
//

#pragma once


#include "../util/debug/gizmos.h"

// FOR MORE INFO SEE:  http://paulbourke.net/geometry/polygonise/
class MarchingCubesTerrain
{

private:
    static const int size = 11;
    float noisefield[size][size][size] = {0};

    Gizmos gizmos;
    float falloff = 0.9f;

    SharedMesh mesh = SharedMesh(new Mesh(0, 0)); // Internal variables are our JOB

    ShaderProgram terrainShader = ShaderProgram::fromAssetFiles("shaders/terrain.vert", "shaders/terrain.frag");
    GLuint mvpId;

    float scale = 0.3f;
    mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), vec3(-5 * scale, -9.5 * scale, -5 * scale)), vec3(scale)); // identity matrix


public:
    MarchingCubesTerrain();

    void render();
    void debugRender();

    int getCubeIndex(int x, int y, int z, float falloff);
};


