//
// Created by sneeuwpop on 13-01-20.
//

#pragma once


#include "../util/interfaces/game_object.h"
#include "../util/splines/line.h"

using namespace glm;

class Stadium : public GameObject
{
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/lib/color.vert", "shaders/lib/color.frag");

    mat4 transform = translate(mat4(1), vec3(0, 1.01, 0));

    float w=10, h=15;
    Line line = Line({scale(rotate(translate(mat4(1), vec3(0-w*0.5, 0, 0-h*0.5)), radians((4-0) * 90 - 45.0f), VEC3::Y), VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(w-w*0.5, 0, 0-h*0.5)), radians((4-1) * 90 - 45.0f), VEC3::Y), VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(w-w*0.5, 0, h-h*0.5)), radians((4-2) * 90 - 45.0f), VEC3::Y), VEC3::ONE),
                      scale(rotate(translate(mat4(1), vec3(0-w*0.5, 0, h-h*0.5)), radians((4-3) * 90 - 45.0f), VEC3::Y), VEC3::ONE)});

    std::vector<vec3> points = {vec3(-1,0,0), vec3(0,1,0), vec3(1,0,0)};

    SharedMesh mesh = SharedMesh(line.wrapMeshAround(&points, true));
    GLuint MVP, u_color;

    bool hover;

public:
    Stadium()
    {
        shader.use();
        MVP = shader.uniformLocation("MVP");
        u_color = shader.uniformLocation("u_color");

//        vec3 a1 = line.getPointPosition(1);
//        vec3 a2 = line.getPointPosition(0);
//        vec3 b1 = line.getPointPosition(2);
//        vec3 b2 = line.getPointPosition(3);
//        mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z, b2.x,b2.y,b2.z,});
//        mesh->nrOfVerts += 6; // TODO not hardcode this 6 here

        VertexBuffer::uploadSingleMesh(mesh);

        colorPickerData = new ColorPickerData{mesh, transform};
    }

    void render();
    void onHover();
    void onClick();

};


