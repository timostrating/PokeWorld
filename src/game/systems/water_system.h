//
// Created by sneeuwpop on 11-01-20.
//

#pragma once


#include <vector>
#include "../../util/interfaces/game_object.h"
#include "../../util/interfaces/system.h"
#include "../../graphics/frame_buffer.h"
#include "../../util/game.h"
#include "../marching_cubes_terrain.h"

class WaterSystem : public System
{
    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec3 v_pos;

        void main() {
            v_pos = a_pos;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";



    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        out vec4 outputColor;

        in vec3 v_pos;

        void main() {
            float v = (v_pos.y + 0.75);
            outputColor = vec4(v, v, v, 1.0);
        })glsl";

    std::vector<GameObject*> *gameObjects;
    MarchingCubesTerrain *terrain = nullptr;
    FrameBuffer reflectionFbo, refractionFbo;

    ShaderProgram waterTerrainShader = ShaderProgram(vertSource, fragSource);
    GLuint MVP;


public:
    float time = 0.0;

    WaterSystem() : reflectionFbo(800, 800), refractionFbo(800, 800) // TODO: test out
    {
        MVP = waterTerrainShader.uniformLocation("MVP");

        reflectionFbo.addColorTexture();
        reflectionFbo.addDepthBuffer();

        refractionFbo.addColorTexture();
        refractionFbo.addDepthBuffer();
    }

    void setGameObjects(std::vector<GameObject *> *gameObjects_, MarchingCubesTerrain *terrain);

    void update(float deltaTime);

    void bindTexture(ShaderProgram &shader);


    void renderGUI();
};


