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
        189.0/255.0,  168.0/255.0,  115.0/255.0, 0.2,
        159.0/255.0,  116.0/255.0, 72.0/255.0, 0.21,
        146.0/255.0, 92.0/255.0, 50.0/255.0, 0.8,
        28.0/255.0, 22.0/255.0, 43.0/255.0, 0.81
    );

public:
    SharedMesh mesh = SharedMesh(new Mesh(0, 0, VA_POSITION_NORMAL)); // Internal variables are our JOB
    mat4 transform = mat4(1.0f);


    MarchingCubesTerrain();

    void renderClipped(float time, float clipHeight);
    void renderReflection(float time);

    void render(float time);
    void debugRender(Gizmos* gizmos);
    void renderGui();

    int getCubeIndex(int x, int y, int z, float falloff);
};


