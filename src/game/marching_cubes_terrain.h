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

    float falloff = 0.9f;

    ShaderProgram terrainShader = ShaderProgram::fromAssetFiles("shaders/terrain.vert", "shaders/terrain.frag");
//    Texture test = Texture::fromAssetFile("textures/rocks1.jpg");
    GLuint MVP, u_gradient;
    mat4 gradient = mat4(
        20.0/255.0,  15.0/255.0,  21.0/255.0, 0.3,
        100.0/255.0,  66.0/255.0, 101.0/255.0, 0.7,
        108.0/255.0, 130.0/255.0, 169.0/255.0, 1.0,
        0, 0, 0, 0
    );

public:
    SharedMesh mesh = SharedMesh(new Mesh(0, 0, VA_POSITION_NORMAL)); // Internal variables are our JOB
    mat4 transform = mat4(1.0f);


    MarchingCubesTerrain();

    void render(float time);
    void debugRender(Gizmos* gizmos);
    void renderGui();

    int getCubeIndex(int x, int y, int z, float falloff);
};


