//
// Created by sneeuwpop on 13-01-20.
//

#pragma once


#include <jmorecfg.h>
#include "../util/interfaces/game_object.h"
#include "../util/splines/line.h"

using namespace glm;

class Stadium : public GameObject
{
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    vec3 pos = vec3(1,1,1);
    mat4 transform = glm::translate(mat4(1), vec3(pos));

    Line line = Line({scale(rotate(translate(mat4(1), pos + vec3(0,0,0)), radians((4-0) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(1,0,0)), radians((4-1) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(1,0,1)), radians((4-2) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE),
                      scale(rotate(translate(mat4(1), pos + vec3(0,0,1)), radians((4-3) * 90 - 45.0f), VEC3::Y), 0.1f * VEC3::ONE)});

    std::vector<vec3> points = {vec3(-1,0,0), vec3(0,1,0), vec3(1,0,0)};

    SharedMesh mesh = SharedMesh(line.wrapMeshAround(&points));
    GLuint MVP, u_color;

    boolean hover;

public:
    Stadium()
    {
        shader.use();
        MVP = shader.uniformLocation("MVP");
        u_color = shader.uniformLocation("u_color");

        vec3 a1 = line.getPointPosition(1);
        vec3 a2 = line.getPointPosition(0);
        vec3 b1 = line.getPointPosition(2);
        vec3 b2 = line.getPointPosition(3);
        mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z, b2.x,b2.y,b2.z,});
        mesh->nrOfVerts += 6; // TODO not hardcode this 6 here

        VertexBuffer::uploadSingleMesh(mesh);

        colorPickerData = new ColorPickerData{mesh, transform};
    }

    void render();
    void onHover();
    void onClick();

};


