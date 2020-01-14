//
// Created by sneeuwpop on 13-01-20.
//

#pragma once


#include "../util/interfaces/game_object.h"
#include "../util/splines/line.h"

using namespace glm;

class Stadium : public GameObject
{
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/skybox.vert", "shaders/skybox.frag");

    vec3 pos = vec3(1,1,1);
    mat4 transform = glm::translate(mat4(1), vec3(pos));

    Line line = Line({scale(rotate(translate(mat4(1), pos + vec3(0,0,0)), radians((4-0) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(1,0,0)), radians((4-1) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(1,0,1)), radians((4-2) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(0,0,1)), radians((4-3) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE)});

    std::vector<vec3> points = {vec3(-1,0,0), vec3(0,1,0), vec3(1,0,0)};

    SharedMesh mesh = SharedMesh(line.wrapMeshAround(&points));
    GLuint mvpId;

public:
    Stadium()
    {
        shader.use();
        mvpId = shader.uniformLocation("MVP");

        VertexBuffer::uploadSingleMesh(mesh);

        colorPickerData = new ColorPickerData{mesh, transform};
    }

    void render();
    void onClick();

};


