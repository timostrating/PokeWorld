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
            float v = (v_pos.y - 8.0) * 0.5;
            outputColor = vec4(v, v, v, 1.0);
        })glsl";

    std::vector<GameObject*> *gameObjects;
    MarchingCubesTerrain *terrain = nullptr;
    FrameBuffer reflectionFbo, refractionFbo;

    ShaderProgram waterTerrainShader = ShaderProgram(vertSource, fragSource);
    GLuint MVP;

public:
    WaterSystem() : reflectionFbo(GAME::g_width, GAME::g_height), refractionFbo(GAME::g_width, GAME::g_height)
    {
        MVP = waterTerrainShader.uniformLocation("MVP");

        reflectionFbo.addColorTexture(GL_RGB, GL_REPEAT, GL_LINEAR);
        reflectionFbo.addDepthBuffer();

        refractionFbo.addColorTexture(GL_RGB, GL_REPEAT, GL_LINEAR);
        refractionFbo.addDepthBuffer();
    }

    void setGameObjects(std::vector<GameObject *> *gameObjects_, MarchingCubesTerrain *terrain);

    void update(float deltaTime);

    void bindTexture(ShaderProgram &shader);


    void renderGUI();
};


